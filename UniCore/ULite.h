/*! \file ULite.h
    \brief 该头文件包含了一些常用的函数.
    该头文件是UniCore库的精简版,没有cpp文件,直接包含即可使用.
    方便测试或者简单开发时使用,不能和UniCore库的其他头文件一起使用.

    \author unigauldoth@gmail.com
    \date 2013-5-28
*/
#ifndef UNICORE_ULITE_H
#define UNICORE_ULITE_H

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <cassert>
#include <typeinfo>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>

/*! \fn void DebugMessage(const char *format,...)
    \brief 格式化输出调试信息到调试器，使用方法和printf类似。
    
    \param format   待格式化的字符串。
    \param ...      用于格式化的参数。
*/
inline void DebugMessage(const char *format,...)
{
    va_list ap;
    va_start(ap,format);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(format,loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,format,loc,ap);
    _free_locale(loc);
    va_end(ap);
    OutputDebugStringA(buffer);

    delete[] buffer;
}

/*! \fn void DebugMessage(const wchar_t *format,...)
    \brief 格式化输出调试信息到调试器，使用方法和printf类似。

    
    \param format   待格式化的字符串。
    \param ...      用于格式化的参数。
*/
inline void DebugMessage(const wchar_t *format,...)
{
    va_list ap;
    va_start(ap,format);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscwprintf_p_l(format,loc,ap)+1;
    wchar_t *buffer = new wchar_t[len];

    _vswprintf_p_l(buffer,len,format,loc,ap);
    _free_locale(loc);
    va_end(ap);
    OutputDebugStringW(buffer);

    delete[] buffer;
}

//! 将std::string转换为std::wstring。
/*!
    ws2s使用指定的locale转换MBCS字符串到Unicode字符串。
    \param s 源字符串。
    \param locale locale名字，请参考setlocale函数。
    \return 转换后的字符串。
    使用方法：
    \code
    //Test.cpp
    //Test.cpp使用到<string>中定义的类，应该包含<string>。
    #include <string>
    #include "UCast.h"

    using namespace std;
    wstring temp = s2ws("我们");  //使用系统当前的区域设置进行转换。
    \endcode
*/
inline std::wstring s2ws(const std::string &s,const char *locale = "")
{
    using namespace std;
    wstring result;
    const size_t len = s.size()+1;
    wchar_t *dest = new wchar_t[len];
    size_t numOfCharConverted = 0;
    _locale_t loc = _create_locale(LC_CTYPE,locale);  //使用指定的locale。
    if(!loc)
    {
        //指定的locale创建失败，使用实现指定的本地环境。
        loc = _create_locale(LC_CTYPE,"");
    }
    errno_t err = _mbstowcs_s_l(&numOfCharConverted,dest,len,s.c_str(),_TRUNCATE,loc);
    _free_locale(loc);
    if(err == 0)
    {
        result = dest;
    }
    else if(err == STRUNCATE)
    {
        OutputDebugStringA("UniCore s2ws 目标缓冲区不足，字符串被截断。");
        result = dest;
    }
    else
    {
        assert(!"UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
        OutputDebugStringA("UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
    }
    delete []dest;
    return result;
}


//! 将std::wstring转换为std::string。
/*!
    ws2s使用指定的locale转换Unicode字符串到MBCS字符串。
    \param ws 源字符串。
    \param locale locale名字，请参考setlocale函数。
    \return 转换后的字符串。
    使用方法：
    \code
    //Test.cpp
    //Test.cpp使用到<string>中定义的类，应该包含<string>。
    #include <string>
    #include "UCast.h"

    using namespace std;
    string temp = ws2s(L"我们");  //使用系统当前的区域设置进行转换。
    \endcode
*/
inline std::string ws2s(const std::wstring &ws,const char *locale = "")
{
    using namespace std;
    string result;
    const size_t len = ws.size()*4+1;
    char *dest = new char[len];
    size_t numOfCharConverted = 0;
    _locale_t loc = _create_locale(LC_CTYPE,locale);
    if(!loc)
    {
        loc = _create_locale(LC_CTYPE,"");
    }
    errno_t err = _wcstombs_s_l(&numOfCharConverted,dest,len,ws.c_str(),_TRUNCATE,loc);
    _free_locale(loc);
    if(err == 0)
    {
        result = dest;
    }
    else if(err == STRUNCATE)
    {
        OutputDebugStringA("UniCore ws2s 目标缓冲区不足，字符串被截断。");
        result = dest;
    }
    else
    {
        assert(!"UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
        OutputDebugStringA("UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
    }
    delete []dest;
    return result;
}

//! 分割字符串.
/*!
    \param s 要分割的字符串.
    \param delim 分割用token.
    分割字符串s.每遇到一次delim就生成一个子字符串并保存到结果数组中.

    \code
    std::string s = "Acc Psw Credit";
    std::vector<std::string> results = split(s,
    \endcode
*/
inline std::vector<std::string> split(const std::string &s,const std::string &delim = " ")
{
    using namespace std;
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
            if(delimPos > startPos)
            {
                results.push_back(s.substr(startPos,delimPos-startPos));
            }
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

//! 从字符串的首尾剔除指定字符串.
/*!
    \param s 处理的字符串.
    \paran trim 要剔除的字符集合,默认为空格.
    该函数从s的首尾剔除字符,任何出现在字符串两端的trimChars中包含的字符都会被剔除.
    s为空字符串时也将返回空字符串.
    \code
    std::string s = "    this is a string with space.    ";
    std::string trimed = trim(s);  //trimed: "this is a string with space."
    trimed = trim(s," .");  //trimed: "this is a string with space"  这里空格和'.'都被剔除了.
    \endcode
*/
inline std::string trim(const std::string &s,const std::string &trimChars = " ")
{
    using namespace std;
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

#endif//UNICORE_ULITE_H