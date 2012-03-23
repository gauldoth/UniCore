/*! \file ULua.h
    \brief 提供基础的Lua函数。

    \author uni
    \date 2012-3-23
*/
#ifndef UNICORE_ULUA_H
#define UNICORE_ULUA_H

extern "C"
{
#include "../lua/lauxlib.h"
#include "../lua/lua.h"
#include "../lua/lualib.h"
}

namespace uni
{

//! 注册常用的Lua函数。
void RegisterCommonLuaFunctions(lua_State *L);

}//namespace uni

#endif//UNICORE_ULUA_H