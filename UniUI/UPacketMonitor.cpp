#include "UPacketMonitor.h"

#include <QHeaderView>

namespace uni
{

UPacketMonitor::UPacketMonitor( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    setAlternatingRowColors(true);
    setVerticalScrollMode(ScrollPerPixel);
}

UPacketMonitor::~UPacketMonitor()
{
    
}

}//namespace uni