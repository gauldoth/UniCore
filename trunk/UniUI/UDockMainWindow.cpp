#include "UDockMainWindow.h"

#include <QKeyEvent>

#include "ULog.h"

namespace uni
{


UDockMainWindow::UDockMainWindow(QWidget *parent /*= 0*/,Qt::WindowFlags flags /*= 0*/)
:QMainWindow(parent,flags)
{

}

UDockMainWindow::~UDockMainWindow()
{

}

void UDockMainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        hide();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

void UDockMainWindow::moveEvent(QMoveEvent *event)
{
    UTRACE<<delim<<event->pos().x()<<event->pos().y();
    event->accept();
}

void UDockMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        UTRACE<<delim<<event->pos().x()<<event->pos().y();
    }
    event->ignore();
}

}//namespace uni