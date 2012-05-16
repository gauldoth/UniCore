#include <QtGui/QApplication>
#include "UMemoryView_Dev.h"

#include "..//UniCore//UDebug.h"

using namespace uni;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UMemoryView_Dev w;
    UStopwatch stopwatch(0);
    w.show();
    return a.exec();
}
