#include "UCast.h"

#include <array>
#include <assert.h>
#include <errno.h>
#include <sstream>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "UDebug.h"

using namespace std;

namespace uni
{

string ws2s(const wstring &ws,const char *locale /*= ""*/)
{
    string result;
    _locale_t loc = _create_locale(LC_CTYPE,locale);
    if(!loc)
    {
        return result;
    }
    const size_t len = ws.size()*4+1;
    char *dest = new char[len];
    size_t numOfCharConverted = 0;

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
        OutputDebugStringA("UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
    }
    delete []dest;
    return result;
}

string ws2s(const wstring &ws,_locale_t locale)
{
    string result;

    if(!locale)
    {
        return result;
    }

    const size_t len = ws.size()*4+1;
    char *dest = new char[len];
    size_t numOfCharConverted = 0; 

    errno_t err = _wcstombs_s_l(&numOfCharConverted,dest,len,ws.c_str(),_TRUNCATE,locale);
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
        OutputDebugStringA("UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
    }
    delete []dest;
    return result;
}

std::string ws2s( const std::wstring &ws,int codepage )
{
    string result;
    if(ws.empty())
    {
        return result;
    }
    int requiredSize = WideCharToMultiByte(codepage,0,ws.c_str(),ws.size(),0,0,0,0);
    if(requiredSize == 0)
    {
        DebugMessage("UniCore ws2s 转换字符串时发生错误.Last Error:%d",GetLastError());
        return result;
    }
    char *dest = new char[requiredSize];
    int numWritten = WideCharToMultiByte(codepage,0,ws.c_str(),ws.size(),dest,requiredSize,0,0);
    if(numWritten == 0)
    {
        delete []dest;
        DebugMessage("UniCore ws2s 转换字符串时发生错误.Last Error:%d",GetLastError());
        return result;
    }

    result.assign(dest,requiredSize);  //dest中保存的字符串不是以0结尾.
    delete []dest;

    return result;
}

wstring s2ws(const string &s,const char *locale /*= ""*/)
{
	assert(locale);

    wstring result;

    _locale_t loc = _create_locale(LC_CTYPE,locale);  //使用指定的locale。
	//如果_create_locale返回空,返回空字符串。
    if(!loc)
    {
        return result;
    }

	const size_t len = s.size()+1;
	wchar_t *dest = new wchar_t[len];
	size_t numOfCharConverted = 0;
    errno_t err = _mbstowcs_s_l(&numOfCharConverted,dest,len,s.c_str(),_TRUNCATE,loc);
    _free_locale(loc);
    if(err == 0)
    {
        result = dest;
    }
    else if(err == STRUNCATE)
    {
		assert(!"dest buffer should be adequate");
        result = dest;
    }
    else
    {
		//转换失败。
    }
    delete []dest;
    return result;
}

wstring s2ws(const string &s,_locale_t locale)
{
    wstring result;
    if(!locale)
    {
        return result;
    }
    const size_t len = s.size()+1;
    wchar_t *dest = new wchar_t[len];
    size_t numOfCharConverted = 0;

    errno_t err = _mbstowcs_s_l(&numOfCharConverted,dest,len,s.c_str(),_TRUNCATE,locale);

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
        OutputDebugStringA("UniCore s2ws 转换MBCS字符串到Unicode字符串时失败。");
    }
    delete []dest;
    return result;
}

std::wstring s2ws( const std::string &s,int codepage )
{
    wstring result;

    int requiredSize = MultiByteToWideChar(codepage,0,s.c_str(),s.size(),0,0);
    if(requiredSize == 0)
    {
        DebugMessage("UniCore s2ws 转换字符串时发生错误.Last Error:%d",GetLastError());
        return result;
    }
    wchar_t *dest = new wchar_t[requiredSize];
    int numWritten = MultiByteToWideChar(codepage,0,s.c_str(),s.size(),dest,requiredSize);
    if(numWritten == 0)
    {
        delete []dest;
        DebugMessage("UniCore s2ws 转换字符串时发生错误.Last Error:%d",GetLastError());
        return result;
    }

    result.assign(dest,requiredSize);  //dest中保存的字符串不是以0结尾.
    delete []dest;

    return result;
}

std::string i2s(long long int i )
{
    char buf[100] = "";
    _i64toa_s(i,buf,100,10);
    return buf;
}

std::wstring i2ws( long long int i )
{
	wchar_t buf[100] = L"";
	_i64tow_s(i,buf,100,10);
	return buf;
}

std::string operator+(const std::string &s,int i )
{
    return s + i2s(i);
}

std::string guid2s(GUID guid) 
{
	std::array<char,40> output;
	sprintf_s(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	return std::string(output.data());
}

}//namespace uni