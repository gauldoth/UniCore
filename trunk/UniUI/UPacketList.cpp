#include "UPacketList.h"

#include <QHeaderView>

namespace uni
{

UPacketList::UPacketList( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

UPacketList::~UPacketList()
{

}

}//namespace uni