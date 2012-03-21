#include "ULuaInterpreter.h"

namespace uni
{

ULuaInterpreter::ULuaInterpreter(lua_State *L /*= 0*/, QWidget *parent /*= 0*/)
:QWidget(parent)
,L_(L)
{
    if(!L_)
    {
        
    }
}

ULuaInterpreter::~ULuaInterpreter()
{

}

}//namespace uni