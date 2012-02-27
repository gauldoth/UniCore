#include <QtGui/QApplication>
#include "UPacketView_Dev.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UPacketView_Dev w;
    w.show();
    return a.exec();
}
