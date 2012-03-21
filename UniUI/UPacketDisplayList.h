/*! \file UPacketDisplayList.h
    UPacketDisplayList用于保存封包视图的显示方案。
    允许快速在几个不同的显示方案间切换。

    \author uni
    \date 2012-3-21
*/
#ifndef UNIUI_UPACKET_DISPLAY_LIST_H
#define UNIUI_UPACKET_DISPLAY_LIST_H

#include <QWidget>

#include "UPacketView.h"

namespace uni
{

class UPacketDisplayList : public QWidget
{
    Q_OBJECT

public:
    explicit UPacketDisplayList(QWidget *parent = 0);
    virtual ~UPacketDisplayList();
    //! 设置显示方案。
    /*!
        \param name 显示方案的名称。
        \param scheme 显示方案。
    */
    void setDisplayScheme(const QString &name,UPacketView::DisplayScheme scheme);
    void deleteDisplayScheme(const QString &name);
};

};

#endif//UNIUI_UPACKET_DISPLAY_LIST_H