/*! \file UPacketMonitorModel.h
    \brief UPacketMonitor使用的数据模型。
    
    \author uni
    \date 2012-2-9
*/
#ifndef UNIUI_UPACKET_MONITOR_MODEL_H
#define UNIUI_UPACKET_MONITOR_MODEL_H

#include <QAbstractTableModel>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UPacketMonitorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UPacketMonitorModel(QObject *parent = 0);
    virtual ~UPacketMonitorModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_MODEL_H