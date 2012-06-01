/*! \file UPacketContentView.h
    \brief 用于显示封包内容的控件。

    \author uni
    \date 2012-6-12
*/
#ifndef UNIUI_UPACKET_CONTENT_VIEW_H
#define UNIUI_UPACKET_CONTENT_VIEW_H

#include <QDialog>
#include "UPacketView.h"

class QTextEdit;

namespace uni
{

//! 用于显示单条封包内容的控件。
class UPacketContentView : public QDialog
{
    Q_OBJECT

public:
    explicit UPacketContentView(QWidget *parent = 0);
    ~UPacketContentView();
    //! 设置要显示的封包。
    void setPacket(UPacketView::PacketData *packetData);
private:
    QTextEdit *packetContentEdit_;  //用于显示封包内容的编辑框。
    QTextEdit *packetTextEdit_;  //用于显示封包对应文本的编辑框。
};


}//namespace uni

#endif//UNIUI_UPACKET_CONTENT_VIEW_H