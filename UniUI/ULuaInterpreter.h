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

public slots:
    void execScript();
private:
    lua_State *L_;
    QTextEdit *edit_;
    QPushButton *execButton_;
};

};

#endif//UNIUI_ULUA_INTERPRETER_H