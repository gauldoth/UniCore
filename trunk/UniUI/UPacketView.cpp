#include "UPacketView.h"

#include <QBitmap>
#include <QCheckBox>
#include <QCoreApplication>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QTime>

#include "..//UniCore//UDebug.h"
#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"
#include "UFlowLayout.h"

#include "UPacketDisplayList.h"
#include "UPacketInfoList.h"
#include "UPacketInfoListModel.h"
#include "UPacketMonitor.h"
#include "UPacketMonitorModel.h"
#include "UPacketMonitorProxyModel.h"

namespace uni
{

UPacketView::UPacketView( QWidget *parent /*= 0*/ )
:QWidget(parent)
,silentMode_(false)
,showOnlySelectedPackets_(false)
{
    //读取配置。
    loadSettings();
    //创建控件。
    createPacketListGroupBox();
    createPacketMonitorGroupBox();

    updateFilters();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(packetListGroupBox_,0);
    layout->addWidget(packetMonitorGroupBox_,1);
    setLayout(layout);

}

UPacketView::~UPacketView()
{
    UTRACE<<"enter";
    saveSettings();
}

void UPacketView::createPacketListGroupBox()
{
    packetListGroupBox_ = new QGroupBox(tr("Packet List"),this);
    packetList_ = new UPacketInfoList(this);
    packetList_->setSortingEnabled(true);
    packetListModel_ = new UPacketInfoListModel(&packetInfos_,&currentDisplayScheme_,this);
    packetInfoProxyModel_ = new UPacketInfoListProxyModel(this);
    packetInfoProxyModel_->setSourceModel(packetListModel_);
    packetList_->setModel(packetInfoProxyModel_);
    
    displayList_ = new UPacketDisplayList(&savedDisplaySchemes_,this);

    silentModePushButton_ = new QPushButton(tr("Silent Mode"),this);
    silentModePushButton_->setCheckable(true);
    silentModePushButton_->setChecked(silentMode_);
    showSendPacketsButton_ = new QPushButton(tr("Show Send"),this);
    showSendPacketsButton_->setCheckable(true);
    showSendPacketsButton_->setChecked(currentDisplayScheme_.showSendPackets);
    showRecvPacketsButton_ = new QPushButton(tr("Show Recv"),this);
    showRecvPacketsButton_->setCheckable(true);
    showRecvPacketsButton_->setChecked(currentDisplayScheme_.showRecvPackets);
    showOnlySelectedButton_ = new QPushButton(tr("Show Only Selected"),this);
    showOnlySelectedButton_->setCheckable(true);
    showOnlySelectedButton_->setChecked(showOnlySelectedPackets_);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(displayList_,30);
    mainLayout->addWidget(packetList_,100);
    UFlowLayout *bottomLayout = new UFlowLayout;
    bottomLayout->addWidget(silentModePushButton_);
    bottomLayout->addWidget(showSendPacketsButton_);
    bottomLayout->addWidget(showRecvPacketsButton_);
    bottomLayout->addWidget(showOnlySelectedButton_);

    mainLayout->addLayout(bottomLayout,0);
    packetListGroupBox_->setLayout(mainLayout);

    connect(packetList_,SIGNAL(visibilityChanged()),this,SLOT(updateFilters()));
    connect(packetListModel_,SIGNAL(saveSettingsRequested()),this,SLOT(saveSettings()));
    connect(silentModePushButton_,SIGNAL(toggled(bool)),this,SLOT(setSilentMode(bool)));
    connect(showOnlySelectedButton_,SIGNAL(toggled(bool)),this,SLOT(setShowOnlySelectedPackets(bool)));
    connect(showSendPacketsButton_,SIGNAL(toggled(bool)),this,SLOT(setShowSendPackets(bool)));
    connect(showRecvPacketsButton_,SIGNAL(toggled(bool)),this,SLOT(setShowRecvPackets(bool)));
    connect(displayList_,SIGNAL(saveSchemeRequested(const QString &)),this,SLOT(saveScheme(const QString &)));
    connect(displayList_,SIGNAL(loadSchemeRequested(const QString &)),this,SLOT(loadScheme(const QString &)));

}

void UPacketView::createPacketMonitorGroupBox()
{
    packetMonitorGroupBox_ = new QGroupBox(tr("Packet Monitor"),this);
    packetMonitor_ = new UPacketMonitor(this);
    packetMonitorModel_ = new UPacketMonitorModel(&packetInfos_,&packetDatas_,this);
    packetMonitorProxyModel_ = new UPacketMonitorProxyModel(this);
    packetMonitorProxyModel_->setDynamicSortFilter(true);
    packetMonitorProxyModel_->setSourceModel(packetMonitorModel_);
    packetMonitor_->setModel(packetMonitorProxyModel_);
    //packetMonitor_->setModel(packetMonitorModel_);
    autoScrollPushButton_ = new QPushButton(tr("Auto Scroll"),this);
    autoScrollPushButton_->setCheckable(true);
    clearPacketsButton_ = new QPushButton(tr("Clear Packets"),this);
    packetCountLabel_ = new QLabel(tr("Packet Count:0"),this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(packetCountLabel_);
    layout->addWidget(packetMonitor_);
    UFlowLayout *layout2 = new UFlowLayout;
    layout2->addWidget(autoScrollPushButton_);
    layout2->addWidget(clearPacketsButton_);
    layout->addLayout(layout2);
    packetMonitorGroupBox_->setLayout(layout);

    connect(autoScrollPushButton_,SIGNAL(toggled(bool)),this,SLOT(setAutoScroll(bool)));
    connect(clearPacketsButton_,SIGNAL(clicked()),this,SLOT(clearPackets()));
    
}

void UPacketView::addPacket(PacketType type, const char *packet,int packetSize )
{
    //假如静默模式，则不忽略要添加的封包。
    if(silentMode_)
    {
        return;
    }
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
        //新增加的封包是否显示？
        currentDisplayScheme_.visibilities[packetInfo] = true;
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
    if(showOnlySelectedPackets_)
    {
        QItemSelection selection = packetList_->selectionModel()->selection();
        foreach(QItemSelectionRange range,selection)
        {
            PacketInfo packetInfo;
            for(int i = range.top(); i <= range.bottom(); i++)
            {
                packetInfo.id = packetInfoProxyModel_->index(i,2).data().toString().toInt(0,16);
                QString type = packetInfoProxyModel_->index(i,1).data().toString();
                if(type == "Send")
                {
                    packetInfo.type = SendType;
                }
                else if(type == "Recv")
                {
                    packetInfo.type = RecvType;
                }
                
                if(currentDisplayScheme_.visibilities[packetInfo])
                {
                    if((packetInfo.type == SendType && currentDisplayScheme_.showSendPackets)
                        || (packetInfo.type == RecvType && currentDisplayScheme_.showRecvPackets))
                    {
                        filters[packetInfo.type].insert(packetInfo.id);
                    }
                }
            }
        }
    }
    else
    {
        for(int i = 0; i < packetInfos_.size(); i++)
        {
            if(currentDisplayScheme_.visibilities[packetInfos_[i]])
            {
                if((packetInfos_[i].type == SendType && currentDisplayScheme_.showSendPackets)
                    || (packetInfos_[i].type == RecvType && currentDisplayScheme_.showRecvPackets))
                {
                    filters[packetInfos_[i].type].insert(packetInfos_[i].id);
                }
            }
        }
    }
    packetMonitorProxyModel_->setFilters(filters);
}


void UPacketView::saveSettings()
{
    UTRACE("UI")<<"enter";
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UPacketView.cfg");
    if(!configFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("UPacketView"),
            tr("Can not open config file(UPacketView.cfg)."),
            QMessageBox::Ok);
        return;
    }
    QDataStream out(&configFile);
    out.setVersion(QDataStream::Qt_4_5);
    out<<Magic;
    out<<Version;
    if(Version == 1000)
    {
        out<<packetInfos_;
        out<<currentDisplayScheme_;
        out<<savedDisplaySchemes_;
        out<<silentMode_;
        out<<showOnlySelectedPackets_;
    }
}

void UPacketView::loadSettings()
{
    UTRACE("UI")<<"enter";
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UPacketView.cfg");
    if(!configFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("UPacketView"),
            tr("Can not open config file(UPacketView.cfg)."),
            QMessageBox::Ok);
        return;
    }
    QDataStream in(&configFile);
    in.setVersion(QDataStream::Qt_4_5);
    int version = 0;
    int magic = 0;
    in>>magic;
    if(magic != Magic)
    {
        return;
    }
    in>>version;
    if(version == 1000)
    {
        in>>packetInfos_;
        in>>currentDisplayScheme_;
        in>>savedDisplaySchemes_;
        in>>silentMode_;
        in>>showOnlySelectedPackets_;
    }

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

void UPacketView::setSilentMode( bool enable )
{
    silentMode_ = enable;
}

void UPacketView::setShowOnlySelectedPackets( bool enable )
{
    showOnlySelectedPackets_ = enable;
    if(enable)
    {
        connect(packetList_->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
            this,SLOT(updateFilters()));
    }
    else
    {
        disconnect(packetList_->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
            this,SLOT(updateFilters()));
    }
    updateFilters();
}

void UPacketView::setShowSendPackets( bool enable )
{
    currentDisplayScheme_.showSendPackets = enable;
    updateFilters();
}

void UPacketView::setShowRecvPackets( bool enable )
{
    currentDisplayScheme_.showRecvPackets = enable;
    updateFilters();
}

void UPacketView::saveScheme( const QString &schemeName )
{
    savedDisplaySchemes_[schemeName] = currentDisplayScheme_;
}

void UPacketView::loadScheme( const QString &schemeName )
{
    currentDisplayScheme_ = savedDisplaySchemes_[schemeName];
    showRecvPacketsButton_->setChecked(currentDisplayScheme_.showRecvPackets);
    showSendPacketsButton_->setChecked(currentDisplayScheme_.showSendPackets);
    packetListModel_->visibilityChange();
    updateFilters();
}

void UPacketView::clearPackets()
{
    packetMonitorModel_->removeRows(0,packetMonitorModel_->rowCount());
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
