#include "ULua.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "ULog.h"
#include "UMemory.h"


namespace uni
{

static int lua_get_at(lua_State *L)
{
    int argCount = lua_gettop(L);
    if(argCount < 2)
    {
        lua_pushstring(L,"insufficient arguments");
        lua_error(L);
    }
    //至少有2个参数。
    int base = luaL_checkint(L,1);
    int *offsets = new int[argCount-1];
    for(int i = 2; i <= argCount; i++)
    {
        offsets[i-2] = luaL_checkint(L,i);
    }
    int result = 0;
    try
    {
        result = GetAt<int>(base,offsets,argCount-1);
        lua_pushinteger(L,result);
    }
    catch (...)
    {
    	lua_pushnil(L);
    }
    return 1;
}

static int lua_sleep(lua_State *L)
{
    int time = luaL_checkint(L,1);
    Sleep(time);
    return 0;
}

static int lua_print(lua_State *L)
{
    ULog log(ULog::DebugType,__FILE__,__LINE__,__FUNCTION__);
    log<<ULogSetName("脚本");
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        switch(lua_type(L,i))
        {
        case LUA_TNIL:
            {
                log<<"nil ";
                break;
            }
        case LUA_TBOOLEAN:
            {
                if(lua_toboolean(L,i) == 0)
                {
                    log<<"false ";
                }
                else if(lua_toboolean(L,i) == 1)
                {
                    log<<"true ";
                }
                else
                {
                    assert(!"lua_toboolean返回值不为0也不为1。");
                }
                break;
            }
        case LUA_TLIGHTUSERDATA:
            {
                void *userData = lua_touserdata(L,i);
                log<<"light userdata:"<<userData;
                break;
            }
        case LUA_TNUMBER:
        case LUA_TSTRING:
            {
                log<<lua_tostring(L,i)<<" ";
                break;
            }
        case LUA_TTABLE:
            {
                const void *tablePtr = lua_topointer(L,i);
                log<<"table:"<<tablePtr<<" ";
                break;
            }
        case LUA_TFUNCTION:
            {
                const void *function = lua_topointer(L,i);
                log<<"function:"<<function<<" ";
                break;
            }
        case LUA_TUSERDATA:
            {
                void *userData = lua_touserdata(L,i);
                log<<"userdata:"<<userData<<" ";
                break;
            }
        case LUA_TTHREAD:
            {
                const void *threadPtr = lua_topointer(L,i);
                log<<"thread:"<<threadPtr<<" ";
                break;
            }
        default:
            {
                assert(!"未知的lua类型。");
            }
        }

    }
    return 0;
}

static const struct luaL_reg g_luaFunctions[] = {
    {"sleep",lua_sleep},
    {"get_at", lua_get_at},
    {"print",lua_print},
    {NULL, NULL}  /* sentinel */
};

int RegisterCommonLuaFunctions( lua_State *L )
{
    for(int i = 0; g_luaFunctions[i].func; i++)
    {
        lua_pushcfunction(L,g_luaFunctions[i].func);
        lua_setglobal(L,g_luaFunctions[i].name);
    }
    return 0;
}

}//namespace uni