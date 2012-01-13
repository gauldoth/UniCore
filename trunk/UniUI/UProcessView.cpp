#include "UProcessView.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QSortFilterProxyModel>

#include "UProcessModel.h"
#include "../UniCore/ULog.h"

namespace uni
{

UProcessView::UProcessView(QWidget *parent /*= 0*/)
:QTableView(parent)
,killProcessAction_(0)
,selectColumnAction_(0)
{
    //设置Model。
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    UProcessModel *processModel = new UProcessModel(this);
    connect(this,SIGNAL(processTerminated(unsigned int)),processModel,SLOT(refresh()));

    proxyModel->setSourceModel(processModel);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    setModel(proxyModel);
    setSortingEnabled(true);
    
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->hide();
    setSelectionMode(QAbstractItemView::SingleSelection);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setupActions();

    setupConnections();
}

UProcessView::~UProcessView()
{
}

bool UProcessView::killProcess()
{
    bool result = true;
    int selectedRow = selectionModel()->currentIndex().row();
    unsigned int pid = model()->data(model()->index(selectedRow,UProcessModel::PIDColumn)).toUInt();
    UTRACE<<"pid:"<<pid;
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,pid);
    if(!TerminateProcess(hProcess,0))
    {
        UERROR<<"TerminateProcess失败。"<<lasterr;
        QMessageBox msgBox(this);
        msgBox.setText(tr("Can not kill process."));
        msgBox.exec();
        result = false;
    }

    CloseHandle(hProcess);
    emit processTerminated(pid);
    return result;
}

void UProcessView::setupActions()
{
    //结束进程。
    killProcessAction_ = new QAction(tr("Kill Process"),this);
    connect(killProcessAction_,SIGNAL(triggered()),this,SLOT(killProcess()));
    addAction(killProcessAction_);

    //选择列...
    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    selectColumnAction_ = new QAction(tr("Select Column..."),horizontalHeader());
    horizontalHeader()->addAction(selectColumnAction_);

}

void UProcessView::setupConnections()
{
}

}//namespace uni