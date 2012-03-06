#include "UPacketMonitor.h"

#include <QHeaderView>
#include <QAction>
#include <QSignalMapper>

namespace uni
{

UPacketMonitor::UPacketMonitor( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    setAlternatingRowColors(true);
    setVerticalScrollMode(ScrollPerPixel);
    //setSortingEnabled(true);
    horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    clearPacketsAction_ = new QAction(tr("Clear Packets"),this);
    addAction(clearPacketsAction_);

    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    horizontalHeader()->setMovable(true);

    connect(clearPacketsAction_,SIGNAL(triggered()),this,SLOT(clearPackets()));
}

UPacketMonitor::~UPacketMonitor()
{
    
}

void UPacketMonitor::clearPackets()
{
    model()->removeRows(0,model()->rowCount());
}

void UPacketMonitor::setModel( QAbstractItemModel *model )
{
    QTableView::setModel(model);
    for(int i = 0; i < model->columnCount(); i++)
    {
        QAction *action = new QAction(model->headerData(i,Qt::Horizontal).toString(),this);
        action->setCheckable(true);
        action->setData(i);
        action->setChecked(true);
        horizontalHeader()->addAction(action);
        connect(action,SIGNAL(toggled(bool)),this,SLOT(onShowHideColumnToggled(bool)));
    }
}

void UPacketMonitor::onShowHideColumnToggled(bool checked)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action)
    {
        if(checked)
        {
            showColumn(action->data().toInt());
        }
        else
        {
            hideColumn(action->data().toInt());
        }
    }
}

}//namespace uni