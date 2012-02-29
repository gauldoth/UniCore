#include "UPacketMonitor.h"

#include <QHeaderView>

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
}

UPacketMonitor::~UPacketMonitor()
{
    
}

}//namespace uni