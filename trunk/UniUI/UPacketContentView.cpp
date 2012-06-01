#include "UPacketContentView.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>

namespace uni
{


UPacketContentView::UPacketContentView( QWidget *parent /*= 0*/ )
:QDialog(parent)
{
    packetContentEdit_ = new QTextEdit(this);
    packetContentEdit_->setMinimumWidth(320);
    packetTextEdit_ = new QTextEdit(this);
    packetTextEdit_->setMinimumWidth(220);
    QSplitter *splitter = new QSplitter(Qt::Horizontal,this);
    splitter->addWidget(packetContentEdit_);
    splitter->addWidget(packetTextEdit_);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    setLayout(layout);
}

UPacketContentView::~UPacketContentView()
{

}


void UPacketContentView::setPacket( UPacketView::PacketData *packetData )
{
    packetContentEdit_->setText(packetData->contentTooltip);
    packetTextEdit_->setText(packetData->textTooltip);
}

}//namespace uni