#ifndef UNIUI_UPROCESSVIEW_H
#define UNIUI_UPROCESSVIEW_H

#define AUTO_LINK_LIB_NAME "UniUI"
#include "AutoLink.h"

#include <QTableView>

namespace uni
{

//! 显示进程信息。
class UProcessView : public QTableView
{
    Q_OBJECT

public:
    explicit UProcessView(QWidget *parent = 0);
    virtual ~UProcessView();

signals:
    //! 进程被结束。
    void processTerminated(unsigned int pid);
    //! 是否有进程被选择。
    void processSelected(bool selected);

protected slots:
    bool killProcess();

private:
    Q_DISABLE_COPY(UProcessView);
    void setupActions();
    QAction *killProcessAction_;
    QAction *selectColumnAction_;
    void setupConnections();

};

}//namespace uni

#endif//UNIUI_UPROCESSVIEW_H