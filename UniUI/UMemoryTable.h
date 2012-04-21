/*! \file UMemoryTable.h
    \brief 内存表格。
    用于显示内存数据的表格。
    
    \author uni
    \date 2012-1-17
*/
#ifndef UNIUI_UMEMORY_TABLE_H
#define UNIUI_UMEMORY_TABLE_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QTableView>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QTextEdit;

namespace uni
{

//! 修改列信息用的对话框。
class EditColumnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditColumnDialog(QWidget *parent = 0);
    virtual ~EditColumnDialog();
    QString columnTitle() const;
    QString getDataScript() const;
    QString setDataScript() const;
    QString dataColorScript() const;
    void setColumnTitle(const QString &title);
    void getDataScript(const QString &script);
    void setDataScript(const QString &script);
    void dataColorScript(const QString &script);
    void setColumn(int column) {column_ = column;}
    int column() const {return column_;}
    void clearExistContents();
private:
    QPushButton *setButton_;
    QPushButton *cancelButton_;
    QDialogButtonBox *buttonBox_;
    QLineEdit *titleEdit_;
    QTextEdit *getDataScriptEdit_;
    QTextEdit *setDataScriptEdit_;
    QTextEdit *dataColorScriptEdit_;
    int column_;
};


//! 显示内存的视图。
/*!
    
*/
class UMemoryTable : public QTableView
{
    Q_OBJECT

public:
    explicit UMemoryTable(QWidget *parent = 0);
    virtual ~UMemoryTable();

public slots:
    //! 添加列对话框被接受。
    void editColumnDialogAccepted();
    //! 显示编辑列对话框。
    void showEditColumnDialog();
    //! 显示添加列对话框。
    void showAddColumnDialog();
    //! 删除指定列。
    void deleteColumn();
    //! 响应gotoAddressAction_的函数。
    void gotoAddress();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
signals:
    void changeAddress(int address);
private:
    EditColumnDialog *editColumnDialog_;
    QMenu *headerMenu_;
    QAction *addColumnAction_;
    QAction *editColumnAction_;
    QAction *deleteColumnAction_;
    QAction *gotoAddressAction_;
    int lastClickedSection_;
};

}//namespace uni

#endif//UNIUI_UMEMORY_TABLE_H