#include "UMemoryModel.h"

namespace uni
{

UMemoryModel::UMemoryModel( QObject *parent /*= 0*/ )
:QAbstractTableModel(parent)
,baseAddress_(0)
{
    
}

UMemoryModel::~UMemoryModel()
{

}

int UMemoryModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return 
}

int UMemoryModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    return columns_.size();
}

QVariant UMemoryModel::data( const QModelIndex &index, int role /*= Qt::DisplayRole*/ ) const
{
    
}


}//namespace uni