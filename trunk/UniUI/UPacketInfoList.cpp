#include "UPacketInfoList.h"

#include <QAction>
#include <QHeaderView>
#include <QSizePolicy>

#include "../UniCore/ULog.h"

namespace uni
{

UPacketInfoList::UPacketInfoList( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);
    QSizePolicy sizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    setSizePolicy(sizePolicy);
    showSelectedPacketInfos_ = new QAction(tr("Show Selected"),this);
    hideSelectedPacketInfos_ = new QAction(tr("Hide Selected"),this);

    addAction(showSelectedPacketInfos_);
    addAction(hideSelectedPacketInfos_);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    

    connect(showSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(showSelectedPacketInfos()));
    connect(hideSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(hideSelectedPacketInfos()));
}

UPacketInfoList::~UPacketInfoList()
{

}

void UPacketInfoList::showSelectedPacketInfos()
{
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        for(int i = range.top(); i <= range.bottom(); i++)
        {
            model()->setData(model()->index(i,0),Qt::Checked,Qt::CheckStateRole);
        }
    }
}

void UPacketInfoList::hideSelectedPacketInfos()
{
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        for(int i = range.top(); i <= range.bottom(); i++)
        {
            model()->setData(model()->index(i,0),Qt::Unchecked,Qt::CheckStateRole);
        }
    }
}

}//namespace uni