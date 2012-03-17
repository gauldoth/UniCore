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
    typedef QString (*GetFunction)(int);
    typedef void (*SetFunction)(int,const QString &);
    struct ColumnInfo
    {
        ColumnInfo(const QString &t,GetFunction g,SetFunction s)
            :title(t),getFunction(g),setFunction(s){}
        QString title;  //!< 列名。
        QString (*getFunction)(int);  //!< 取该列值的函数。
        void (*setFunction)(int,const QString &);  //!< 设置该列值的函数。
    };

    enum {PageSize = 100};  //每次增加的项数。
    enum {RowStep = 1};  //下一行地址为上一行地址加4。
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
    //! 设置要监视的地址。
    void setAddress(int address);
    //! 添加一列。
    void addColumnInfo(const QString &title,GetFunction getFunction,
        SetFunction setFunction);
private:
    int baseAddress_;  //!< 基地址。
    int currentRowCount_;  //!< 当前要查看的行数，这个数据模型会根据视图的需要自动扩展数据的行数。
    lua_State *luaState_;
    QList<ColumnInfo> columnInfos_;
};

}//namespace uni

#endif//UNIUI_UMEMORY_MODEL_H