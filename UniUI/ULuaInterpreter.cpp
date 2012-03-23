#include "ULuaInterpreter.h"

#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>

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

    edit_ = new QTextEdit(this);
    execButton_ = new QPushButton(tr("exec"),this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(edit_);
    layout->addWidget(execButton_);
    setLayout(layout);

    connect(execButton_,SIGNAL(clicked()),this,SLOT(execScript()));
}

ULuaInterpreter::~ULuaInterpreter()
{
    lua_close(L_);
}

void ULuaInterpreter::registerLuaFunctions( lua_CFunction registerFunction )
{
    registerFunction(L_);
}

void ULuaInterpreter::execScript()
{
    int result = luaL_loadstring(L_,edit_->toPlainText().toAscii());
    if(result == LUA_ERRSYNTAX)
    {
        UERROR("Lua解释器")<<"语法错误。";
        return;
    }
    else if(result == LUA_ERRMEM)
    {
        UERROR("Lua解释器")<<"内存不足。";
        return;
    }

    assert(result == 0);

    //lua_pcall(L_,0,0,traceback);
}

}//namespace uni