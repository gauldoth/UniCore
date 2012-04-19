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

//! 添加列用的对话框。
class AddColumnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddColumnDialog(QWidget *parent);
    virtual ~AddColumnDialog();
    QString title() const;
    QString getDataScript() const;
    QString setDataScript() const;
    QString dataColorScript() const;
    void clearExistContents();
private:
    QPushButton *setButton_;
    QPushButton *cancelButton_;
    QDialogButtonBox *buttonBox_;
    QLineEdit *titleEdit_;
    QTextEdit *getDataScriptEdit_;
    QTextEdit *setDataScriptEdit_;
    QTextEdit *dataColorScriptEdit_;
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
    //! 显示添加列对话框。
    void showAddColumnDialog();
    //! 显示编辑列对话框。
    void showEditColumnDialog();
    //! 显示
private:
};

}//namespace uni

#endif//UNIUI_UMEMORY_TABLE_H