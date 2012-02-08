/*! \file UPacketView.h
    \brief 封包视图。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_VIEW_H
#define UNIUI_UPACKET_VIEW_H

#include <QMap>
#include <QWidget>

namespace uni
{

class UPacketList;
class UPacketListModel;

class PacketInfo
{
    int id;
    QString name;
    bool filter;
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
    QMap<int,PacketInfo> packetInfos_;
};

}//namespace uni

#endif//UNIUI_UPACKET_VIEW_H