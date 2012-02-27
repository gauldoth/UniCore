#ifndef UPACKETVIEW_DEV_H
#define UPACKETVIEW_DEV_H

#include <QtGui/QMainWindow>
#include "ui_UPacketView_Dev.h"

namespace uni
{
class UPacketView;
}//namespace uni

class UPacketView_Dev : public QMainWindow
{
    Q_OBJECT

public:
    UPacketView_Dev(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~UPacketView_Dev();
private slots:
    void addPacketTest();
private:
    Ui::UPacketView_DevClass ui;
    uni::UPacketView *packetView_;
};

#endif // UPACKETVIEW_DEV_H
