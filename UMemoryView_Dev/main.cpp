#include <QtGui/QApplication>
#include "UMemoryView_Dev.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UMemoryView_Dev w;
    w.show();
    return a.exec();
}
