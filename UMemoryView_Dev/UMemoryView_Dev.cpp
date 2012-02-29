#include "UMemoryView_Dev.h"

#include "../UniUI/UMemoryView.h"

using namespace uni;

UMemoryView_Dev::UMemoryView_Dev(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    memoryView_ = new UMemoryView(this);
    setCentralWidget(memoryView_);
}

UMemoryView_Dev::~UMemoryView_Dev()
{

}
