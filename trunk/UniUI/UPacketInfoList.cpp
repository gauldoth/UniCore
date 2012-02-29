#include "UPacketInfoList.h"

#include <QHeaderView>

namespace uni
{

UPacketInfoList::UPacketInfoList( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

UPacketInfoList::~UPacketInfoList()
{

}

}//namespace uni