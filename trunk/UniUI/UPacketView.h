/*! \file UPacketView.h
    \brief 封包视图。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_VIEW_H
#define UNIUI_UPACKET_VIEW_H

#include <QMap>
#include <QWidget>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"


class QCheckBox;
class QGroupBox;

namespace uni
{

class UPacketList;
class UPacketListModel;
class UPacketMonitor;
class UPacketMonitorModel;

struct PacketInfo
{
    PacketInfo()
        :id(0),visible(true)
    {
    }
    int id;
    QString name;
    bool visible;  //!< 该封包是否可见。
};

class UPacketView : public QWidget
{
    Q_OBJECT

public:
    explicit UPacketView(QWidget *parent = 0);
    virtual ~UPacketView();
private:
    void setupUI();
    void createPacketListGroupBox();
    void createPacketMonitorGroupBox();
    QMap<int,PacketInfo> packetInfos_;
    QList<QByteArray> packets_;

    QGroupBox *packetListGroupBox_;
    uni::UPacketList *packetList_;
    uni::UPacketListModel *packetListModel_;
    QCheckBox *selectAllCheckBox_;

    QGroupBox *packetMonitorGroupBox_;
    uni::UPacketMonitor *packetMonitor_;
    uni::UPacketMonitorModel *packetMonitorModel_;
};

}//namespace uni

#endif//UNIUI_UPACKET_VIEW_H