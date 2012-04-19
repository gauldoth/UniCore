#include "UMemoryTable.h"

#include <QAction>

#include <QHeaderView>
#include <QtGui>

#include "UMemoryModel.h"

namespace uni
{

AddColumnDialog::AddColumnDialog( QWidget *parent )
    :QDialog(parent)
{
    QLabel *titleLabel_ = new QLabel(tr("Column Title:"));
    titleEdit_ = new QLineEdit("",this);
    
    QLabel *getDataScriptLabel = new QLabel(tr("Get Data Script:"));
    getDataScriptEdit_ = new QTextEdit("",this);

    QLabel *setDataScriptLabel = new QLabel(tr("Set Data Script"));
    setDataScriptEdit_ = new QTextEdit("",this);

    QLabel *dataColorScriptLabel = new QLabel(tr("Data Color Script"));
    dataColorScriptEdit_ = new QTextEdit("",this);

    setButton_ = new QPushButton(tr("&Set"),this);
    setButton_->setDefault(true);
    cancelButton_ = new QPushButton(tr("&Cancel"),this);
    buttonBox_ = new QDialogButtonBox(this);
    buttonBox_->addButton(setButton_,QDialogButtonBox::AcceptRole);
    buttonBox_->addButton(cancelButton_,QDialogButtonBox::RejectRole);


    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(titleLabel_);
    layout1->addWidget(titleEdit_);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(getDataScriptLabel);
    mainLayout->addWidget(getDataScriptEdit_);
    mainLayout->addWidget(setDataScriptLabel);
    mainLayout->addWidget(setDataScriptEdit_);
    mainLayout->addWidget(dataColorScriptLabel);
    mainLayout->addWidget(dataColorScriptEdit_);
    mainLayout->addWidget(buttonBox_);
    setLayout(mainLayout);

    setWindowTitle(tr("Add Column"));

    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));

}

void AddColumnDialog::clearExistContents()
{
    titleEdit_->clear();
    getDataScriptEdit_->clear();
    setDataScriptEdit_->clear();
    dataColorScriptEdit_->clear();
}


AddColumnDialog::~AddColumnDialog()
{

}

QString AddColumnDialog::title() const
{
    return titleEdit_->text();
}

QString AddColumnDialog::getDataScript() const
{
    return getDataScriptEdit_->toPlainText();
}

QString AddColumnDialog::setDataScript() const
{
    return setDataScriptEdit_->toPlainText();
}

QString AddColumnDialog::dataColorScript() const
{
    return dataColorScriptEdit_->toPlainText();
}

UMemoryTable::UMemoryTable( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    QAction *addColumnAction = new QAction(tr("&Add Column"),this);
    QAction *editColumnAction = new QAction(tr("&Edit Column"),this);
    
    horizontalHeader()->addAction(addColumnAction);
    horizontalHeader()->addAction(editColumnAction);
    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    connect(addColumnAction,SIGNAL(triggered(bool)),this,SLOT(showAddColumnDialog()));
    connect(editColumnAction,SIGNAL(triggered(bool)),this,SLOT(showEditColumnDialog()));
    
}

UMemoryTable::~UMemoryTable()
{

}

void UMemoryTable::showEditColumnDialog()
{
    //获得列信息。
    //修改完写入。
    UMemoryModel *model = static_cast<UMemoryModel *>(this->model());

    //model->columnInfo();
    
}

void UMemoryTable::showAddColumnDialog()
{
    static AddColumnDialog *addColumnDialog = new AddColumnDialog(this);
    addColumnDialog->show();
    addColumnDialog->raise();
    addColumnDialog->activateWindow();
}

}//namespace uni