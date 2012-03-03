#include "UPacketMonitor.h"

#include <QHeaderView>
#include <QAction>

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

    connect(clearPacketsAction_,SIGNAL(triggered()),this,SLOT(clearPackets()));
}

UPacketMonitor::~UPacketMonitor()
{
    
}

void UPacketMonitor::clearPackets()
{
    model()->removeRows(0,model()->rowCount());
}

}//namespace uni