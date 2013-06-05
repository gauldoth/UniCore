#include "UCommon.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <sstream>
#include <time.h>
#include <cassert>

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
    if(s.empty() || delim.empty())
    {
        //源字符串为空或者分隔符为空都无法进行分割.
        return results;
    }
    if(s.size() <= delim.size())
    {
        return results;
    }
    string::size_type currentPos = 0;
    string::size_type delimPos = 0;
    while(true)
    {
        delimPos = s.find(delim,currentPos);
        if(delimPos == string::npos)
        {
            //找到结尾都没找到分隔符,将剩余的字符串加入结果数组.
            results.push_back(s.substr(currentPos));
            break;
        }
        else
        {
            //找到分隔符
            assert(delimPos >= currentPos);
            if(delimPos > currentPos)
            {
                //分隔符位置如果等于当前位置,则子字符串为空.
                results.push_back(s.substr(currentPos,delimPos-currentPos));
            }
            currentPos = delimPos+delim.length();
            if(currentPos >= s.length())
            {
                //startPos已经超出字符串范围.
                break;
            }
        }

    }
    return results;
}

std::string trim(const std::string &s,const std::string &trimChars /*= " "*/)
{
    std::string result = s;
    // Remove char at the end.
    while(!result.empty())
    {
        if(trimChars.find(result.back()) != string::npos)
        {
            result.pop_back();
        }
        else
        {
            break;
        }
    }
    // Remove char at the beginning.
    while(!result.empty())
    {
        if(trimChars.find(result.front()) != string::npos)
        {
            result.erase(result.begin());
        }
        else
        {
            break;
        }
    }
    return result;
}

}//namespace uni