#include "UPacketMonitorModel.h"

namespace uni
{

UPacketMonitorModel::UPacketMonitorModel( QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
{

}

UPacketMonitorModel::~UPacketMonitorModel()
{

}

int UPacketMonitorModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 0;
}

int UPacketMonitorModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 0;
}

QVariant UPacketMonitorModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    return QVariant();
}

}//namespace uni