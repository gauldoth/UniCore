#include "UPacketView.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>

#include "UPacketList.h"
#include "UPacketListModel.h"
#include "UPacketMonitor.h"
#include "UPacketMonitorModel.h"

namespace uni
{

UPacketView::UPacketView( QWidget *parent /*= 0*/ )
:QWidget(parent)
{

}

UPacketView::~UPacketView()
{
    
}

void UPacketView::createPacketListGroupBox()
{
    packetListGroupBox_ = new QGroupBox(tr("Packet List"),this);
    packetList_ = new UPacketList(this);
    packetListModel_ = new UPacketListModel(&packetInfos_,this);
    packetList_->setModel(packetListModel_);
    selectAllCheckBox_ = new QCheckBox(tr("Select All"),this);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packetList_);
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(selectAllCheckBox_);

    mainLayout->addWidget(selectAllCheckBox_);
    mainLayout->addLayout(bottomLayout);
    packetMonitorGroupBox_->setLayout(mainLayout);

}

void UPacketView::setupUI()
{
    createPacketListGroupBox();
    createPacketMonitorGroupBox();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(packetListGroupBox_);
    layout->addWidget(packetMonitorGroupBox_);
    setLayout(layout);
}

void UPacketView::createPacketMonitorGroupBox()
{
    packetMonitorGroupBox_ = new QGroupBox(tr("Packet Monitor"),this);
    packetMonitor_ = new UPacketMonitor(this);
    packetMonitorModel_ = new UPacketMonitorModel(this);
}

}//namespace uni
