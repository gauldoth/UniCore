#ifndef UNIUI_UPROCESSMODEL_H
#define UNIUI_UPROCESSMODEL_H

#include <QAbstractTableModel>

namespace uni
{

class ProcessInfo;

class UProcessModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum {DefaultInterval = 2000};
    enum {MaxProcessCount = 1024};
    enum {FieldCount = 2};
    enum 
    {
        NameColumn = 0,
        PIDColumn = 1,
    };
    explicit UProcessModel(QObject *parent = 0);
    virtual ~UProcessModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private slots:
    void refresh();

private:
    Q_DISABLE_COPY(UProcessModel);
    int interval_;
    QList<ProcessInfo> processInfos_;
};

}//namespace uni

#endif//UNIUI_UPROCESSMODEL_H