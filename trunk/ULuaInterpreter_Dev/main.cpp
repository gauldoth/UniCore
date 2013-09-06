#include <QtGui>
#include "ULuaInterpreter_Dev.h"

#include "../UniUI/ULuaInterpreter.h"

using namespace uni;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ULuaInterpreter w;
    w.show();
    return a.exec();
}
