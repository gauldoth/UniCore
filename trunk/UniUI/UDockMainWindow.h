#ifndef UNIUI_UDOCKMAINWINDOW_H
#define UNIUI_UDOCKMAINWINDOW_H

#define AUTO_LINK_LIB_NAME "UniUI"
#include "AutoLink.h"

#include <QMainWindow>

namespace uni
{

//! 可停靠窗口。
/*!
    试验品。
*/
class UDockMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UDockMainWindow(QWidget *parent = 0,Qt::WindowFlags flags = 0);
    virtual ~UDockMainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void moveEvent(QMoveEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    Q_DISABLE_COPY(UDockMainWindow);
};

}//namespace uni

#endif//UNIUI_UDOCKMAINWINDOW_H