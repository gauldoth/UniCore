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
class UPacketMonitorProxyModel;

enum PacketType
{
    RecvType,  //!< 接收的封包。
    SendType,  //!< 发送的封包。
};

struct PacketInfo
{
    PacketInfo()
        :id(0),visible(true)
    {
    }
    int id;
    PacketType type;
    QString name;
    bool visible;  //!< 该封包是否可见。
};

struct Packet
{
    int id;
    PacketType type;
    QByteArray content;
};

class UPacketView : public QWidget
{
    Q_OBJECT

public:
    explicit UPacketView(QWidget *parent = 0);
    virtual ~UPacketView();

public slots:
    //! 添加封包。
    void addPacket(PacketType type,const char *packet,int packetSize);
    //! 更新封包的过滤条件。
    void updateFilters();
private slots:
    void onSelectAllCheckBoxChanged( int state );
private:
    void setupUI();
    void createPacketListGroupBox();
    void createPacketMonitorGroupBox();
   
    QList<PacketInfo> packetInfos_;
    QList<Packet> packets_;

    QGroupBox *packetListGroupBox_;
    uni::UPacketList *packetList_;
    uni::UPacketListModel *packetListModel_;
    QCheckBox *selectAllCheckBox_;

    QGroupBox *packetMonitorGroupBox_;
    uni::UPacketMonitor *packetMonitor_;
    UPacketMonitorProxyModel *packetMonitorProxyModel_;
    uni::UPacketMonitorModel *packetMonitorModel_;
};

}//namespace uni

#endif//UNIUI_UPACKET_VIEW_H