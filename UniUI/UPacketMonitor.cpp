#include "UPacketMonitor.h"

namespace uni
{

UPacketMonitor::UPacketMonitor( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    setAlternatingRowColors(true);
}

UPacketMonitor::~UPacketMonitor()
{

}

}//namespace uni