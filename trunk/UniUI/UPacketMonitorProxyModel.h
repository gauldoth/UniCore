/*! \file UPacketMonitorProxyModel.h
    \brief 添加了过滤封包功能的代理数据模型。

    \author uni
    \date 2012-2-11
*/
#ifndef UNIUI_UPACKET_MONITOR_PROXY_MODEL_H
#define UNIUI_UPACKET_MONITOR_PROXY_MODEL_H

#include <QMap>
#include <QSet>
#include <QSortFilterProxyModel>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

#include "UPacketView.h"

namespace uni
{

class UPacketMonitorProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit UPacketMonitorProxyModel(QObject *parent = 0);
    virtual ~UPacketMonitorProxyModel();
    void setFilters(QMap<UPacketView::PacketType,QSet<int> > filters);
public slots:
protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
private:
    QMap<UPacketView::PacketType,QSet<int> > filters_; 
};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_PROXY_MODEL_H