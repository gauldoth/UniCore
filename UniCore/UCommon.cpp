#include "UCommon.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

namespace uni
{

bool WaitFor( bool condition,int timeout )
{
    if(condition)
    {
        return true;
    }
    for(int i = 0; i < timeout/50; i++)
    {
        Sleep(50);
        if(condition)
        {
            return true;
        }
    }
    return false;
}

}//namespace uni