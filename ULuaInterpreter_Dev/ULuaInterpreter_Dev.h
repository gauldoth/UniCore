#ifndef ULUAINTERPRETER_DEV_H
#define ULUAINTERPRETER_DEV_H

#include <QtGui>
#include "ui_ULuaInterpreter_Dev.h"

class ULuaInterpreter_Dev : public QMainWindow
{
    Q_OBJECT

public:
    ULuaInterpreter_Dev(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ULuaInterpreter_Dev();

private:
    Ui::ULuaInterpreter_DevClass ui;
};

#endif // ULUAINTERPRETER_DEV_H
