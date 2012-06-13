#include "UCommon.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <time.h>

namespace uni
{

static const char *g_prefix[] = {
"Cloudy",
"Sadllly",
"Bubble",
"QX",
"Cobra",
"Bad",
"Good",
"Lucky",
"Warm",
"Shadowy",
0
};

static const char *g_suffix[] = {
"ManX",
"Sharker",
"Bomb",
"Code",
"Strike",
"Applee",
"Ninnja",
"Cobara",
"Conraaa",
"Cannoner",
"Summoner",
"Beatler",
0
};

std::string GetRandomRoleName(int minSize /*= 0*/,int maxSize /*= 0*/)
{
    int prefixCount = 0;
    int suffixCount = 0;
    for(int i = 0; g_prefix[i]; i++)
    {
        prefixCount++;
    }
    for(int i = 0; g_suffix[i]; i++)
    {
        suffixCount++;
    }
    static bool flag = false;
    if(!flag)
    {
        srand((unsigned int)time(0));
        flag = true;
    }
    const char *prefix = g_prefix[rand()%prefixCount];
    const char *suffix = g_suffix[rand()%suffixCount];
    std::string result = prefix;
    result += suffix;
    if(minSize)
    {
        if((int)result.size() < minSize)
        {   
            return GetRandomRoleName(minSize,maxSize);
        }
    }
    if(maxSize)
    {
        if((int)result.size() > maxSize)
        {
            return GetRandomRoleName(minSize,maxSize);
        }
    }
    return result;
}

}//namespace uni