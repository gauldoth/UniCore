#include "UCommon.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <sstream>
#include <time.h>

using namespace std;

namespace uni
{

//GetRandomName会从这个数组中取词.
static const char *g_wordsForGetRandomName[] = {
"Angel",
"Apple",
"Agent",
"Another",
"Bad",
"Bubble",
"Cloudy",
"Cobra",
"Dark",
"Devine",
"Ghost",
"Good",
"Kuso",
"Lucky",
"QX",
"Orange",
"Rider",
"Rock",
"Sadllly",
"Shadowy",
"Sunny",
"Warm",
"X",
"XX",
"Zero",
"Zebra"
};

std::string GetRandomRoleName(int minSize /*= 0*/,int maxSize /*= 0*/,int wordCount /*= 1*/,int randomSuffixCount /*= 0*/)
{
    int totalWordCount = (sizeof(g_wordsForGetRandomName) / sizeof(g_wordsForGetRandomName[0]));
    static bool flag = false;
    if(!flag)
    {
        srand((unsigned int)time(0));
        flag = true;
    }
    
    //生成wordCount个随机的词语.
    vector<string> words;
    int currentNameLength = 0;  
    for(int i = 0; i < wordCount; i++)
    {
        words.push_back(g_wordsForGetRandomName[rand()%totalWordCount]);
        currentNameLength += words[i].length();
    }

    currentNameLength += randomSuffixCount;
    if(currentNameLength < minSize || currentNameLength > maxSize)
    {
        //长度不合适,重新生成.
        return GetRandomRoleName(minSize,maxSize,wordCount,randomSuffixCount);
    }

    //生成随机的数字后缀.
    if(randomSuffixCount > 255)
    {
        randomSuffixCount = 255;
    }
    char randomSuffix[256] = "";

    for(int i = 0; i < randomSuffixCount; i++)
    {
        randomSuffix[i] = 48+rand()%10;
    }

    string result;
    for(int i = 0; i < words.size(); i++)
    {
        result += words[i];
    }
    result += randomSuffix;

    return result;
}

vector<string> split( const string &s,const string &delim /*= " "*/ )
{
    vector<string> results;
    string::size_type startPos = 0;
    string::size_type delimPos = 0;
    while(true)
    {
        delimPos = s.find(delim,startPos);
        if(delimPos == startPos)
        {
            //开始位置就找到分割符,或者遇到连续的分割符,子字符串长度为0,不保存.
        }
        else if(delimPos == string::npos)
        {
            //找到结尾了.
            results.push_back(s.substr(startPos));
            break;
        }
        else
        {
            results.push_back(s.substr(startPos,delimPos-startPos));
        }
        startPos = delimPos+delim.length();
        if(startPos >= s.length())
        {
            //startPos已经超出字符串范围.
            break;
        }
    }
    return results;
}

}//namespace uni