#include "UPacketMonitor.h"

#include <QHeaderView>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSignalMapper>

#include "../UniCore/UDebug.h"
#include "../UniCore/ULog.h"
#include "UPacketView.h"
#include "UPacketContentView.h"

namespace uni
{

UPacketMonitor::UPacketMonitor( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    setAlternatingRowColors(true);
    setVerticalScrollMode(ScrollPerPixel);
    //setWordWrap(false);
    //setSortingEnabled(true);
    QFontMetrics fontMetrics(font());
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(fontMetrics.height()+3);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    copyAction_ = new QAction(tr("Copy"),this);
    addAction(copyAction_);

    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    horizontalHeader()->setSectionsMovable(true);

    connect(copyAction_,SIGNAL(triggered()),this,SLOT(copySelected()));
    connect(this,SIGNAL(activated(const QModelIndex &)),this,SLOT(showPacket(const QModelIndex &)));
}

UPacketMonitor::~UPacketMonitor()
{
    
}

void UPacketMonitor::setModel( QAbstractItemModel *model )
{
    QTableView::setModel(model);
    for(int i = 0; i < model->columnCount(); i++)
    {
        QAction *action = new QAction(model->headerData(i,Qt::Horizontal).toString(),this);
        action->setCheckable(true);
        action->setData(i);
        action->setChecked(true);
        horizontalHeader()->addAction(action);
        connect(action,SIGNAL(toggled(bool)),this,SLOT(onShowHideColumnToggled(bool)));

    }
    //重设列宽。
    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void UPacketMonitor::onShowHideColumnToggled(bool checked)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action)
    {
        if(checked)
        {
            showColumn(action->data().toInt());
        }
        else
        {
            hideColumn(action->data().toInt());
        }
    }
}

void UPacketMonitor::copySelected()
{
    QApplication::clipboard()->setText(currentIndex().data(Qt::ToolTipRole).toString());
}

void UPacketMonitor::updateGeometries()
{
    QTableView::updateGeometries();
}

int UPacketMonitor::sizeHintForRow( int row ) const
{
    return QTableView::sizeHintForRow(row);
}

void UPacketMonitor::paintEvent( QPaintEvent * event )
{
    QTableView::paintEvent(event);
}

void UPacketMonitor::showPacket( const QModelIndex &index )
{
    UPacketView::PacketData *packetData = 
        (UPacketView::PacketData *)(index.data(Qt::UserRole).value<void *>());
    UPacketContentView *packetContentView = new UPacketContentView(this);
    packetContentView->setAttribute(Qt::WA_DeleteOnClose,true);
    packetContentView->setPacket(packetData);
    packetContentView->show();
    packetContentView->raise();
    packetContentView->activateWindow();
}

}//namespace uni