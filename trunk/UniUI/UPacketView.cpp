﻿#include "UPacketView.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"

#include "UPacketList.h"
#include "UPacketListModel.h"
#include "UPacketMonitor.h"
#include "UPacketMonitorModel.h"
#include "UPacketMonitorProxyModel.h"

namespace uni
{

UPacketView::UPacketView( QWidget *parent /*= 0*/ )
:QWidget(parent)
{
    createPacketListGroupBox();
    createPacketMonitorGroupBox();

    //布局。
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(packetListGroupBox_,1);
    layout->addWidget(packetMonitorGroupBox_,2);
    setLayout(layout);
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
    packetListGroupBox_->setLayout(mainLayout);

    connect(packetListModel_,SIGNAL(visibilityChanged()),this,SLOT(updateFilters()));
    connect(selectAllCheckBox_,SIGNAL(stateChanged(int)),this,SLOT(onSelectAllCheckBoxChanged(int)));
}

void UPacketView::createPacketMonitorGroupBox()
{
    packetMonitorGroupBox_ = new QGroupBox(tr("Packet Monitor"),this);
    packetMonitor_ = new UPacketMonitor(this);
    packetMonitorModel_ = new UPacketMonitorModel(&packetDatas_,this);
    packetMonitorProxyModel_ = new UPacketMonitorProxyModel(this);
    packetMonitorProxyModel_->setDynamicSortFilter(true);
    packetMonitorProxyModel_->setSourceModel(packetMonitorModel_);
    packetMonitor_->setModel(packetMonitorProxyModel_);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(packetMonitor_);
    packetMonitorGroupBox_->setLayout(layout);
}

void UPacketView::addPacket(PacketType type, const char *packet,int packetSize )
{
    //假如是新类型的封包，则在封包列表添加封包。
    //在封包监视器中添加封包。
    int packetID = GetAt<unsigned short>(packet,0);

    PacketInfo packetInfo;
    packetInfo.id = packetID;
    packetInfo.type = type;
    packetInfo.visible = true;
    if(!packetInfos_.contains(packetInfo))
    {
        packetListModel_->addPacketInfo(packetInfo);
        updateFilters();
    }
    //添加封包。
    PacketData packetData;
    packetData.id = packetID;
    packetData.type = type;
    packetData.content = QByteArray(packet,packetSize);
    packetMonitorModel_->addPacketData(packetData);
}

void UPacketView::updateFilters()
{
    QMap<PacketType,QSet<int> > filters;
    for(int i = 0; i < packetInfos_.size(); i++)
    {
        if(packetInfos_[i].visible)
        {
            filters[packetInfos_[i].type].insert(packetInfos_[i].id);
        }
    }
    packetMonitorProxyModel_->setFilters(filters);
}

void UPacketView::onSelectAllCheckBoxChanged( int state )
{
    if(state == Qt::Checked)
    {
        packetListModel_->selectAll();
    }
    else
    {
        packetListModel_->deselectAll();
    }
}

}//namespace uni