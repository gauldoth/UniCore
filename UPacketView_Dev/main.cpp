#include <QtGui/QApplication>
#include "UPacketView_Dev.h"

#include "..//UniCore//UDebug.h"
#include "..//UniUI//UStopwatchView.h"

using namespace uni;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UPacketView_Dev w;
    w.show();
    UStopwatchView stopwatchView;
    stopwatchView.show();
    UStopwatch stopwatch(0);
    return a.exec();
}
