#include "UPacketInfoListModel.h"

#include <time.h>

#include <QMapIterator>

#include "../UniCore/ULog.h"

namespace uni
{

UPacketInfoListModel::UPacketInfoListModel(
    QList<UPacketView::PacketInfo> *packetInfos,
    UPacketView::DisplayScheme *currentDisplayScheme,
    QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,currentDisplayScheme_(currentDisplayScheme)
,packetInfos_(packetInfos)
{
}

UPacketInfoListModel::~UPacketInfoListModel()
{
}

QVariant UPacketInfoListModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    Q_ASSERT(index.isValid());
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case 0:
            {
                return tr("");
                break;
            }
        case 1:
            {
                switch(packetInfos_->at(index.row()).type)
                {
                case UPacketView::SendType:
                    {
                        return tr("Send");
                    }
                case UPacketView::RecvType:
                    {
                        return tr("Recv");
                    }
                default:
                    {
                        return tr("Unknown");
                    }
                }
                break;
            }
        case 2:
            {
                //将封包ID格式化为8位16进制。
                return tr("0x%1").arg(QString::number(packetInfos_->at(index.row()).id,16),4,QChar('0'));
                break;
            }
        case 3:
            {
                return packetInfos_->at(index.row()).name;
                break;
            }
        default:
            {
                return QVariant();
            }
        }
    }
    else if(role == Qt::BackgroundColorRole)
    {
        if(packetInfos_->at(index.row()).outOfDate)
        {
            return Qt::red;
        }
    }
    else if(role == Qt::CheckStateRole)
    {
        if(index.column() == 0)
        {
            if(currentDisplayScheme_->visibilities[packetInfos_->at(index.row())])
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        return QVariant();
    }
    return QVariant();
}

int UPacketInfoListModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return packetInfos_->size();
}

int UPacketInfoListModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 4;
}

QVariant UPacketInfoListModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if(orientation == Qt::Horizontal
        && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            {
                return tr("");
                break;
            }
        case 1:
            {
                return tr("Type");
                break;
            }
        case 2:
            {
                return tr("ID");
                break;
            }
        case 3:
            {
                return tr("Name");
                break;
            }
        default:
            {
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags UPacketInfoListModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid())
    {
        return 0;
    }
    if(index.column() == 3 || index.column() == 2)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else if(index.column() == 0)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool UPacketInfoListModel::setData( const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/ )
{
    if(role == Qt::EditRole)
    {
        if(index.column() == 3)
        {
            UTRACE<<value.toString().toStdWString();
            (*packetInfos_)[index.row()].name = value.toString();
            //这里暂时不通知封包监视器数据改变，因为代价较大。
            //且封包监视器刷新频繁，不通知应该影响不大。
            emit dataChanged(index,index);
            emit saveSettingsRequested();
            return true;
        }
        else if(index.column() == 2)
        {
            //假如要修改成的目标ID存在，将原ID对应的封包信息置为过期。
            //修改ID，并将修改的封包信息改为未过期。
            for(int i = 0; i < packetInfos_->size(); i++)
            {
                if(i != index.row())
                {
                    if(value.toString().toInt(0,16) == packetInfos_->at(i).id 
                        && (*packetInfos_)[index.row()].type == packetInfos_->at(i).type)
                    {
                        (*packetInfos_)[i].outOfDate = true;
                        emit dataChanged(this->index(i,0),this->index(i,columnCount()));
                        break;
                    }
                }
            }
            (*packetInfos_)[index.row()].id = value.toString().toInt(0,16);
            (*packetInfos_)[index.row()].outOfDate = false;
            emit dataChanged(this->index(index.row(),0),this->index(index.row(),columnCount()));
            emit saveSettingsRequested();
        }
        return false;
    }
    else if(index.column() == 0 && role == Qt::CheckStateRole)
    {
        if(value == Qt::Checked)
        {
            currentDisplayScheme_->visibilities[(*packetInfos_)[index.row()]] = true; 
        }
        else
        {
            currentDisplayScheme_->visibilities[(*packetInfos_)[index.row()]] = false;
        }
        emit dataChanged(index,index);
        return true;
    }
    else
    {
        return false;
    }
}

void UPacketInfoListModel::addPacketInfo( UPacketView::PacketInfo packetInfo )
{
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    if(!packetInfos_->contains(packetInfo))
    {
        currentDisplayScheme_->visibilities[packetInfo] = true;
        packetInfos_->push_back(packetInfo);
    }
    endInsertRows();
}

bool UPacketInfoListModel::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex()*/ )
{
    beginRemoveRows(parent,row,row+count-1);
    for(int i = 0; i < count; i++)
    {
        currentDisplayScheme_->visibilities.remove((*packetInfos_)[row]);
        packetInfos_->removeAt(row);
    }
    endRemoveRows();
    return true;
}

void UPacketInfoListModel::visibilityChange()
{
    emit dataChanged(index(0,0),index(rowCount()-1,0));
}


bool UPacketInfoListProxyModel::lessThan( const QModelIndex & left, const QModelIndex & right ) const
{
    QString leftType = sourceModel()->index(left.row(),1).data().toString();
    QString rightType = sourceModel()->index(right.row(),1).data().toString();
    int leftID = sourceModel()->index(left.row(),2).data().toString().toInt(0,16);
    int rightID = sourceModel()->index(right.row(),2).data().toString().toInt(0,16);
    if(leftType == "Send")
    {
        if(rightType == "Recv")
        {
            return false;
        }
        else if(rightType == "Send")
        {
            if(leftID < rightID)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {//未知类型。
            return true;
        }
    }
    else if(leftType == "Recv")
    {
        if(rightType == "Send")
        {
            return true;
        }
        else if(rightType == "Recv")
        {
            if(leftID < rightID)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    else
    {
        if(rightType != "Unknown")
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}

UPacketInfoListProxyModel::UPacketInfoListProxyModel( QObject *parent /*= 0*/ )
:QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(2,Qt::AscendingOrder);
}

UPacketInfoListProxyModel::~UPacketInfoListProxyModel()
{

}

}//namespace uni