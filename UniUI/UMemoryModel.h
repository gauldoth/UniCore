/*! \file UMemoryModel.h
    \brief 表示内存数据。

    \author uni
    \date 2012-1-18
*/
#ifndef UNIUI_UMEMORY_MODEL_H
#define UNIUI_UMEMORY_MODEL_H

#include <QAbstractItemModel>
#include <QList>

extern "C"
{
#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
}
namespace uni
{

class UMemoryModel : public QAbstractTableModel
{
    Q_OBJECT
        
public:
    struct HeaderData
    {
        QString headerTitle;
        QString calculus;  //该列的计算方法，Lua脚本。
    };
    enum {PageSize = 100};  //每次增加的项数。
    enum {RowStep = 4};  //下一行地址为上一行地址加4。
    explicit UMemoryModel(QObject *parent = 0,lua_State *state = 0);
    virtual ~UMemoryModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual void fetchMore(const QModelIndex &parent);
    virtual bool canFetchMore(const QModelIndex &parent) const;

public slots:
    //! 设置要监视进程的ID。
    void setTargetProcessID(int processID) {targetProcessID_ = processID;}
    //! 设置要监视的地址。
    void setAddress(int address);
    //! 添加一列。
    void addColumn(const QString &title,const QString &calculus);
private:
    int targetProcessID_;  //!< 要获得内存信息的目标进程ID，假如为0则为当前进程。
    int baseAddress_;  //!< 基地址。
    //数组，按顺序记录每一列的标题和计算方法。
    QList<HeaderData> columns_;
    int currentRowCount_;  //!< 当前要查看的行数，这个数据模型会根据视图的需要自动扩展数据的行数。
    lua_State *luaState_;
};

}//namespace uni

#endif//UNIUI_UMEMORY_MODEL_H