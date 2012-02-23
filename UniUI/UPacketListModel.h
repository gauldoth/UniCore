/*! \file UPacketListModel.h
    \brief 提供封包ID及其对应信息的数据模型。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_LIST_MODEL_H
#define UNIUI_UPACKET_LIST_MODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "UPacketView.h"

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

//! UPacketList使用的Model。
class UPacketListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UPacketListModel(QList<UPacketView::PacketInfo> *packetInfos,QObject *parent = 0);
    virtual ~UPacketListModel();
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    void addPacketInfo(UPacketView::PacketInfo packetInfo);
public slots:
    //! 全选。
    /*!
        所有封包类型都允许输出。
    */
    void selectAll();
    //! 全部取消选择。
    /*!
        所有封包类型都不允许输出。
    */
    void deselectAll();
signals:
    void visibilityChanged();
private:
    QList<UPacketView::PacketInfo> *packetInfos_;
};

}//namespace uni

#endif//UNIUI_UPACKET_LIST_MODEL_H