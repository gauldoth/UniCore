#include "UPacketView_Dev.h"

#include <time.h>

#include <QTimer>

#include "../UniUI/UPacketView.h"
#include "../UniCore/UMemory.h"

using namespace uni;

UPacketView_Dev::UPacketView_Dev(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    srand(time(0));
    packetView_ = new UPacketView(this);
    setCentralWidget(packetView_);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(addPacketTest()));
    timer->start(1);
}

UPacketView_Dev::~UPacketView_Dev()
{

}

void UPacketView_Dev::addPacketTest()
{
    UPacketView::PacketType type = static_cast<UPacketView::PacketType>(rand()%2);
    int len = 2+rand()%800;
    char *packet = new char[len];
    GetAt<unsigned short>(packet,0) = rand()%250;
    for(int i = 2; i < len; i++)
    {
        packet[i] = rand()%256;
    }
    packetView_->addPacket(type,packet,len);

}
