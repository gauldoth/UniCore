/*! \file UPacketDisplayList.h
    UPacketDisplayList用于保存封包视图的显示方案。
    允许快速在几个不同的显示方案间切换。

    \author uni
    \date 2012-3-21
*/
#ifndef UNIUI_UPACKET_DISPLAY_LIST_H
#define UNIUI_UPACKET_DISPLAY_LIST_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QWidget>
#include <QLineEdit>

#include "UPacketView.h"

class QListView;
class QPushButton;
class QStringListModel;

namespace uni
{

class UPacketDisplayList_NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UPacketDisplayList_NewDialog(QWidget *parent = 0);
    ~UPacketDisplayList_NewDialog();
    QString schemeName() const {return schemeNameEdit_->text();}
public slots:

private:
    QLineEdit *schemeNameEdit_;
};

class UPacketDisplayList : public QWidget
{
    Q_OBJECT

public:
    explicit UPacketDisplayList(QMap<QString,UPacketView::DisplayScheme> *displaySchemes,
        QWidget *parent = 0);
    virtual ~UPacketDisplayList();
    //! 设置显示方案。
    /*!
        \param name 显示方案的名称。
        \param scheme 显示方案。
    */
    void setDisplayScheme(const QString &name,UPacketView::DisplayScheme scheme);
    void deleteDisplayScheme(const QString &name);
public slots:
    void deleteSelectedScheme();
    void newScheme();
    void saveScheme();
    void newSchemeDialogAccepted();
    //! 双击某个显示方案后，加载该显示方案。
    void loadScheme( const QModelIndex &index );
signals:
    void saveSchemeRequested(const QString &schemeName);
    void loadSchemeRequested(const QString &schemeName);

private:
    QMap<QString,UPacketView::DisplayScheme> *displaySchemes_;
    UPacketDisplayList_NewDialog *newSchemeDialog_;
    QListView *displayList_;
    QStringListModel *model_;
    QPushButton *deleteButton_;
    QPushButton *newButton_;
    QPushButton *saveButton_;
};

}//namespace uni

#endif//UNIUI_UPACKET_DISPLAY_LIST_H