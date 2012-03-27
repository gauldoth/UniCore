/*! \file ULuaInterpreter.h
    \brief 一个Lua解释器。

    \author uni
    \date 2012-3-21
*/
#ifndef UNIUI_ULUA_INTERPRETER_H
#define UNIUI_ULUA_INTERPRETER_H

#include <QThread>
#include <QWidget>
#include <QVariant>

extern "C"
{

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
}

class QTextEdit;
class QPushButton;

namespace uni
{

//! 执行Lua脚本的线程。
class ULuaInterpreter_ExecRoutine : public QThread
{
    Q_OBJECT

public:
    explicit ULuaInterpreter_ExecRoutine(QObject *parent = 0);
    ~ULuaInterpreter_ExecRoutine();
    //! 开始执行脚本。
    void execScript(lua_State *L,const QString &script);
signals:
    void output(const QString &msg);
protected:
    void run();
private:
    lua_State *L_;
    QString script_;
};

//! Lua解释器控件。
/*!
    按下执行，会开启一个线程执行其中脚本。
    提供函数，用于向界面的lua环境中注册函数。
*/
class ULuaInterpreter : public QWidget
{
    Q_OBJECT

public:
    //! 构造一个Lua解释器窗口。
    /*!
        \param L lua状态，假如为0，会创建一个默认的lua状态。
        \param parent 父窗体。
    */
    explicit ULuaInterpreter(lua_State *L = 0,QWidget *parent = 0);
    virtual ~ULuaInterpreter();
    //! 注册新的Lua函数。
    /*!
        \param registerFunction 用于注册lua函数的函数。
        该函数用于注册函数到窗口所使用的Lua环境。
    */
    void registerLuaFunctions(lua_CFunction registerFunction);

    //! 这个函数用于替代lua默认的print函数。
    static int lua_print(lua_State *L);
    //! 保存lua环境到窗口指针的映射，print时根据这个找到正确的窗口。
    static QMap<lua_State *,ULuaInterpreter *> interpreters_;  

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    
public slots:
    //! 添加输出。
    void output(const QString &msg);
    //! 开启线程执行脚本。
    void execScript();
    //! 停止脚本执行。
    void stopScript();
    //! 脚本线程开始执行。
    void scriptStarted();
    //! 脚本线程执行结束。
    void scriptStopped();
private:
    //修改默认的print函数。
    void registerPrint();

    lua_State *L_;
    QTextEdit *scriptEdit_;
    QTextEdit *outputEdit_;
    QPushButton *execButton_;
    QPushButton *stopButton_;
    ULuaInterpreter_ExecRoutine *execRoutine_;
};

};

#endif//UNIUI_ULUA_INTERPRETER_H