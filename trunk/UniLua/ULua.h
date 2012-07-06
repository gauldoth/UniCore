extern "C"
{
#include "../lua/lauxlib.h"
#include "../lua/lua.h"
#include "../lua/lualib.h"
}

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

inline void ULuaInit(lua_State *L)
{
    HMODULE mod = LoadLibraryW(L"unilua.dll");
    if(!mod)
    {
        return;
    }
    FARPROC proc = GetProcAddress(mod,"luaopen_unilua");
    if(proc)
    {
        ((lua_CFunction)proc)(L);
    }
}