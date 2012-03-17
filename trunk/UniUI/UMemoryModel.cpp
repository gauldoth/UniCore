#include "UMemoryModel.h"

#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"

namespace uni
{

QString UMemoryModel_GetAddress(int address)
{
    return QString("%1").arg((unsigned long)address,8,16,QChar('0'));
}

QString UMemoryModel_GetHex(int address)
{
    int hex = GetAt<int>(address,0);
    return QString("%1").arg((unsigned long)hex,8,16,QChar('0'));
}

void UMemoryModel_SetHex(int address,const QString &data)
{

}

UMemoryModel::UMemoryModel( QObject *parent /*= 0*/ ,lua_State *state)
:QAbstractTableModel(parent)
,baseAddress_(0)
,currentRowCount_(0)
,luaState_(state)
{
    if(!luaState_)
    {
        //假如没有传入Lua状态，则自己建个私有的Lua状态。
        luaState_ = lua_open();

        lua_gc(luaState_,LUA_GCSTOP,0);
        luaL_openlibs(luaState_);
        lua_gc(luaState_,LUA_GCRESTART,0);
    }
    addColumnInfo("Address",UMemoryModel_GetAddress,0);
    addColumnInfo("Hex",UMemoryModel_GetHex,UMemoryModel_SetHex);
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
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int address = baseAddress_+row*RowStep;

        QString (*getFunction)(int) = columnInfos_[index.column()].getFunction;
        if(getFunction)
        {
            return getFunction(address);
        }
        //QString calculus = columns_[index.column()].calculus;
        //lua_pushinteger(luaState_,address);
        //lua_setglobal(luaState_,"address");
        //int result = luaL_loadstring(luaState_,calculus.toStdString().c_str());
        //QString displayedString;
        //switch(result)
        //{
        //case 0:
        //    {
        //        result = lua_pcall(luaState_, 0, LUA_MULTRET, 0);
        //        switch(result)
        //        {
        //        case 0:
        //            {
        //                //脚本执行成功，获得返回值。
        //                displayedString = QString(luaL_checkstring(luaState_,1));
        //                lua_settop(luaState_,0);
        //                break;
        //            }
        //        case LUA_ERRRUN:
        //            {
        //                UERROR<<"脚本运行时错误："<<luaL_checkstring(luaState_,1);
        //                lua_pop(luaState_,1);
        //                break;
        //            }
        //        case LUA_ERRMEM:
        //            {
        //                UERROR<<"执行时分配内存失败："<<luaL_checkstring(luaState_,1);
        //                lua_pop(luaState_,1);
        //                break;
        //            }
        //        case LUA_ERRERR:
        //            {
        //                UERROR<<"执行错误处理函数时失败："<<luaL_checkstring(luaState_,1);
        //                lua_pop(luaState_,1);
        //                break;
        //            }
        //        }
        //        break;
        //    }
        //case LUA_ERRSYNTAX:
        //    {
        //        UERROR<<"语法错误。"<<luaL_checkstring(luaState_,1);
        //        lua_pop(luaState_,1);
        //        break;
        //    }
        //case LUA_ERRMEM:
        //    {
        //        UERROR<<"内存分配错误。"<<luaL_checkstring(luaState_,1);
        //        lua_pop(luaState_,1);
        //        break;
        //    }
        //default:
        //    {
        //        UERROR<<"未知错误："<<result;
        //    }
        //}
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

void UMemoryModel::addColumnInfo( const QString &title,GetFunction getFunction,
                                 SetFunction setFunction)
{
    beginInsertColumns(QModelIndex(),columnCount(),columnCount());
    ColumnInfo columnInfo(title,getFunction,setFunction);
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
            return QString("+%1").arg(section*4,0,16);
        }
    }
    return QVariant();
}


}//namespace uni