#include <QtGui/QApplication>
#include "UPacketView_Dev.h"

#include "..//UniCore//UDebug.h"
#include "..//UniUI//UStopwatchView.h"
#include "..//UniCore//UConfig.h"

using namespace uni;

int main(int argc, char *argv[])
{
    UConfig config("./≈‰÷√.ini");
    config.set("µ»º∂≈‰÷√/%d/µÿÕºID","2",100);
    QApplication a(argc, argv);
    UPacketView_Dev w;
    w.show();
    UStopwatchView stopwatchView;
    stopwatchView.show();
    UStopwatch stopwatch(0);
    return a.exec();
}
