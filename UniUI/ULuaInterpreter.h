/*! \file ULuaInterpreter.h
    \brief 一个Lua解释器。

    \author uni
    \date 2012-3-21
*/
#ifndef UNIUI_ULUA_INTERPRETER_H
#define UNIUI_ULUA_INTERPRETER_H

#include <QWidget>

extern "C"
{
#include "../lua/lua.h"
}

namespace uni
{

//! Lua解释器控件。
/*!
    按下执行，会开启一个线程执行其中脚本。
    提供函数，用于向界面的lua环境中注册函数。
*/
class ULuaInterpreter : QWidget
{
    Q_OBJECT

public:
    explicit ULuaInterpreter(lua_State *L = 0,QWidget *parent = 0);
    virtual ~ULuaInterpreter();
private:
    lua_State *L_;
};

};

#endif//UNIUI_ULUA_INTERPRETER_H