#include "UPacketInfoListModel.h"

#include <time.h>

#include <QMapIterator>

namespace uni
{

UPacketInfoListModel::UPacketInfoListModel(QList<UPacketView::PacketInfo> *packetInfos,QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,packetInfos_(packetInfos)
{
    qSort(*packetInfos_);
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
    else if(role == Qt::CheckStateRole)
    {
        if(index.column() == 0)
        {
            if(packetInfos_->at(index.row()).visible)
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

    if(index.column() == 0)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool UPacketInfoListModel::setData( const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/ )
{
    if(index.column() == 0 && role == Qt::CheckStateRole)
    {
        if(value == Qt::Checked)
        {
            (*packetInfos_)[index.row()].visible = true; 
        }
        else
        {
            (*packetInfos_)[index.row()].visible = false;
        }
        emit dataChanged(index,index);
        emit visibilityChanged();
        return true;
    }
    else
    {
        return false;
    }
}

void UPacketInfoListModel::selectAll()
{
    for(int i = 0; i < packetInfos_->size(); i++)
    {
        (*packetInfos_)[i].visible = true;
    }
    
    emit dataChanged(index(0,0),index(rowCount(),0));
    emit visibilityChanged();
}

void UPacketInfoListModel::deselectAll()
{
    for(int i = 0; i < packetInfos_->size(); i++)
    {
        (*packetInfos_)[i].visible = false;
    }
    emit dataChanged(index(0,0),index(rowCount(),0));
    emit visibilityChanged();
}

void UPacketInfoListModel::addPacketInfo( UPacketView::PacketInfo packetInfo )
{
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    if(!packetInfos_->contains(packetInfo))
    {
        packetInfos_->push_back(packetInfo);
        qSort(*packetInfos_);
    }
    endInsertRows();
}

bool UPacketInfoListModel::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex()*/ )
{
    beginRemoveRows(parent,row,row+count-1);
    for(int i = 0; i < count; i++)
    {
        packetInfos_->removeAt(row);
    }
    endRemoveRows();
    return true;
}

}//namespace uni