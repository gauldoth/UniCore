/*! \file UMemoryModel.h
    \brief 表示内存数据。

    \author uni
    \date 2012-1-18
*/
#ifndef UNIUI_UMEMORY_MODEL_H
#define UNIUI_UMEMORY_MODEL_H

#include <QAbstractItemModel>
#include <QList>

namespace uni
{

class UMemoryModel : public QAbstractTableModel
{
    Q_OBJECT
        
public:
    class HeaderData
    {

    };
    enum {PageSize = 100};
    explicit UMemoryModel(QObject *parent = 0);
    virtual ~UMemoryModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    int baseAddress_;
    //数组，记录每一列的标题和计算方法。
    QList<HeaderData> columns_;
};

}//namespace uni

#endif//UNIUI_UMEMORY_MODEL_H