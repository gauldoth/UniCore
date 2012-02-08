/*! \file UPacketListModel.h
    \brief 提供封包ID及其对应信息的数据模型。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_LIST_MODEL_H
#define UNIUI_UPACKET_LIST_MODEL_H

#include <QAbstractTableModel>

namespace uni
{

//! UPacketList使用的Model。
class UPacketListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UPacketListModel(QWidget *parent = 0);
    virtual ~UPacketListModel();
};

}//namespace uni

#endif//UNIUI_UPACKET_LIST_MODEL_H