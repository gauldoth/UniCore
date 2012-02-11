#include "UPacketMonitorModel.h"

#include "../UniCore/UMemory.h"

namespace uni
{

UPacketMonitorModel::UPacketMonitorModel(QList<PacketInfo> *packetInfos,QList<Packet> *packets,QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,packetInfos_(packetInfos)
,packets_(packets)
{
    
}

UPacketMonitorModel::~UPacketMonitorModel()
{

}

int UPacketMonitorModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return packets_->size();
}

int UPacketMonitorModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 4;
}

QVariant UPacketMonitorModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    Q_ASSERT(index.isValid());
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int column = index.column();
        const char *packet = packets_->at(index.row()).content.constData();
        int packetSize = packets_->at(index.row()).content.size();
        
        if(column == 0)
        {
            if(packets_->at(index.row()).type == SendType)
            {
                return tr("Send");
            }
            else if(packets_->at(index.row()).type == RecvType)
            {
                return tr("Recv");
            }
            else
            {
                return tr("Unknown");
            }
        }
        else if(column == 1)
        {
            return QString("%1").arg(packets_->at(index.row()).id,8,16,QChar('0'));
        }
        else if(column == 2)
        {
            QString datas;

            for(int i = 0; i < packetSize; i++)
            {
                if(i%16 == 0)
                {
                    datas += "|";
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
        else if(column == 3)
        {
            QString text;

            for(int i = 0; i < packetSize; i++)
            {
                if(packet[i] == '\r')
                {
                    text += "\r";
                }
                else if(packet[i] == '\d')
                {
                    text += "\d";
                }
                else if(packet[i] == '\t')
                {
                    text += "\t";
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
    }
    return QVariant();
}

void UPacketMonitorModel::addPacket(PacketType type,const char * packet, int packetSize)
{
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    Packet data;
    QByteArray content(packet,packetSize);
    data.content = content;
    data.id = GetAt<unsigned short>(packet,0);
    data.type = type;
    packets_->push_back(data);
    endInsertRows();
}

QVariant UPacketMonitorModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            {
                return tr("Type");
                break;
            }
        case 1:
            {
                return tr("Packet ID");
                break;
            }
        case 2:
            {
                return tr("Packet Content");
                break;
            }
        case 3:
            {
                return tr("Text");
                break;
            }
        default:
            {
                
            }
        }
    }
    return QVariant();
}

}//namespace uni