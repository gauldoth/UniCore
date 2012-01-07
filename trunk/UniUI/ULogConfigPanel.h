#ifndef UNIUI_ULOG_CONFIG_PANEL_H
#define UNIUI_ULOG_CONFIG_PANEL_H

#include <QWidget>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "AutoLink.h"

class QCheckBox;
class QComboBox;
class QListView;
class QModelIndex;

namespace uni
{

class ULogConfigPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ULogConfigPanel(QWidget *parent);
    virtual ~ULogConfigPanel();
public slots:
    void updateNames();
    void updateConfig(const QModelIndex &index);
private:
    void setupUI();
    QListView *logNameList_;  //!< 用于显示存在的日志名。
    QCheckBox *isOutputCheckBox_;  //!< 当前日志是否输出。
    QComboBox *logLevelComboBox_;  //!< 日志等级。
};

}//namespace uni

#endif//UNIUI_ULOG_CONFIG_PANEL_H