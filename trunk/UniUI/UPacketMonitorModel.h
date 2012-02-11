/*! \file UPacketMonitorModel.h
    \brief UPacketMonitor使用的数据模型。
    
    \author uni
    \date 2012-2-9
*/
#ifndef UNIUI_UPACKET_MONITOR_MODEL_H
#define UNIUI_UPACKET_MONITOR_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

#include "UPacketView.h"

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UPacketMonitorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UPacketMonitorModel(QList<PacketInfo> *packetInfos,QList<Packet> *packets,QObject *parent = 0);
    virtual ~UPacketMonitorModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    void addPacket( PacketType type,const char * packet, int packetSize);
private:
    QList<PacketInfo> *packetInfos_;
    QList<Packet> *packets_;
};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_MODEL_H