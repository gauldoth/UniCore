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
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
    
    connect(this,SIGNAL(clicked(const QModelIndex &)),this,SLOT(onClicked(const QModelIndex &)));
    connect(showSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(showSelectedPacketInfos()));
    connect(hideSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(hideSelectedPacketInfos()));
    connect(deleteSelectedPacketInfos_,SIGNAL(triggered()),this,SLOT(deleteSelectedPacketInfos()));
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
    emit visibilityChanged();
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
    emit visibilityChanged();
}

void UPacketInfoList::deleteSelectedPacketInfos()
{
    QItemSelection selection = selectionModel()->selection();
    foreach(QItemSelectionRange range,selection)
    {
        model()->removeRows(range.top(),range.height());
    }
}

void UPacketInfoList::onClicked( const QModelIndex &index )
{
    if(index.column() == 0)
    {
        emit visibilityChanged();
    }
}

}//namespace uni