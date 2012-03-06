#include "UPacketView.h"

#include <QCheckBox>
#include <QCoreApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QTime>

#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"

#include "UPacketInfoList.h"
#include "UPacketInfoListModel.h"
#include "UPacketMonitor.h"
#include "UPacketMonitorModel.h"
#include "UPacketMonitorProxyModel.h"

namespace uni
{

UPacketView::UPacketView( QWidget *parent /*= 0*/ )
:QWidget(parent)
{
    loadPacketInfos();
    createPacketListGroupBox();
    createPacketMonitorGroupBox();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(packetListGroupBox_);
    layout->addWidget(packetMonitorGroupBox_);
    setLayout(layout);

    updateFilters();
}

UPacketView::~UPacketView()
{
    UTRACE<<"enter";
    savePacketInfos();
}

void UPacketView::createPacketListGroupBox()
{
    packetListGroupBox_ = new QGroupBox(tr("Packet List"),this);
    packetList_ = new UPacketInfoList(this);
    packetListModel_ = new UPacketInfoListModel(&packetInfos_,this);
    packetList_->setModel(packetListModel_);

    clearPacketInfosButton_ = new QPushButton(tr("Clear"),this);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packetList_);
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(clearPacketInfosButton_);

    mainLayout->addLayout(bottomLayout);
    packetListGroupBox_->setLayout(mainLayout);

    connect(packetListModel_,SIGNAL(visibilityChanged()),this,SLOT(updateFilters()));
    connect(clearPacketInfosButton_,SIGNAL(clicked()),this,SLOT(clearPacketInfos()));
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
    //packetMonitor_->setModel(packetMonitorModel_);
    autoScrollCheckBox_ = new QCheckBox(tr("Auto Scroll"),this);
    packetCountLabel_ = new QLabel(tr("Packet Count:0"),this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(packetCountLabel_);
    layout->addWidget(packetMonitor_);
    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(autoScrollCheckBox_);
    layout->addLayout(layout2);
    packetMonitorGroupBox_->setLayout(layout);

    connect(autoScrollCheckBox_,SIGNAL(toggled(bool)),this,SLOT(setAutoScroll(bool)));
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
    //UTRACE("临时")<<"1";
    if(!packetInfos_.contains(packetInfo))
    {
        packetListModel_->addPacketInfo(packetInfo);
        updateFilters();
    }
    //UTRACE("临时")<<"2";
    //添加封包。
    PacketData packetData;
    packetData.id = packetID;
    packetData.type = type;
    packetData.time = QTime::currentTime();
    packetData.content = QByteArray(packet,packetSize);
    packetMonitorModel_->addPacketData(packetData);
    packetCountLabel_->setText(tr("Packet Count:%1").arg(packetMonitorModel_->rowCount()));
    //UTRACE("临时")<<"3";
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


void UPacketView::savePacketInfos()
{
    UTRACE("UI")<<"enter";
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UPacketView.cfg");
    configFile.open(QIODevice::WriteOnly);
    QDataStream out(&configFile);
    out.setVersion(QDataStream::Qt_4_5);
    out<<packetInfos_;
}

void UPacketView::loadPacketInfos()
{
    UTRACE("UI")<<"enter";
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UPacketView.cfg");
    if(!configFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QDataStream in(&configFile);
    in.setVersion(QDataStream::Qt_4_5);
    in>>packetInfos_;
}

void UPacketView::clearPacketInfos()
{
    UTRACE<<"enter";
    packetListModel_->removeRows(0,packetListModel_->rowCount());
}

void UPacketView::setAutoScroll( bool isAutoScroll )
{
    //packetMonitor_->scrollToBottom();
    if(isAutoScroll)
    {
        connect(packetMonitorModel_,SIGNAL(rowsInserted(const QModelIndex &,int,int)),packetMonitor_,SLOT(scrollToBottom()));
    }
    else
    {
        disconnect(packetMonitorModel_,SIGNAL(rowsInserted(const QModelIndex &,int,int)),packetMonitor_,SLOT(scrollToBottom()));
    }
}

void UPacketView::setSilentMode( bool silentMode )
{
    
}

QDataStream & operator<<( QDataStream &s, const UPacketView::PacketInfo &packetInfo )
{
    s<<packetInfo.id;
    s<<packetInfo.name;
    s<<static_cast<int>(packetInfo.type);
    s<<packetInfo.visible;
    return s;
}

QDataStream & operator>>( QDataStream &s, UPacketView::PacketInfo &packetInfo )
{
    s>>packetInfo.id;
    s>>packetInfo.name;
    int type = 0;
    s>>type;
    packetInfo.type = static_cast<UPacketView::PacketType>(type);
    s>>packetInfo.visible;
    return s;
}


}//namespace uni
