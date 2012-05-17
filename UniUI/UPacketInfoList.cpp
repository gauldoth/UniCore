#include "UPacketInfoList.h"

#include <QAction>
#include <QHeaderView>
#include <QSizePolicy>

#include "../UniCore/ULog.h"
#include "UPacketInfoListModel.h"

namespace uni
{

UPacketInfoList::UPacketInfoList( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    verticalHeader()->setResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);
    QSizePolicy sizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    setSizePolicy(sizePolicy);
    showSelectedPacketInfos_ = new QAction(tr("Show Selected"),this);
    hideSelectedPacketInfos_ = new QAction(tr("Hide Selected"),this);
    deleteSelectedPacketInfos_ = new QAction(tr("Delete Selected"),this);

    addAction(showSelectedPacketInfos_);
    addAction(hideSelectedPacketInfos_);
    addAction(deleteSelectedPacketInfos_);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    

    connect(showSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(showSelectedPacketInfos()));
    connect(hideSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(hideSelectedPacketInfos()));
    connect(deleteSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(deleteSelectedPacketInfos()));
}

UPacketInfoList::~UPacketInfoList()
{

}

void UPacketInfoList::showSelectedPacketInfos()
{
    UPacketInfoListModel *model = qobject_cast<UPacketInfoListModel *>(this->model());
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        model->select(range.top(),range.height());
    }
}

void UPacketInfoList::hideSelectedPacketInfos()
{
    UPacketInfoListModel *model = qobject_cast<UPacketInfoListModel *>(this->model());
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        model->deselect(range.top(),range.height());
    }
}

void UPacketInfoList::deleteSelectedPacketInfos()
{
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        model()->removeRows(range.top(),range.height());
    }
}

}//namespace uni