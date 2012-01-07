#include "UCast.h"

#include <assert.h>
#include <errno.h>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

using namespace std;

namespace uni
{

string ws2s(const wstring &ws,const char *locale /*= ""*/)
{
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

string ws2s(const wstring &ws,_locale_t locale)
{
    string result;
    const size_t len = ws.size()*4+1;
    char *dest = new char[len];
    size_t numOfCharConverted = 0; 

    errno_t err = _wcstombs_s_l(&numOfCharConverted,dest,len,ws.c_str(),_TRUNCATE,locale);

    if(err == 0 || err == STRUNCATE)
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

void ws2s(char *dest,int len,const wchar_t *source,const char *locale )
{
    size_t numOfCharConverted = 0;
    _locale_t loc = _create_locale(LC_CTYPE,locale);
    if(!loc)
    {
        loc = _create_locale(LC_CTYPE,"");
    }
    errno_t err = _wcstombs_s_l(&numOfCharConverted,dest,len,source,_TRUNCATE,loc);
    _free_locale(loc);

    if(err == 0)
    {
    }
    else if(err == STRUNCATE)
    {
        OutputDebugStringA("UniCore ws2w 目标缓冲区不足，字符串被截断。");
    }
    else
    {
        assert(!"UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
        OutputDebugStringA("UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
    }
}

wstring s2ws(const string &s,const char *locale /*= ""*/)
{
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
    }
    else
    {
        assert(!"UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
        OutputDebugStringA("UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
    }
    delete []dest;
    return result;
}

wstring s2ws(const string &s,_locale_t locale)
{
    wstring result;
    const size_t len = s.size()+1;
    wchar_t *dest = new wchar_t[len];
    size_t numOfCharConverted = 0;

    errno_t err = _mbstowcs_s_l(&numOfCharConverted,dest,len,s.c_str(),_TRUNCATE,locale);

    if(err == 0 || err == STRUNCATE)
    {
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

void s2ws( wchar_t *dest,int len,const char *source,const char *locale )
{
    size_t numOfCharConverted = 0;
    _locale_t loc = _create_locale(LC_CTYPE,locale);  //使用指定的locale。
    if(!loc)
    {
        //指定的locale创建失败，使用实现指定的本地环境。
        loc = _create_locale(LC_CTYPE,"");
    }
    errno_t err = _mbstowcs_s_l(&numOfCharConverted,dest,len,source,_TRUNCATE,loc);
    _free_locale(loc);
    if(err == 0 || err == STRUNCATE)
    {
    }
    else
    {
        assert(!"UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
        OutputDebugStringA("UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
    }
}

}//namespace uni