/*! \file UMemoryView.h
    \brief 内存视图。
    用于显示内存数据。

    \author uni
    \date 2012-1-17
*/
#ifndef UNIUI_UMEMORY_VIEW_H
#define UNIUI_UMEMORY_VIEW_H

#include <QDialog>
#include <QWidget>

extern "C"
{
#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
}

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QTextEdit;

namespace uni
{

class UMemoryModel;
class UMemoryTable;

class AddColumnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddColumnDialog(QWidget *parent);
    virtual ~AddColumnDialog();
    QString title() const;
    QString calculus() const;
    void clearExistContents();
private:
    QPushButton *setButton_;
    QPushButton *cancelButton_;
    QDialogButtonBox *buttonBox_;
    QLineEdit *titleEdit_;
    QTextEdit *calculusEdit_;
};

//! 内存视图。
class UMemoryView : public QWidget
{
    Q_OBJECT

public:
    //! 构造函数。
    /*!
        \param parent 父窗口句柄。
        \param state Lua状态。假如为0，则会在内部自己创建一个lua_State。
        
        UMemoryView可以在某个指定的Lua状态下工作，这时UMemoryView中自定义的内存
        脚本可以使用该Lua状态下的全局的内容。
    */
    explicit UMemoryView(QWidget *parent = 0,lua_State *state = 0);
    virtual ~UMemoryView();

public slots:
    //! 设置当前地址。
    void setAddress(int address);
    //! 添加新列。
    /*!
        \param title 列的标题。
        \param calculus 该列数据的计算脚本。
    */
    void addColumn(const QString &title,const QString &calculus);
    //! addressEdit内容改变。
    void addressEditChanged(const QString &text);
    //! 显示添加列对话框。
    void showAddColumnDialog();
    //! 添加列。
    void addColumnDialogAccepted();
protected:
    //! 关闭事件。
    /*!
        关闭的时候保存列信息到当前进程所在目录。
    */
    void closeEvent(QCloseEvent *event);
private:
    //! 读取列信息。
    /*!
        从当前进程所在目录查找UMemoryView_ColumnInfos.lua。
    */
    void loadColumnInfos();
    //! 保存列信息。
    void saveColumnInfos();
    void setupUI();
    QLabel *addressLabel_;
    QLineEdit *addressEdit_;
    UMemoryTable *memoryTable_;  //!< 显示内存内容的表格。
    UMemoryModel *memoryModel_;  //!< 显示内存内容的表格用到的数据模型。
    AddColumnDialog *addColumnDialog_;  //!< 添加列的对话框。
};

}//namespace uni

#endif//UNIUI_UMEMORY_VIEW_H