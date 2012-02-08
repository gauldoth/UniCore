#include "UMemoryView.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "UMemoryModel.h"
#include "UMemoryTable.h"

namespace uni
{

UMemoryView::UMemoryView( QWidget *parent /*= 0*/,lua_State *state /*=0*/)
:QWidget(parent)
{
    memoryModel_ = new UMemoryModel(this,state);
    memoryModel_->addColumn("Address","return string.format(\"0x%08x\",address)");
    setupUI();
    QAction *addColumnAction = new QAction("Add Column",this);
    memoryTable_->horizontalHeader()->addAction(addColumnAction);
    memoryTable_->horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(addColumnAction,SIGNAL(triggered(bool)),this,SLOT(showAddColumnDialog()));

    loadColumnInfos();
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
    connect(addressEdit_,SIGNAL(textChanged(const QString &)),this,SLOT(addressEditChanged(const QString &)));
    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addLayout(layout);

    memoryTable_ = new UMemoryTable;
    memoryTable_->setModel(memoryModel_);
    layout1->addWidget(memoryTable_);
    setLayout(layout1);

    //添加列的对话框。
    addColumnDialog_ = new AddColumnDialog(this);
    connect(addColumnDialog_,SIGNAL(accepted()),this,SLOT(addColumnDialogAccepted()));
}

void UMemoryView::setAddress( int address )
{
    memoryModel_->setAddress(address);
}

void UMemoryView::addressEditChanged( const QString &text )
{
    setAddress(text.toInt(0,0));
}

void UMemoryView::addColumn( const QString &title,const QString &calculus )
{
    memoryModel_->addColumn(title,calculus);
}

void UMemoryView::showAddColumnDialog()
{
    addColumnDialog_->clearExistContents();
    addColumnDialog_->show();
    //memoryModel_->addColumn(addColumnDialog_->title(),addColumnDialog_->calculus());
}

void UMemoryView::addColumnDialogAccepted()
{
    memoryModel_->addColumn(addColumnDialog_->title(),addColumnDialog_->calculus());
}

void UMemoryView::loadColumnInfos()
{

}

void UMemoryView::closeEvent( QCloseEvent *event )
{

}

void UMemoryView::saveColumnInfos()
{

}


AddColumnDialog::AddColumnDialog( QWidget *parent )
:QDialog(parent)
{
    QLabel *titleLabel_ = new QLabel(tr("Column Title:"));
    QLabel *calculusLabel_ = new QLabel(tr("Calculus Script:"));
    titleEdit_ = new QLineEdit("",this);
    calculusEdit_ = new QTextEdit("",this);
    setButton_ = new QPushButton(tr("&Set"),this);
    setButton_->setDefault(true);
    cancelButton_ = new QPushButton(tr("&Cancel"),this);
    buttonBox_ = new QDialogButtonBox(this);
    buttonBox_->addButton(setButton_,QDialogButtonBox::AcceptRole);
    buttonBox_->addButton(cancelButton_,QDialogButtonBox::RejectRole);
    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(titleLabel_);
    layout1->addWidget(titleEdit_);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(calculusLabel_);
    mainLayout->addWidget(calculusEdit_);
    mainLayout->addWidget(buttonBox_);
    setLayout(mainLayout);

    setWindowTitle(tr("Add Column"));

}

void AddColumnDialog::clearExistContents()
{
    titleEdit_->clear();
    calculusEdit_->clear();
}


AddColumnDialog::~AddColumnDialog()
{
    
}

QString AddColumnDialog::title() const
{
    return titleEdit_->text();
}

QString AddColumnDialog::calculus() const
{
    return calculusEdit_->toPlainText();
}

}//namespace uni