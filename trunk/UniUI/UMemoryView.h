/*! \file UMemoryView.h
    \brief 内存视图。
    用于显示内存数据。

    \author uni
    \date 2012-1-17
*/
#ifndef UNIUI_UMEMORY_VIEW_H
#define UNIUI_UMEMORY_VIEW_H

#include <QWidget>

class QLabel;
class QLineEdit;

namespace uni
{

class UMemoryModel;
class UMemoryTable;

//! 内存视图。
class UMemoryView : public QWidget
{
    Q_OBJECT

public:
    explicit UMemoryView(QWidget *parent = 0);
    virtual ~UMemoryView();

public slots:
    //! 设置当前地址。
    void setAddress(int address);

private:
    void setupUI();
    QLabel *addressLabel_;
    QLineEdit *addressEdit_;
    UMemoryTable *memoryTable_;  //!< 显示内存内容的表格。
    UMemoryModel *memoryModel_;  //!< 显示内存内容的表格用到的数据模型。

};

}//namespace uni

#endif//UNIUI_UMEMORY_VIEW_H