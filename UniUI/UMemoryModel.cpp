#include "UMemoryModel.h"

#include <QTimer>

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

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(100);

}

UMemoryModel::~UMemoryModel()
{
    lua_close(L_);
}

int UMemoryModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return currentRowCount_;
}

int UMemoryModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return columnInfos_.size();
}

static int traceback (lua_State *L) {
    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
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
        else
        {
            //查看是否有该行的脚本。
            lua_settop(L_,0);
            //QString script;
            //script = QString("return function (address) %1 end").arg(columnInfos_.at(column).getDataScript);
            
            lua_getglobal(L_,QString("memory_model_get_data_%1").arg(column).toLocal8Bit());
            if(lua_type(L_,-1) != LUA_TFUNCTION)
            {
                return QVariant();
            }

            lua_pushcfunction(L_,traceback);
            lua_insert(L_,1);
            lua_pushinteger(L_,address);
            
            int to = lua_gettop(L_);
            int result = lua_pcall(L_,1,1,1);
            to = lua_gettop(L_);
            lua_remove(L_,1);
            if(result == LUA_ERRRUN)
            {
                UTRACE("UMemoryView")<<"Runtime Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            else if(result == LUA_ERRMEM)
            {
                UTRACE("UMemoryView")<<"Memory Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            else if(result == LUA_ERRERR)
            {
                UTRACE("UMemoryView")<<"Error Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            int a = lua_gettop(L_);
            const char *str = luaL_checkstring(L_,1);
            return QString::fromLocal8Bit(luaL_checkstring(L_,1));
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        DataColorFunction dataColorFunction = columnInfos_.at(column).dataColorFunction;
        if(dataColorFunction)
        {
            return dataColorFunction(address);
        }
        else
        {
            lua_settop(L_,0);
            //QString script;
            //script = QString("return function (address) %1 end").arg(columnInfos_.at(column).getDataScript);

            lua_getglobal(L_,QString("memory_model_data_color_%1").arg(column).toLocal8Bit());
            if(lua_type(L_,-1) != LUA_TFUNCTION)
            {
                return QVariant();
            }

            lua_pushcfunction(L_,traceback);
            lua_insert(L_,1);
            lua_pushinteger(L_,address);

            int to = lua_gettop(L_);
            int result = lua_pcall(L_,1,1,1);
            to = lua_gettop(L_);
            lua_remove(L_,1);
            if(result == LUA_ERRRUN)
            {
                UTRACE("UMemoryView")<<"Runtime Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            else if(result == LUA_ERRMEM)
            {
                UTRACE("UMemoryView")<<"Memory Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            else if(result == LUA_ERRERR)
            {
                UTRACE("UMemoryView")<<"Error Error:"<<luaL_checkstring(L_,-1);
                return QVariant();
            }
            int a = lua_gettop(L_);
            const char *str = luaL_checkstring(L_,1);
            return QColor(QString::fromLocal8Bit(luaL_checkstring(L_,1)));
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
    compileScripts();
    endInsertColumns();
    emit columnInfoChanged();
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

bool UMemoryModel::removeColumns( int column, int count, const QModelIndex &parent /*= QModelIndex()*/ )
{
    beginRemoveColumns(parent,column,column+count-1);
    for(int i = 0; i < count; i++)
    {
        columnInfos_.removeAt(column);
    }
    compileScripts();
    endRemoveColumns();
    emit columnInfoChanged();
    return true;
}

void UMemoryModel::compileScripts()
{        
    QStringList scripts;
    QString script;
    for(int i = 0; i < columnInfos_.size(); i++)
    {

        if(columnInfos_[i].getDataScript.size())
        {
            script = QString("function memory_model_get_data_%1(address) %2 end")
                .arg(i).arg(columnInfos_[i].getDataScript);
            scripts.push_back(script);
        }
        if(columnInfos_[i].setDataScript.size())
        {
            script = QString("function memory_model_set_data_%1(address,value) %2 end")
                .arg(i).arg(columnInfos_[i].setDataScript);
            scripts.push_back(script);
        }
        if(columnInfos_[i].dataColorScript.size())
        {
            script = QString("function memory_model_data_color_%1(address) %2 end")
                .arg(i).arg(columnInfos_[i].dataColorScript);
            scripts.push_back(script);
        }
    }
    foreach(QString script,scripts)
    {
        lua_settop(L_,0);
        int result = luaL_loadstring(L_,script.toLocal8Bit());
        if(result == LUA_ERRSYNTAX)
        {
            UTRACE("UMemoryView")<<"Syntax Error:"<<luaL_checkstring(L_,-1);
            continue;
        }
        else if(result == LUA_ERRMEM)
        {
            UTRACE("UMemoryView")<<"Memory Error:"<<luaL_checkstring(L_,-1);
            continue;
        }

        lua_pushcfunction(L_,traceback);
        lua_insert(L_,1);

        result = lua_pcall(L_,0,0,1);
        lua_remove(L_,1);
        if(result == LUA_ERRRUN)
        {
            UTRACE("UMemoryView")<<"Runtime Error:"<<luaL_checkstring(L_,-1);
            continue;
        }
        else if(result == LUA_ERRMEM)
        {
            UTRACE("UMemoryView")<<"Memory Error:"<<luaL_checkstring(L_,-1);
            continue;
        }
        else if(result == LUA_ERRERR)
        {
            UTRACE("UMemoryView")<<"Error Error:"<<luaL_checkstring(L_,-1);
            continue;
        }
    }
}

void UMemoryModel::update()
{
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

void UMemoryModel::setColumnInfo( int column,ColumnInfo columnInfo )
{
    columnInfos_[column] = columnInfo;
    compileScripts();
    emit headerDataChanged(Qt::Horizontal,column,column);
    emit dataChanged(index(0,column),index(rowCount()-1,column));
    emit columnInfoChanged();
}

void UMemoryModel::runLuaCFunction( lua_CFunction function )
{
    function(L_);
}


QDataStream & operator<<( QDataStream& stream, const UMemoryModel::ColumnInfo& columnInfo )
{
    stream<<columnInfo.title;
    stream<<columnInfo.getDataScript;
    stream<<columnInfo.setDataScript;
    stream<<columnInfo.dataColorScript;
    return stream;
}

QDataStream & operator>>( QDataStream& stream, UMemoryModel::ColumnInfo& columnInfo )
{
    stream>>columnInfo.title;
    stream>>columnInfo.getDataScript;
    stream>>columnInfo.setDataScript;
    stream>>columnInfo.dataColorScript;
    return stream;
}

QDataStream & operator>>( QDataStream& stream, UMemoryModel& model )
{
    stream>>model.columnInfos_;
    model.compileScripts();
    return stream;
}

QDataStream & operator<<( QDataStream& stream, const UMemoryModel& model )
{
    stream<<model.columnInfos_;
    return stream;
}


}//namespace uni