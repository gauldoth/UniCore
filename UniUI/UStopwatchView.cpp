#include "UStopwatchView.h"

#include <QList>
#include <QTimer>
#include <QStandardItemModel>

#include "..//UniCore//UDebug.h"
#include "..//UniCore//ULog.h"

namespace uni
{

UStopwatchView::UStopwatchView( QWidget *parent /*= 0*/ )
{
    model_ = new QStandardItemModel(this);
    setModel(model_);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(1000);
}

UStopwatchView::~UStopwatchView()
{

}

void UStopwatchView::update()
{
    for(int i = 0; i < UStopwatch::StopwatchCount; i++)
    {

        if(UStopwatch::milliseconds(i))
        {
            QList<QStandardItem *> items = model_->findItems(QString::number(i));
            __int64 time = UStopwatch::milliseconds(i);
            __int64 hour = time/3600000;
            __int64 minute = (time/60000)%60;
            __int64 second = (time/1000)%60;
            __int64 miliSecond = time%1000;

            QString timeString = QString("%1:%2:%3'%4").arg((int)hour,2,10,QChar('0'))
                .arg((int)minute,2,10,QChar('0')).arg((int)second,2,10,QChar('0'))
                .arg((int)miliSecond,3,10,QChar('0'));
            if(items.empty())
            {
                QList<QStandardItem *> newItems;
                newItems.push_back(new QStandardItem(QString::number(i)));
                newItems.push_back(new QStandardItem(timeString));
                model_->insertRow(0,newItems);
                model_->sort(0);
            }
            else
            {
                model_->setItem(items[0]->index().row(),1,new QStandardItem(timeString));
            }
        }
    }
}

}//namespace uni