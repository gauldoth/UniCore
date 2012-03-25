/*! \file ULuaInterpreter.h
    \brief 一个Lua解释器。

    \author uni
    \date 2012-3-21
*/
#ifndef UNIUI_ULUA_INTERPRETER_H
#define UNIUI_ULUA_INTERPRETER_H

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
    
    //! 添加输出。
    void output(const QString &msg);
public slots:
    void execScript();
    //! 停止脚本执行。
    void stopScript();
private:
    //修改默认的print函数。
    void registerPrint();

    lua_State *L_;
    QTextEdit *scriptEdit_;
    QTextEdit *outputEdit_;
    QPushButton *execButton_;
    QPushButton *stopButton_;
};

};

#endif//UNIUI_ULUA_INTERPRETER_H