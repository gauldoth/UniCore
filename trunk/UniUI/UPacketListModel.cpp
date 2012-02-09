#include "UPacketListModel.h"

#include <time.h>

#include <QMapIterator>

namespace uni
{

UPacketListModel::UPacketListModel(QMap<int,PacketInfo> *packetInfos,QWidget *parent /*= 0*/ )
:packetInfos_(packetInfos)
{

}

UPacketListModel::~UPacketListModel()
{
}

QVariant UPacketListModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    Q_ASSERT(index.isValid());
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case 0:
            {
                return QVariant();
                break;
            }
        case 1:
            {
                QMapIterator<int,PacketInfo> it(*packetInfos_);
                int i = 0;
                while(it.hasNext())
                {
                    it.next();
                    if(i == index.row())
                    {
                        return it.value().id;
                    }
                    i++;
                }

                return QVariant();
                break;
            }
        case 2:
            {
                QMapIterator<int,PacketInfo> it(*packetInfos_);
                int i = 0;
                while(it.hasNext())
                {
                    it.next();
                    if(i == index.row())
                    {
                        return it.value().name;
                    }
                    i++;
                }

                return QVariant();
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
            QMapIterator<int,PacketInfo> it(*packetInfos_);
            int i = 0;
            while(it.hasNext())
            {
                it.next();
                if(i == index.row())
                {
                    if(it.value().visible)
                    {
                        return Qt::Checked;
                    }
                    else
                    {
                        return Qt::Unchecked;
                    }
                }
                i++;
            }
        }
        return Qt::Unchecked;
    }
    return QVariant();
}

int UPacketListModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return packetInfos_->size();
}

int UPacketListModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 3;
}

}//namespace uni