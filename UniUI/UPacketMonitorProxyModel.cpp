#include "UPacketMonitorProxyModel.h"

#include "../UniCore/ULog.h"

namespace uni
{

UPacketMonitorProxyModel::UPacketMonitorProxyModel( QObject *parent /*= 0*/ )
{
    
}

UPacketMonitorProxyModel::~UPacketMonitorProxyModel()
{

}

bool UPacketMonitorProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
    //UTRACE("性能")<<"enter";
    //sourceModel是UPacketMonitorModel。
    UStopwatch a(1);
    static bool init = false;
    static int columnForType = 0;
    static int columnForData = 0;
    if(!init)
    {
        //假设此时sourceModel已经被设置。
        for(int i = 0; i < sourceModel()->columnCount(); i++)
        {
            QString columnName = sourceModel()->headerData(i,Qt::Horizontal).toString();
            if(columnName == tr("Type"))
            {
                columnForType = i;
            }
            else if(columnName == tr("ID"))
            {
                columnForData = i;
            }
        }
        init = true;
    }
    QModelIndex indexForType = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex indexForData = sourceModel()->index(sourceRow, 2, sourceParent);

    if(indexForType.data().toString() == "Send")
    {
        int id = indexForData.data().toString().toInt(0,16);
        //查看是否在显示ID列表中。
        if(filters_[UPacketView::SendType].contains(id))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(indexForType.data().toString() == "Recv")
    {
        int id = indexForData.data().toString().toInt(0,16);
        if(filters_[UPacketView::RecvType].contains(id))
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
        return false;
    }
}

void UPacketMonitorProxyModel::setFilters( QMap<UPacketView::PacketType,QSet<int> > filters )
{
    filters_ = filters;
    invalidateFilter();
}

}//namespace uni