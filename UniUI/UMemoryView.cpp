#include "UMemoryView.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "UMemoryModel.h"
#include "UMemoryTable.h"

namespace uni
{

UMemoryView::UMemoryView( QWidget *parent /*= 0*/ )
:QWidget(parent)
{
    setupUI();
}

UMemoryView::~UMemoryView()
{

}

void UMemoryView::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout;
    addressLabel_ = new QLabel("Address:");
    layout->addWidget(addressLabel_);
    addressEdit_ = new QLineEdit();
    layout->addWidget(addressEdit_);
    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addLayout(layout);
    memoryModel_ = new UMemoryModel(this);
    memoryTable_ = new UMemoryTable;
    memoryTable_->setModel(memoryModel_);
    layout1->addWidget(memoryTable_);
    setLayout(layout1);
}

}//namespace uni