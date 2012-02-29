#include "UPacketMonitorModel.h"

#include "../UniCore/UMemory.h"
#include "../UniCore/ULog.h"

namespace uni
{

UPacketMonitorModel::UPacketMonitorModel(QList<UPacketView::PacketData> *packetDatas,QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,packetDatas_(packetDatas)
{
    font_ = new QFont();
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
    return 5;
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

        if(column == 0)
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
        else if(column == 1)
        {
            return QString("0x%1").arg(packetData.id,8,16,QChar('0'));
        }
        else if(column == 2)
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
        else if(column == 3)
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
        else if(column == 4)
        {
            return packetSize;
        }
    }
    else if(role == Qt::SizeHintRole)
    {
         QSize size;
         int column = index.column();
         int row = index.row();
         int packetSize = packetDatas_->at(row).content.size();
         int contentRowCount = (packetSize-1)/16+1;
         switch(column)
         {
         case 0:
             {
                 size.setWidth(40);
                 size.setHeight(30);
                 break;
             }
         case 1:
             {
                 size.setWidth(10);
                 size.setHeight(30);
                 break;
             }
         case 2:
             {
                 size.setWidth(320);
                 size.setHeight(contentRowCount*12);
                 break;
             }
         case 3:
             {
                 size.setWidth(220);
                 size.setHeight(50);
                 break;
             }
         default:
             {
                 size.setWidth(150);
                 size.setHeight(50);
                 break;
             }
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
    UTRACE("性能")<<"enter";
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    packetDatas_->push_back(data);
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
        case 4:
            {
                return tr("Len");
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