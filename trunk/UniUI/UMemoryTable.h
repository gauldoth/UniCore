/*! \file UMemoryTable.h
    \brief 内存表格。
    用于显示内存数据的表格。
    
    \author uni
    \date 2012-1-17
*/
#ifndef UNIUI_UMEMORY_TABLE_H
#define UNIUI_UMEMORY_TABLE_H

#include <QTableView>

namespace uni
{

//! 显示内存的视图。
/*!
    该视图可以增加
*/
class UMemoryTable : public QTableView
{
    Q_OBJECT

public:
    explicit UMemoryTable(QWidget *parent = 0);
    virtual ~UMemoryTable();

public slots:

};

}//namespace uni

#endif//UNIUI_UMEMORY_TABLE_H