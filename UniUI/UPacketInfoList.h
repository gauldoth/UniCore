/*! \file UPacketInfoList.h
    \brief 封包过滤列表。
    用于显示和配置哪些封包要过滤。

    \author uni
    \date 2012-2-8
*/
#ifndef UNIUI_UPACKET_FILTER_LIST_H
#define UNIUI_UPACKET_FILTER_LIST_H

#include <QTableView>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

class QAction;

namespace uni
{

//! 封包列表。
/*!
    列出所有封包ID及相关信息，可以选择哪些封包允许输出，哪些不能输出。\r
    可以自定义封包名字。
    
    \todo 考虑自动生成封包ID枚举类型。
*/
class UPacketInfoList : public QTableView
{
    Q_OBJECT

public:
    explicit UPacketInfoList(QWidget *parent = 0);
    virtual ~UPacketInfoList();
private slots:
    //! 显示选择的封包信息。
    void showSelectedPacketInfos();
    //! 隐藏选择的封包信息。
    void hideSelectedPacketInfos();
private:
    QAction *showSelectedPacketInfos_;  //!< 显示选择的封包信息。
    QAction *hideSelectedPacketInfos_;  //!< 隐藏选择的封包信息。
};

}//namespace uni

#endif//UNIUI_UPACKET_FILTER_LIST_H