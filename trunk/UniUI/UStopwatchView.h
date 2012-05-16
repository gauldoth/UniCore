/*! \file UStopwatchView.h
    \brief 查看计时器数值的窗口。
    该窗口用于统计执行时间。

    \author uni
    \date 2012-5-16
*/
#ifndef UNIUI_USTOPWATCH_VIEW_H
#define UNIUI_USTOPWATCH_VIEW_H

#include <QTableView>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UStopwatchView : public QTableView
{
    Q_OBJECT

public:
    explicit UStopwatchView(QWidget *parent = 0);
    ~UStopwatchView();
};

}//namespace uni

#endif//UNIUI_USTOPWATCH_VIEW_H