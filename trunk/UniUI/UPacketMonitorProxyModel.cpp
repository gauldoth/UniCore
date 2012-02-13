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
    //sourceModel是UPacketMonitorModel。
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);

    if(index0.data().toString() == "Send")
    {
        int id = index1.data().toString().toInt(0,16);
        //查看是否在显示ID列表中。
        if(filters_[SendType].contains(id))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(index0.data().toString() == "Recv")
    {
        int id = index1.data().toInt();
        if(filters_[RecvType].contains(id))
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

void UPacketMonitorProxyModel::setFilters( QMap<PacketType,QSet<int> > filters )
{
    filters_ = filters;
    invalidateFilter();
}


}//namespace uni