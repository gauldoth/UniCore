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
class QPushButton;
class QModelIndex;

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

    \todo 记录封包时间。
    \todo 调整封包监视器中数据大小。
*/
class UPacketView : public QWidget
{
    Q_OBJECT

public:
    //! 封包类型。
    enum PacketType
    {
        RecvType = 0,  //!< 收到的封包。
        SendType = 1,  //!< 发送的封包。
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
public slots:
    //! 向封包视图中添加封包。
    /*!
        \param type 封包的类型（Recv或者Send）。
        \param packet 封包数据的指针。
        \param packetSize 封包长度。
    */
    void addPacket(PacketType type,const char *packet,int packetSize);

    //! 清除封包信息列表中内容。
    /*!
        
    */
    void clearPacketInfos();

    //! 设置自动滚动。
    /*!
        \param isAutoScroll 是否自动滚动。
        假如启用自动滚动，封包监视器每收到一条封包就会滚动到最底部。
    */
    void setAutoScroll(bool isAutoScroll);
protected:
    virtual void hideEvent(QHideEvent *);
private slots:
    void onSelectAllCheckBoxChanged( int state );
    //! 更新封包的过滤条件。
    void updateFilters();
private:
    //! 创建封包信息列表处的组合框。
    void createPacketListGroupBox();
    //! 创建封包监视器处的组合框。
    void createPacketMonitorGroupBox();
    //! 保存封包信息。
    /*!
        将封包信息保存到配置档，配置档放在当前进程所在目录。
    */
    void savePacketInfos();
    //! 读取封包信息。
    /*!
        从当前进程所在的配置档中读取封包信息。
    */
    void loadPacketInfos();
   
    //! 封包信息。保存了所有种类的封包信息。
    QList<PacketInfo> packetInfos_;
    QList<PacketData> packetDatas_;

    QGroupBox *packetListGroupBox_;
    uni::UPacketList *packetList_;
    uni::UPacketListModel *packetListModel_;
    QCheckBox *selectAllCheckBox_;
    QPushButton *clearPacketInfosButton_;

    QGroupBox *packetMonitorGroupBox_;
    QCheckBox *autoScrollCheckBox_;
    uni::UPacketMonitor *packetMonitor_;
    UPacketMonitorProxyModel *packetMonitorProxyModel_;
    uni::UPacketMonitorModel *packetMonitorModel_;
};

QDataStream &operator<<(QDataStream &, const UPacketView::PacketInfo &);
QDataStream &operator>>(QDataStream &, UPacketView::PacketInfo &);

inline bool operator ==(const UPacketView::PacketInfo &a,const UPacketView::PacketInfo &b)
{
    return a.id == b.id && a.type == b.type;
}

inline bool operator <(const UPacketView::PacketInfo &a,const UPacketView::PacketInfo &b)
{
    bool lessThan = false;
    if(a.type < b.type)
    {
        return true;
    }
    else if(a.type > b.type)
    {
        return false;
    }
    else
    {
        return ((a.id - b.id) < 0) ? true : false;
    }
}


}//namespace uni

#endif//UNIUI_UPACKET_VIEW_H