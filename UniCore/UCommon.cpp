#include "UCommon.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <sstream>
#include <time.h>
#include <cassert>
#include <algorithm>
#include <locale>

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
    if(s.size() < delim.size())
    {
		//源字符串长度小于分隔符长度时也无法分割.
        return results;
    }
    string::size_type currentPos = 0;
    string::size_type delimPos = 0;
    while(true)
    {
        delimPos = s.find(delim,currentPos);
        if(delimPos == string::npos)
        {
            //从当前位置开始找不到分隔符,将剩余的字符串加入结果数组.
            results.push_back(s.substr(currentPos));
            break;
        }
        else
        {
            //找到分隔符
            assert(delimPos >= currentPos);
            if(delimPos >= currentPos)
            {
                //分隔符位置如果等于当前位置,则保存一个空字符串.
                results.push_back(s.substr(currentPos,delimPos-currentPos));
            }
            currentPos = delimPos+delim.length();  //当前位置指向分隔符后面一个位置.
        }

    }
    return results;
}

std::vector<std::wstring> split( const std::wstring &s,const std::wstring &delim /*= L" "*/ )
{

	vector<wstring> results;
	if(s.empty() || delim.empty())
	{
		//源字符串为空或者分隔符为空都无法进行分割.
		return results;
	}
	if(s.size() < delim.size())
	{
		//源字符串长度小于分隔符长度时也无法分割.
		return results;
	}
	wstring::size_type currentPos = 0;
	wstring::size_type delimPos = 0;
	while(true)
	{
		delimPos = s.find(delim,currentPos);
		if(delimPos == wstring::npos)
		{
			//从当前位置开始找不到分隔符,将剩余的字符串加入结果数组.
			results.push_back(s.substr(currentPos));
			break;
		}
		else
		{
			//找到分隔符
			assert(delimPos >= currentPos);
			if(delimPos >= currentPos)
			{
                //分隔符位置如果等于当前位置,则保存一个空字符串.
				results.push_back(s.substr(currentPos,delimPos-currentPos));
			}
			currentPos = delimPos+delim.length();  //当前位置指向分隔符后面一个位置.
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

std::wstring trim( const std::wstring &s,const std::wstring &trimChars /*= L" "*/ )
{
    std::wstring result = s;
    // Remove char at the end.
    while(!result.empty())
    {
        if(trimChars.find(result.back()) != wstring::npos)
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
        if(trimChars.find(result.front()) != wstring::npos)
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

std::wstring rtrim( const std::wstring &s,const std::wstring &trimChars /*= L" "*/ )
{
  std::wstring result = s;
  // Remove char at the end.
  while(!result.empty())
  {
    if(trimChars.find(result.back()) != wstring::npos)
    {
      result.pop_back();
    }
    else
    {
      break;
    }
  }
  return result;
}

std::wstring pad( const std::wstring &s,const std::wstring &padString )
{
  assert(!"not implemented.");
  return L"";
}

std::wstring rpad( const std::wstring &s,const std::wstring &padString )
{
  size_t pos = s.rfind(padString);
  if(pos != std::wstring::npos
    && pos + padString.size() == s.size())
  {
    // not found or not ends with.
    return s;
  }
  else
  {
    return s+padString;
  }
}

std::string join( const std::vector<std::string> &stringsToJoin,const std::string &delim )
{
    std::string result;
    for(int i = 0; i < stringsToJoin.size(); i++)
    {
        result += stringsToJoin[i];
        if(i != stringsToJoin.size() - 1)
        {
            result += delim;
        }
    }
    return result;
}

std::wstring join( const std::vector<std::wstring> &stringsToJoin,const std::wstring &delim )
{
    std::wstring result;
    for(int i = 0; i < stringsToJoin.size(); i++)
    {
        result += stringsToJoin[i];
        if(i != stringsToJoin.size() - 1)
        {
            result += delim;
        }
    }
    return result;
}

bool contains( const std::wstring &src, const std::wstring &pattern, CaseSensitivity caseSensitivity /*= CaseSensitive*/ )
{
    if(caseSensitivity == CaseSensitive)
    {
        return src.find(pattern) != std::wstring::npos;
    }
    std::wstring srcLowerCase = src;
    std::transform(src.begin(), src.end(), srcLowerCase.begin(),tolower);
    std::wstring patternLowerCase = pattern;
    std::transform(pattern.begin(),pattern.end(),patternLowerCase.begin(),tolower);

    return srcLowerCase.find(patternLowerCase) != std::wstring::npos;
}

bool contains( const std::string &src, const std::string &pattern, CaseSensitivity caseSensitivity /*= CaseSensitive*/ )
{
    if(caseSensitivity == CaseSensitive)
    {
        return src.find(pattern) != std::string::npos;
    }
    std::string srcLowerCase = src;
    std::transform(src.begin(), src.end(), srcLowerCase.begin(), tolower);
    std::string patternLowerCase = pattern;
    std::transform(pattern.begin(),pattern.end(),patternLowerCase.begin(),tolower);

    return srcLowerCase.find(patternLowerCase) != std::string::npos;
}

bool starts_with( const std::wstring &src, const std::wstring &pattern, CaseSensitivity caseSensitivity /*= CaseSensitive*/ )
{
    if(caseSensitivity == CaseSensitive)
    {
        return src.find(pattern) == 0;
    }
    
    std::wstring srcLowerCase = src;
    std::transform(src.begin(), src.end(), srcLowerCase.begin(),tolower);
    std::wstring patternLowerCase = pattern;
    std::transform(pattern.begin(),pattern.end(),patternLowerCase.begin(),tolower);

    return srcLowerCase.find(patternLowerCase) == 0;
}

bool starts_with( const std::string &src, const std::string &pattern, CaseSensitivity caseSensitivity /*= CaseSensitive*/ )
{
    if(caseSensitivity == CaseSensitive)
    {
        return src.find(pattern) == 0;
    }

    std::string srcLowerCase = src;
    std::transform(src.begin(), src.end(), srcLowerCase.begin(), tolower);
    std::string patternLowerCase = pattern;
    std::transform(pattern.begin(),pattern.end(),patternLowerCase.begin(),tolower);

    return srcLowerCase.find(patternLowerCase) == 0;
}

std::wstring GetRandomAlnumName( int length )
{
  std::wstring result;
  static bool init = false;
  if(!init)
  {
    srand((unsigned)time(0));
    init = true;
  }
  for(int i = 0; i < length; i++)
  {
    int u = (double)rand()/(RAND_MAX + 1)*36;
    if(u >= 0 && u < 10)
    {
      u += '0';
    }
    else if(u >= 10 && u < 36)
    {
      u -= 10;
      u += 'a';
    }
    result.push_back(u);
  }
  return result;
}




}//namespace uni