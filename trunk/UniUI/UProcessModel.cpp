#include "UProcessModel.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "psapi.h"

#include <assert.h>

#include <QTimer>

#include "ULog.h"
#include "UProcess.h"

namespace uni
{

class ProcessInfo
{
public:
    ProcessInfo()
        :pid_(0)
    {
    }
    QString name_;
    unsigned int pid_;
};

UProcessModel::UProcessModel(QObject *parent /*= 0*/)
:QAbstractTableModel(parent)
,interval_(DefaultInterval)
{
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refresh()));
    timer->start(interval_);
    refresh();
}

UProcessModel::~UProcessModel()
{

}

int UProcessModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return processInfos_.size();
}

int UProcessModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return FieldCount;
}

QVariant UProcessModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            switch(index.column())
            {
            case NameColumn:
                {
                    return processInfos_[index.row()].name_;
                    break;
                }
            case PIDColumn:
                {
                    return processInfos_[index.row()].pid_;
                    break;
                }
            default:
                {
                    assert(!"无效的列。");
                    break;
                }
            }
            break;
        }
    default:
        {
            return QVariant();
            break;
        }
    }
    return QVariant();
}

void UProcessModel::refresh()
{
    DWORD pids[MaxProcessCount];
    DWORD count = 0;

    if(!EnumProcesses(pids,sizeof(pids),&count))
    {
        UERROR<<"EnumProcesses失败。"<<lasterr;
        return;
    }

    count /= sizeof(DWORD);

    UTRACE<<"1";
    UTRACE<<"2";

    //emit layoutAboutToBeChanged();

    //更新进程信息。
    UTRACE<<"更新进程信息。";
    for(int i = 0; i < static_cast<int>(count); i++)
    {
        bool exists = false;
        for(int j = 0; j < processInfos_.size(); j++)
        {
            if(pids[i] == processInfos_[j].pid_)
            {
                exists = true;
                //processInfos_[j].name_ = QString::fromStdWString(GetProcessName(pids[i]));
                setData(index(j,NameColumn),QString::fromStdWString(GetProcessName(pids[i])));
                break;
            }
        }
        if(!exists)
        {
            insertRow(0);
            setData(index(0,NameColumn),QString::fromStdWString(GetProcessName(pids[i])));
            setData(index(0,PIDColumn),QVariant(static_cast<unsigned int>(pids[i])));
        }
    }

    //判断进程是否已经不在列表中。
    UTRACE<<"判断进程是否已经不在列表中。";
    int i = 0;
    while(i < static_cast<int>(processInfos_.size()))
    {
        bool exists = false;
        for(int j = 0; j < static_cast<int>(count); j++)
        {
            if(processInfos_[i].pid_ == pids[j])
            {
                exists = true;
                break;
            }
        }

        if(!exists)
        {
            UTRACE<<"before remove";
            //processInfos_.removeAt(i);
            removeRow(i);

            UTRACE<<"after remove";
        }
        else
        {
            i++;
        }
    }
    UTRACE<<"判断进程是否已经不在列表中完成。";

    //emit layoutChanged();
}

QVariant UProcessModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(orientation == Qt::Horizontal)
        {
            switch(section)
            {
            case NameColumn:
                {
                    return tr("Process");
                    break;
                }
            case PIDColumn:
                {
                    return tr("PID");
                    break;
                }
            default:
                {
                    assert(!"无效的列。");
                    break;
                }
            }
        }
        else if(orientation == Qt::Vertical)
        {

        }
    }

    return QVariant();
}

bool UProcessModel::setData(const QModelIndex &index,const QVariant &value,int role /*= Qt::EditRole*/)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        
        switch(index.column())
        {
        case NameColumn:
            {
                processInfos_[row].name_ = value.toString();
                break;
            }
        case PIDColumn:
            {
                processInfos_[row].pid_ = value.toUInt();
                break;
            }
        default:
            {
                assert(!"无效的列。");
                break;
            }
        }
        emit(dataChanged(index,index));
        return true;
    }

    return false;
}

bool UProcessModel::insertRows(int row, int count, const QModelIndex &/*parent = QModelIndex()*/)
{
    beginInsertRows(QModelIndex(),row,row+count-1);

    ProcessInfo info;
    for(int i = 0; i < count; i++)
    {
        processInfos_.insert(row,info);
    }

    endInsertRows();
    return true;
}

bool UProcessModel::removeRows(int row, int count, const QModelIndex &/*parent = QModelIndex()*/)
{
    beginRemoveRows(QModelIndex(),row,row+count-1);

    for(int i = 0; i < count; i++)
    {
        processInfos_.removeAt(row);
    }
    endRemoveRows();

    return true;
}

}//namespace uni