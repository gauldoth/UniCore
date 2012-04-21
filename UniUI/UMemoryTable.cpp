#include "UMemoryTable.h"

#include <QAction>

#include <QHeaderView>
#include <QtGui>

#include "../UniCore/ULog.h"

#include "UMemoryModel.h"

namespace uni
{

EditColumnDialog::EditColumnDialog( QWidget *parent )
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

void EditColumnDialog::clearExistContents()
{
    titleEdit_->clear();
    getDataScriptEdit_->clear();
    setDataScriptEdit_->clear();
    dataColorScriptEdit_->clear();
}


EditColumnDialog::~EditColumnDialog()
{

}

QString EditColumnDialog::columnTitle() const
{
    return titleEdit_->text();
}

QString EditColumnDialog::getDataScript() const
{
    return getDataScriptEdit_->toPlainText();
}

void EditColumnDialog::getDataScript( const QString &script )
{
    getDataScriptEdit_->setText(script);
}

QString EditColumnDialog::setDataScript() const
{
    return setDataScriptEdit_->toPlainText();
}

void EditColumnDialog::setDataScript( const QString &script )
{
    setDataScriptEdit_->setText(script);
}

QString EditColumnDialog::dataColorScript() const
{
    return dataColorScriptEdit_->toPlainText();
}

void EditColumnDialog::dataColorScript( const QString &script )
{
    dataColorScriptEdit_->setText(script);
}

void EditColumnDialog::setColumnTitle( const QString &title )
{
    titleEdit_->setText(title);
}

UMemoryTable::UMemoryTable( QWidget *parent /*= 0*/ )
:QTableView(parent)
{
    addColumnAction_ = new QAction(tr("&Add Column"),this);
    editColumnAction_ = new QAction(tr("&Edit Column"),this);
    deleteColumnAction_ = new QAction(tr("&Delete Column"),this);
    headerMenu_ = new QMenu(this);
    headerMenu_->addAction(addColumnAction_);
    headerMenu_->addAction(editColumnAction_);
    headerMenu_->addAction(deleteColumnAction_);

    editColumnDialog_ = new EditColumnDialog(this);
    
    connect(addColumnAction_,SIGNAL(triggered(bool)),this,SLOT(showAddColumnDialog()));
    connect(editColumnAction_,SIGNAL(triggered(bool)),this,SLOT(showEditColumnDialog()));
    connect(deleteColumnAction_,SIGNAL(triggered(bool)),this,SLOT(deleteColumn()));

    connect(editColumnDialog_,SIGNAL(accepted()),this,SLOT(editColumnDialogAccepted()));
}

UMemoryTable::~UMemoryTable()
{

}


void UMemoryTable::showAddColumnDialog()
{
    editColumnDialog_->setColumn(-1);
    editColumnDialog_->show();
    editColumnDialog_->raise();
    editColumnDialog_->activateWindow();
}


void UMemoryTable::showEditColumnDialog()
{
    //获得列信息。
    //修改完写入。
    editColumnDialog_->setColumn(lastClickedSection_);
    UMemoryModel *model = static_cast<UMemoryModel *>(this->model());
    UMemoryModel::ColumnInfo columnInfo = model->columnInfo(lastClickedSection_);
    editColumnDialog_->setColumnTitle(columnInfo.title);
    editColumnDialog_->getDataScript(columnInfo.getDataScript);
    editColumnDialog_->setDataScript(columnInfo.setDataScript);
    editColumnDialog_->dataColorScript(columnInfo.dataColorScript);
    editColumnDialog_->show();
    editColumnDialog_->raise();
    editColumnDialog_->activateWindow();

    //model->columnInfo();
    
}

void UMemoryTable::editColumnDialogAccepted()
{
    UMemoryModel::ColumnInfo columnInfo;
    int column = editColumnDialog_->column();
    columnInfo.title = editColumnDialog_->columnTitle();
    columnInfo.getDataScript = editColumnDialog_->getDataScript();
    columnInfo.setDataScript = editColumnDialog_->setDataScript();
    columnInfo.dataColorScript = editColumnDialog_->dataColorScript();
    UMemoryModel *model = static_cast<UMemoryModel *>(this->model());
    if(column == -1)
    {
        model->addColumnInfo(columnInfo);
    }
    else
    {
        model->setColumnInfo(column,columnInfo);
    }
}

void UMemoryTable::contextMenuEvent( QContextMenuEvent *event )
{
    int section = horizontalHeader()->logicalIndexAt(event->pos());
    editColumnAction_->setEnabled(false);
    deleteColumnAction_->setEnabled(false);
    if(section != -1)
    {
        lastClickedSection_ = section;
        UTRACE<<section;
        editColumnAction_->setEnabled(true);
        deleteColumnAction_->setEnabled(true);
        headerMenu_->popup(event->globalPos());
    }
}

void UMemoryTable::deleteColumn()
{
    model()->removeColumn(lastClickedSection_);
}

}//namespace uni