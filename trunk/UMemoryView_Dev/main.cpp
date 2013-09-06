#include <QtGui>
#include "UMemoryView_Dev.h"

#include "..//UniCore//UDebug.h"

using namespace uni;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UMemoryView_Dev w;
    w.show();
    return a.exec();
}
