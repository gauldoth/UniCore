#include "UDebug.h"

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace uni
{

void DebugMessage(const char *format,...)
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

void DebugMessage(const wchar_t *format,...)
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

}//namespace uni