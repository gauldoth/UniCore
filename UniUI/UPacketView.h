/*! \file UPacketView.h
    \brief 封包视图。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_VIEW_H
#define UNIUI_UPACKET_VIEW_H

#include <QMap>
#include <QWidget>
#include <QTime>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"


class QCheckBox;
class QGroupBox;
class QLabel;
class QModelIndex;
class QPushButton;

namespace uni
{

class UPacketInfoList;
class UPacketInfoListModel;
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
    - 封包过滤方案列表。
    - 封包信息列表中的设置使用配置档保存。

    \todo 记录封包时间。（完成）
    \todo 根据字体计算单元格大小。
    \todo 增加显示方案，可以创建，保存，读取显示方案。（重要）
    \todo 增加Send和Recv过滤，去除全选。（完成）
    \todo 增加静默模式，开启后暂停接受新的封包。（完成）
    \todo 可以在封包监视器中设置标记（类似书签）。
    \todo 封包信息列表可编辑，并增加删除封包信息功能。（完成）
    \todo 设置指定类型的封包颜色。（设想）
    \todo 增加选项，仅显示选中封包。
    \todo 考虑封包信息变化时（更新时），相关的处理。
    \todo 抓到的封包数据可以保存，复制，用于后续查看。
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
        QTime time;  //!< 封包时间。
    };
    //! 显示方案。
    struct DisplayScheme
    {
        
        bool showOnlySelectedPackets;
        bool showRecvPackets;
        bool showSendPackets;
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
    
    //! 设置静默模式。
    /*!
        \param silentMode 是否为静默模式。
        静默模式下将直接丢弃新添加的封包。
    */
    void setSilentMode(bool enable);

    //! 设置仅显示选中的封包。
    /*!
        \param enable 是否仅显示选中封包。
    */
    void setShowOnlySelectedPackets(bool enable);

    //! 设置仅显示Send封包。
    /*!
        \param enable 是否仅显示Send封包。
    */
    void setShowSendPackets(bool enable);

    //! 设置仅显示Recv封包。
    /*!
        \param enable 是否仅显示Recv封包。
    */
    void setShowRecvPackets(bool enable);
private slots:
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
    void saveSettings();
    //! 读取封包信息。
    /*!
        从当前进程所在的配置档中读取封包信息。
    */
    void loadSettings();
   
    //! 封包信息。保存了所有种类的封包信息。
    QList<PacketInfo> packetInfos_;
    QList<PacketData> packetDatas_;

    QGroupBox *packetListGroupBox_;
    uni::UPacketInfoList *packetList_;
    uni::UPacketInfoListModel *packetListModel_;
    QPushButton *clearPacketInfosButton_;
    QPushButton *silentModePushButton_;
    QPushButton *showSendPacketsButton_;
    QPushButton *showRecvPacketsButton_;
    QPushButton *showOnlySelectedButton_;  //!< 仅显示选中按钮。

    QGroupBox *packetMonitorGroupBox_;
    QPushButton *autoScrollPushButton_;
    QLabel *packetCountLabel_;
    uni::UPacketMonitor *packetMonitor_;
    UPacketMonitorProxyModel *packetMonitorProxyModel_;
    uni::UPacketMonitorModel *packetMonitorModel_;

    bool silentMode_;  //!< 是否处在静默模式。
    bool showOnlySelectedPackets_;  //!< 只显示选中的封包。
    bool showSendPackets_;  //!< 显示发送的封包。
    bool showRecvPackets_;  //!< 显示接收的封包。
};

QDataStream &operator<<(QDataStream &, const UPacketView::PacketInfo &);
QDataStream &operator>>(QDataStream &, UPacketView::PacketInfo &);

inline bool operator ==(const UPacketView::PacketInfo &a,const UPacketView::PacketInfo &b)
{
    return a.id == b.id && a.type == b.type;
}

inline bool operator <(const UPacketView::PacketInfo &a,const UPacketView::PacketInfo &b)
{
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