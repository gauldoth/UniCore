/*! \file UPacketMonitorModel.h
    \brief UPacketMonitor使用的数据模型。
    
    \author uni
    \date 2012-2-9
*/
#ifndef UNIUI_UPACKET_MONITOR_MODEL_H
#define UNIUI_UPACKET_MONITOR_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

#include "UPacketView.h"

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UPacketMonitorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    //! 构造函数。
    /*!
        \param packetDatas 所有封包数据的指针。
        \param parent 父对象。
    */
    explicit UPacketMonitorModel(QList<UPacketView::PacketData> *packetDatas,QObject *parent = 0);
    virtual ~UPacketMonitorModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void addPacketData(UPacketView::PacketData data);

private:
    QList<UPacketView::PacketData> *packetDatas_;
    QFont *font_;  //!< 所有数据使用的字体。
    QStringList columnNames_;  //!< 列信息。
};

}//namespace uni

#endif//UNIUI_UPACKET_MONITOR_MODEL_H