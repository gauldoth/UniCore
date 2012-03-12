﻿/*! \file UPacketMonitor.h
    \brief 用于显示收到的封包内容。

    \author uni
    \date 2012-2-9
*/
#ifndef UNIUI_UPACKET_MONITOR_H
#define UNIUI_UPACKET_MONITOR_H

#include <QTableView>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

class QAction;

namespace uni
{

class UPacketMonitor : public QTableView
{
    Q_OBJECT

public:
    explicit UPacketMonitor(QWidget *parent = 0);
    virtual ~UPacketMonitor();
    virtual void setModel(QAbstractItemModel *model);
public slots:
    //! 清除所有封包。
    void clearPackets();
    //! 显示隐藏列的响应函数。
    void onShowHideColumnToggled(bool checked);
    //! 拷贝选中的项的内容。
    void copySelected();
private:
    QAction *clearPacketsAction_;  //!< 清除所有封包。
    QAction *copyAction_;  //!< 复制当前项的内容到剪贴板。
};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_H