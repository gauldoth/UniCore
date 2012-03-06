#include "UPacketMonitorModel.h"

#include <QTime>

#include "../UniCore/UMemory.h"
#include "../UniCore/ULog.h"

namespace uni
{

UPacketMonitorModel::UPacketMonitorModel(QList<UPacketView::PacketData> *packetDatas,QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,packetDatas_(packetDatas)
{
    font_ = new QFont();
    columnNames_<<tr("Time")<<tr("Type")<<tr("PacketID")<<tr("PacketContent")
        <<tr("Text")<<tr("Len");
}

UPacketMonitorModel::~UPacketMonitorModel()
{

}

int UPacketMonitorModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return packetDatas_->size();
}

int UPacketMonitorModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return columnNames_.size();
}

QVariant UPacketMonitorModel::data( const QModelIndex &index,int role /*= Qt::DisplayRole*/ ) const
{
//     if(role == Qt::BackgroundRole)
//     {
//     UTRACE("性能")<<"row:"<<index.row()<<" column:"<<index.column()<<" role:"<<role;
//     }
    Q_ASSERT(index.isValid());
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int column = index.column();
        int row = index.row();
        const char *packet = packetDatas_->at(index.row()).content.constData();
        int packetSize = packetDatas_->at(index.row()).content.size();
        UPacketView::PacketData packetData = packetDatas_->at(row);

        if(columnNames_[column] == tr("Type"))
        {
            if(packetData.type == UPacketView::SendType)
            {
                return tr("Send");
            }
            else if(packetData.type == UPacketView::RecvType)
            {
                return tr("Recv");
            }
            else
            {
                return tr("Unknown");
            }
        }
        else if(columnNames_[column] == tr("PacketID"))
        {
            return QString("0x%1").arg(packetData.id,4,16,QChar('0'));
        }
        else if(columnNames_[column] == tr("PacketContent"))
        {
            QString datas;

            for(int i = 0; i < packetSize; i++)
            {
                if(i%16 == 0)
                {
                }
                else if(i%4 == 0)
                {
                    datas += "|";
                }
                datas += QString("%1 ").arg(QString::number((unsigned char)packet[i],16),2,QChar('0'));
                if(i%16 == 15 /*|| i == packetSize-1*/)
                {
                    datas += "\r\n";
                }
            }
            return datas;
        }
        else if(columnNames_[column] == tr("Text"))
        {
            QString text;

            for(int i = 0; i < packetSize; i++)
            {
                if(packet[i] == '\0')
                {
                    text += "\\0";
                }
                else if(packet[i] == '\r')
                {
                    text += "\\r";
                }
                else if(packet[i] == '\n')
                {
                    text += "\\n";
                }
                else if(packet[i] == '\t')
                {
                    text += "\\t";
                }
                else if(packet[i] < 0)
                {
                    text += ". ";
                }
                else
                {
                    text += QString("%1 ").arg(packet[i],1,QChar(' '));
                }
                if(i%16 == 15 /*|| i == packetSize-1*/)
                {
                    text += "\r\n";
                }
            }
            return text;
        }
        else if(columnNames_[column] == tr("Len"))
        {
            return packetSize;
        }
        else if(columnNames_[column] == tr("Time"))
        {
            return packetData.time;
        }

    }
    else if(role == Qt::SizeHintRole)
    {
         QSize size;
         int column = index.column();
         int row = index.row();
         int packetSize = packetDatas_->at(row).content.size();
         int contentRowCount = (packetSize-1)/16+1;
         if(columnNames_[column] == tr("Type"))
         {
             size.setWidth(40);
             size.setHeight(12);
         }
         else if(columnNames_[column] == tr("PacketID"))
         {
             size.setWidth(40);
             size.setHeight(12);
         }
         else if(columnNames_[column] == tr("PacketContent"))
         {
             size.setWidth(320);
             size.setHeight(contentRowCount*12);
         }
         else if(columnNames_[column] == tr("Text"))
         {
             size.setWidth(220);
             size.setHeight(contentRowCount*12);
         }
         else if(columnNames_[column] == tr("Len"))
         {
             size.setWidth(50);
             size.setHeight(12);
         }
         else if(columnNames_[column] == tr("Time"))
         {
             size.setWidth(65);
             size.setHeight(12);
         }
         return size;
    }
    else if(role == Qt::FontRole)
    {
        return (*font_);
    }
    return QVariant();
}

void UPacketMonitorModel::addPacketData(UPacketView::PacketData data)
{
    //UTRACE("性能")<<"enter";
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    packetDatas_->push_back(data);
    endInsertRows();
}

QVariant UPacketMonitorModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return columnNames_.at(section);
    }
    return QVariant();
}

bool UPacketMonitorModel::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex()*/ )
{
    beginRemoveRows(QModelIndex(),row,row+count-1);
    for(int i = row; i < row+count; i++)
    {
        packetDatas_->removeAt(row);
    }
    endRemoveRows();
    return true;
}

}//namespace uni