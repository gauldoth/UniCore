#include "UPacketMonitor.h"

#include <QHeaderView>

namespace uni
{

UPacketMonitor::UPacketMonitor( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    setAlternatingRowColors(true);
    setVerticalScrollMode(ScrollPerPixel);
    setSortingEnabled(true);
}

UPacketMonitor::~UPacketMonitor()
{
    
}

}//namespace uni