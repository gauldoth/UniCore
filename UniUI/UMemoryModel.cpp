#include "UMemoryModel.h"

#include "../UniCore/ULog.h"
#include "../UniCore/ULua.h"
#include "../UniCore/UMemory.h"


namespace uni
{

QString UMemoryModel_GetAddress(int address)
{
    return QString("%1").arg((unsigned long)address,8,16,QChar('0'));
}

QString UMemoryModel_GetHex(int address)
{
    int hex = 0;
    try
    {
        hex = GetAt<int>(address,0);
    }
    catch (...)
    {
    	
    }
    return QString("%1").arg((unsigned long)hex,8,16,QChar('0'));
}  

void UMemoryModel_SetHex(int address,const QString &data)
{

}

QColor UMemoryModel_GetHexColor(int address)
{
    try
    {
        int hex = GetAt<int>(address,0);
        return Qt::white;
    }
    catch (...)
    {
        return Qt::black;
    }
}

QString UMemoryModel_GetInt(int address)
{
    int data = GetAt<int>(address,0);
    return QString::number(data);
}

void UMemoryModel_SetInt(int address,const QString &data)
{

}

UMemoryModel::UMemoryModel(QObject *parent /*= 0*/)
:QAbstractTableModel(parent)
,baseAddress_(0)
,currentRowCount_(0)
{
    L_ = lua_open();

    lua_gc(L_,LUA_GCSTOP,0);
    luaL_openlibs(L_);
    RegisterCommonLuaFunctions(L_);
    lua_gc(L_,LUA_GCRESTART,0);
}

UMemoryModel::~UMemoryModel()
{

}

int UMemoryModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return currentRowCount_;
}

int UMemoryModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return columnInfos_.size();
}

QVariant UMemoryModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    //当前进程，直接从内存中读取数据。
    int row = index.row();
    int column = index.column();
    int address = baseAddress_+row*RowStep;
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {

        GetDataFunction getFunction = columnInfos_.at(column).getDataFunction;
        if(getFunction)
        {
            return getFunction(address);
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        DataColorFunction dataColorFunction = columnInfos_.at(column).dataColorFunction;
        if(dataColorFunction)
        {
            return dataColorFunction(address);
        }
    }
    return QVariant();
}

void UMemoryModel::fetchMore(const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), currentRowCount_, currentRowCount_+PageSize-1);
    currentRowCount_ += PageSize;
    endInsertRows();
}

bool UMemoryModel::canFetchMore( const QModelIndex &parent ) const
{
    if(currentRowCount_+PageSize >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UMemoryModel::setAddress( int address )
{
    baseAddress_ = address;
    reset();
}

void UMemoryModel::addColumnInfo(ColumnInfo columnInfo)
{
    beginInsertColumns(QModelIndex(),columnCount(),columnCount());
    columnInfos_.push_back(columnInfo);  
    endInsertColumns();
}

QVariant UMemoryModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            return columnInfos_[section].title;
        }
        else
        {
            return QString("+%1").arg(section*RowStep,0,16);
        }
    }
    return QVariant();
}

void UMemoryModel::initCommonColumn()
{
    //地址。
    while(columnInfos_.size() < 2)
    {
        columnInfos_.append(ColumnInfo());
    }
    columnInfos_[0].title = "Addr";
    columnInfos_[0].getDataFunction = UMemoryModel_GetAddress;

    //取4位16进制。
    columnInfos_[1].title = "Hex";
    columnInfos_[1].getDataFunction = UMemoryModel_GetHex;
    columnInfos_[1].dataColorFunction = UMemoryModel_GetHexColor;
}

QDataStream & operator<<( QDataStream& stream, const UMemoryModel::ColumnInfo& columnInfo )
{
    stream<<columnInfo.title;
    stream<<columnInfo.getDataScript;
    stream<<columnInfo.setDataScript;
    return stream;
}

QDataStream & operator>>( QDataStream& stream, UMemoryModel::ColumnInfo& columnInfo )
{
    stream>>columnInfo.title;
    stream>>columnInfo.getDataScript;
    stream>>columnInfo.setDataScript;
    return stream;
}

QDataStream & operator>>( QDataStream& stream, UMemoryModel& model )
{
    stream>>model.columnInfos_;
    return stream;
}

QDataStream & operator<<( QDataStream& stream, const UMemoryModel& model )
{
    stream<<model.columnInfos_;
    return stream;
}


}//namespace uni