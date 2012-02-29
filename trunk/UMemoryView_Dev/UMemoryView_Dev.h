#ifndef UMEMORYVIEW_DEV_H
#define UMEMORYVIEW_DEV_H

#include <QtGui/QMainWindow>
#include "ui_UMemoryView_Dev.h"

namespace uni
{
class UMemoryView;
}//namespace uni

class UMemoryView_Dev : public QMainWindow
{
    Q_OBJECT

public:
    UMemoryView_Dev(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~UMemoryView_Dev();

private:
    Ui::UMemoryView_DevClass ui;
    uni::UMemoryView *memoryView_;
};

#endif // UMEMORYVIEW_DEV_H
