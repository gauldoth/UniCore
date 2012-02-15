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

//! 封包视图。
/*!
    封包视图用于显示游戏中收发的封包。主要功能如下：
    - 封包监视器（PacketMonitor）显示收到的所有封包，类似DebugView，
    并且能以各种方式显示。
    - 封包信息列表（PacketInfoList）收集收到的所有封包ID和类型，并能设置哪些
    封包可以在封包监视器中显示。
    - 封包信息列表中的设置使用配置档保存。
*/
class UPacketView : public QWidget
{
    Q_OBJECT

public:
    //! 封包类型。
    enum PacketType
    {
        RecvType,  //!< 收到的封包。
        SendType,  //!< 发送的封包。
    };
    //! 封包信息。
    /*!
        保存了各种类封包的详细信息。
    */
    struct PacketInfo
    {
        PacketInfo()
            :id(0),visible(true)
        {
        }
        bool operator ==(const PacketInfo &packetInfo)
        {
            return id == packetInfo.id && type == packetInfo.type;
        }
        
        int id;  //!< 封包ID。
        PacketType type;  //!< 封包类型。
        QString name;  //!< 封包名字。
        bool visible;  //!< 该封包是否可见。
    };
    //! 具体的一条封包数据。
    struct PacketData
    {
        int id;  //!< 封包ID。
        PacketType type;  //!< 封包类型。
        QByteArray content;  //!< 封包内容。
    };
    explicit UPacketView(QWidget *parent = 0);
    virtual ~UPacketView();
public:
    //! 向封包视图中添加封包。
    /*!
        \param type 封包的类型（Recv或者Send）。
        \param packet 封包数据的指针。
        \param packetSize 封包长度。
    */
    void addPacket(PacketType type,const char *packet,int packetSize);
private slots:
    void onSelectAllCheckBoxChanged( int state );
    //! 更新封包的过滤条件。
    void updateFilters();
private:
    //! 创建封包信息列表处的组合框。
    void createPacketListGroupBox();
    //! 创建封包监视器处的组合框。
    void createPacketMonitorGroupBox();
   
    QList<PacketInfo> packetInfos_;
    QList<PacketData> packetDatas_;

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