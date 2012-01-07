#ifndef UNIUI_UMEMORY_VIEW_H
#define UNIUI_UMEMORY_VIEW_H

#include <QAbstractItemView>

//! 内存视图。
class UMemoryView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit UMemoryView(QWidget *parent = 0);
    virtual ~UMemoryView();

    void setAddress(int address);
};

#endif//UNIUI_UMEMORY_VIEW_H