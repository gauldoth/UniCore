#include "UMemoryView.h"

#include <QAction>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QFile>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "UMemoryModel.h"
#include "UMemoryTable.h"
#include "../UniCore/ULua.h"



namespace uni
{

UMemoryView::UMemoryView( QWidget *parent /*= 0*/)
:QWidget(parent)
{
    //初始化Lua。


    memoryModel_ = new UMemoryModel(this);
    memoryTable_ = new UMemoryTable(this);
    memoryTable_->setModel(memoryModel_);
    addressLabel_ = new QLabel("Address:");
    addressEdit_ = new QLineEdit();

    //读取保存的列信息。
    readSettings();

    memoryModel_->initCommonColumn();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(addressLabel_);
    layout->addWidget(addressEdit_);
    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addLayout(layout);
    layout1->addWidget(memoryTable_);
    setLayout(layout1);

    connect(addressEdit_,SIGNAL(textChanged(const QString &)),this,SLOT(addressEditChanged(const QString &)));
    connect(memoryModel_,SIGNAL(columnInfoChanged()),this,SLOT(writeSettings()));
    connect(memoryTable_,SIGNAL(changeAddress(int)),this,SLOT(setAddress(int)));
}

UMemoryView::~UMemoryView()
{

}

void UMemoryView::setAddress( int address )
{
    memoryModel_->setAddress(address);
}

void UMemoryView::addressEditChanged( const QString &text )
{
    setAddress(text.toInt(0,0));
}

void UMemoryView::readSettings()
{
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UMemoryView.cfg");
    configFile.open(QIODevice::ReadOnly);
    QDataStream in(&configFile);

    // Read the version
    int version;
    in >> version;
    if (version == 1000)
    {
        in>>*memoryModel_;
    }
}

void UMemoryView::closeEvent( QCloseEvent *event )
{

}

void UMemoryView::writeSettings()
{
    QFile configFile(QCoreApplication::applicationDirPath() + "\\UMemoryView.cfg");
    configFile.open(QIODevice::WriteOnly);
    QDataStream out(&configFile);
    out.setVersion(QDataStream::Qt_4_5);
    int version = 1000;
    //保存版本。
    out<<1000;
    if(version == 1000)
    {
        out<<*memoryModel_;
    }
}

void UMemoryView::runLuaCFunction( lua_CFunction function )
{
    memoryModel_->runLuaCFunction(function);
}





}//namespace uni

