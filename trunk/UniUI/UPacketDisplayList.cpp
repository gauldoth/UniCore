#include "UPacketDisplayList.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QPushButton>
#include <QStringListModel>
#include <QVBoxLayout>

#include "UFlowLayout.h"

namespace uni
{

UPacketDisplayList::UPacketDisplayList( QMap<QString,UPacketView::DisplayScheme> *displaySchemes,
                                       QWidget *parent /*= 0*/ )
:displaySchemes_(displaySchemes)
,QWidget(parent)
{
    displayList_ = new QListView(this);
    model_ = new QStringListModel(displaySchemes_->keys());
    displayList_->setModel(model_);
    displayList_->setEditTriggers(displayList_->editTriggers()&(~QAbstractItemView::DoubleClicked));
    newButton_ = new QPushButton(tr("&New"));
    saveButton_ = new QPushButton(tr("&Save"));
    deleteButton_ = new QPushButton(tr("&Del"));
    newSchemeDialog_ = new UPacketDisplayList_NewDialog(this);

    connect(deleteButton_,SIGNAL(clicked()),this,SLOT(deleteSelectedScheme()));
    connect(newButton_,SIGNAL(clicked()),this,SLOT(newScheme()));
    connect(saveButton_,SIGNAL(clicked()),this,SLOT(saveScheme()));
    connect(newSchemeDialog_,SIGNAL(accepted()),this,SLOT(newSchemeDialogAccepted()));
    connect(displayList_,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(loadScheme(const QModelIndex &)));

    QVBoxLayout *layout = new QVBoxLayout;
    UFlowLayout *flowLayout = new UFlowLayout;
    flowLayout->addWidget(newButton_);
    flowLayout->addWidget(saveButton_);
    flowLayout->addWidget(deleteButton_);
    layout->addWidget(displayList_);
    layout->addLayout(flowLayout);

    setLayout(layout);
}

UPacketDisplayList::~UPacketDisplayList()
{

}

void UPacketDisplayList::deleteSelectedScheme()
{
    QModelIndex index = displayList_->currentIndex();
    displaySchemes_->remove(index.data().toString());
    model_->removeRow(index.row());
}

void UPacketDisplayList::newScheme()
{
    newSchemeDialog_->show();
    newSchemeDialog_->raise();
    newSchemeDialog_->activateWindow();
}

void UPacketDisplayList::saveScheme()
{
    QModelIndex index = displayList_->currentIndex();
    emit saveSchemeRequested(index.data().toString());
}

void UPacketDisplayList::newSchemeDialogAccepted()
{
    if(!model_->stringList().count(newSchemeDialog_->schemeName().trimmed()))
    {
        //不存在则创建。
        model_->insertRow(0);
        model_->setData(model_->index(0,0),newSchemeDialog_->schemeName());
        emit saveSchemeRequested(newSchemeDialog_->schemeName());
    }
    else
    {
        QMessageBox::information(this, tr("Packet View"),
            tr("Name already exists!"),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void UPacketDisplayList::loadScheme( const QModelIndex &index )
{
    emit loadSchemeRequested(index.data().toString());
}


UPacketDisplayList_NewDialog::UPacketDisplayList_NewDialog( QWidget *parent /*= 0*/ )
:QDialog(parent)
{
    QLabel *schemeNameLabel = new QLabel(tr("Name:"));
    schemeNameEdit_ = new QLineEdit(this);
    QPushButton *okButton = new QPushButton(tr("&Ok"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(okButton,QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(schemeNameLabel);
    layout2->addWidget(schemeNameEdit_);
    layout->addLayout(layout2);
    layout->addWidget(buttonBox);
    
    setLayout(layout);
}

UPacketDisplayList_NewDialog::~UPacketDisplayList_NewDialog()
{

}


}//namespace uni