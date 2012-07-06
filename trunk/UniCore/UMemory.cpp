#include "UMemory.h"

namespace uni
{


std::string DumpString( int address )
{
    std::string a;
    try
    {
        a = (char *)address;
    }
    catch (...)
    {
    	
    }

    return a;
}

}//namespace uni