#include "ULuaInterpreter.h"

#include "../UniCore/ULua.h"
#include "../UniCore/ULog.h"

namespace uni
{

ULuaInterpreter::ULuaInterpreter(lua_State *L /*= 0*/, QWidget *parent /*= 0*/)
:QWidget(parent)
,L_(L)
{
    if(!L_)
    {
        L_ = lua_open();
        if(!L_)
        {
            UERROR("Lua解释器")<<"无法创建Lua状态，内存不足。";
            return;
        }
        lua_gc(L_,LUA_GCSTOP,0);
        luaL_openlibs(L_);
        RegisterCommonLuaFunctions(L_);
        lua_gc(L_,LUA_GCRESTART,0);
    }
}

ULuaInterpreter::~ULuaInterpreter()
{
    lua_close(L_);
}

}//namespace uni