#include "ULogConfigPanel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>
#include <QTimer.h>
#include <QVBoxLayout>

#include <boost/foreach.hpp>

#include "ULog.h"

namespace uni
{

ULogConfigPanel::ULogConfigPanel( QWidget *parent )
:QWidget(parent)
{
    setupUI();

    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateNames()));
    updateTimer->start(1000);
}

ULogConfigPanel::~ULogConfigPanel()
{

}

void ULogConfigPanel::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout;
    {
        logNameList_ = new QListView(this);
        QStringListModel *model = new QStringListModel(this);
        logNameList_->setModel(model);
        connect(logNameList_,SIGNAL(activated(const QModelIndex &)),this,SLOT(updateConfig(const QModelIndex &)));
        layout->addWidget(logNameList_);
        QVBoxLayout *layout1 = new QVBoxLayout;
        {
            isOutputCheckBox_ = new QCheckBox(this);
            layout1->addWidget(isOutputCheckBox_);
            logLevelComboBox_ = new QComboBox(this);
            QStringList levels = QString("all trace debug info warn error fatal off").split(" ");
            for(int i = 0; i < levels.size(); i++)
            {
                logLevelComboBox_->addItem(levels[i]);
            }
            layout1->addWidget(logLevelComboBox_);
        }
        layout->addLayout(layout1);
    }
    setLayout(layout);
}

void ULogConfigPanel::updateNames()
{
    logNameList_->model()->removeRows(0,logNameList_->model()->rowCount());
    BOOST_FOREACH(std::string name,ULog::names())
    {
        logNameList_->model()->insertRow(0);
        logNameList_->model()->setData(logNameList_->model()->index(0,0),QVariant(QString::fromStdString(name)));
    }
}

void ULogConfigPanel::updateConfig( const QModelIndex &index )
{
    //ULog::logLevel(
}

}//namespace uni