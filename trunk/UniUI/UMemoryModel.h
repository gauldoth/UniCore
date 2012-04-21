/*! \file UMemoryModel.h
    \brief 表示内存数据。

    \author uni
    \date 2012-1-18
*/
#ifndef UNIUI_UMEMORY_MODEL_H
#define UNIUI_UMEMORY_MODEL_H

#include <QAbstractItemModel>
#include <QList>

#include "UMemoryView.h"

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
    //! 取数据的函数。
    /*!
        参数1为地址。
    */
    typedef QString (*GetDataFunction)(int);
    //! 保存数据的函数。
    /*!
        参数1为地址，参数2为要保存的数据。
    */
    typedef void (*SetDataFunction)(int,const QString &);
    //! 获得数据颜色的函数。
    /*!
        参数1为地址。
    */
    typedef QColor (*DataColorFunction)(int);
    struct ColumnInfo
    {
        ColumnInfo()
            :getDataFunction(0),setDataFunction(0),dataColorFunction(0)
        {
        }
        QString title;  //!< 列名。
        GetDataFunction getDataFunction;  //!< 取该列数据的函数。
        SetDataFunction setDataFunction;  //!< 保存该列数据的函数。
        DataColorFunction dataColorFunction;  //!< 获得该列单元格的颜色的函数。
        QString getDataScript;
        QString setDataScript;
        QString dataColorScript;
    };
    enum {PageSize = 100};  //每次增加的项数。
    enum {RowStep = 1};  //下一行地址为上一行地址加4。
    explicit UMemoryModel(QObject *parent = 0);
    virtual ~UMemoryModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual void fetchMore(const QModelIndex &parent);
    virtual bool canFetchMore(const QModelIndex &parent) const;
    virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    void initCommonColumn();  //!< 初始化非自定义的列。

    //! 运行luaC函数。
    void runLuaCFunction(lua_CFunction function);

    //! 获得指定列的列信息。
    ColumnInfo columnInfo(int column) {return columnInfos_[column];}

    friend QDataStream & operator<< (QDataStream& stream, const UMemoryModel::ColumnInfo& columnInfo);
    friend QDataStream & operator>> (QDataStream& stream, UMemoryModel::ColumnInfo& columnInfo);

    friend QDataStream & operator<< (QDataStream& stream, const UMemoryModel& model);
    friend QDataStream & operator>> (QDataStream& stream, UMemoryModel& model);

public slots:
    //! 设置要监视的地址。
    void setAddress(int address);
    //! 添加一列。
    void addColumnInfo(ColumnInfo columnInfo);
    //! 改变指定列的列信息。
    void setColumnInfo(int column,ColumnInfo columnInfo);
    //! 更新数据。
    void update();
signals:
    //! 列信息改变。
    void columnInfoChanged();
private:
    //! 编译列信息中脚本。
    void compileScripts();
    int baseAddress_;  //!< 基地址。
    int currentRowCount_;  //!< 当前要查看的行数，这个数据模型会根据视图的需要自动扩展数据的行数。
    QList<ColumnInfo> columnInfos_;
    lua_State *L_;
};



}//namespace uni

#endif//UNIUI_UMEMORY_MODEL_H