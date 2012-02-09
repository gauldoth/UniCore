/*! \file UPacketMonitor.h
    \brief 用于显示收到的封包内容。

    \author uni
    \date 2012-2-9
*/
#ifndef UNIUI_UPACKET_MONITOR_H
#define UNIUI_UPACKET_MONITOR_H

#include <QTableView>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UPacketMonitor : public QTableView
{
    Q_OBJECT

public:
    explicit UPacketMonitor(QWidget *parent = 0);
    virtual ~UPacketMonitor();
};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_H