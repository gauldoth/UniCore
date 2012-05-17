#include "UDebug.h"

#include <typeinfo>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>

#include "..//UniCore//ULog.h"

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

void DebugMessage(const std::string &format,...)
{
    va_list ap;
    va_start(ap,format);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(format.c_str(),loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,format.c_str(),loc,ap);
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

void DebugMessage(const std::wstring &format,...)
{
    va_list ap;
    va_start(ap,format);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscwprintf_p_l(format.c_str(),loc,ap)+1;
    wchar_t *buffer = new wchar_t[len];

    _vswprintf_p_l(buffer,len,format.c_str(),loc,ap);
    _free_locale(loc);
    va_end(ap);
    OutputDebugStringW(buffer);

    delete[] buffer;
}

volatile double UStopwatch::s_milliseconds_[50] = {0.0};

UStopwatch::UStopwatch(int index /*= -1*/)
:index_(index)
,elapsedMilliseconds_(0)
{
    beginTime_.QuadPart = 0;
    endTime_.QuadPart = 0;
    freqTime_.QuadPart = 0;
    QueryPerformanceFrequency(&freqTime_);
    if(index_ < -1 && index_ >= StopwatchCount)
    {
        index_ = -1;
    }
    if(index_ == -1)
    {
        start();
    }
    else if(s_milliseconds_[index] == 0)
    {
        start();
    }
    else
    {
        restart();
    }
}

UStopwatch::~UStopwatch()
{
    //DebugMessage("enter dtor");
    pause();
}

void UStopwatch::start()
{
    QueryPerformanceCounter(&beginTime_);
}

void UStopwatch::pause()
{
    QueryPerformanceCounter(&endTime_);

    double result = 0;
    result = 
        (double)(endTime_.QuadPart - beginTime_.QuadPart) * 1000 / (double)freqTime_.QuadPart;


    if(index_ < -1 && index_ >= StopwatchCount)
    {
        index_ = -1;
    }
    if(index_ != -1)
    {
        s_milliseconds_[index_] += result;
    }
    else
    {
        elapsedMilliseconds_ += result;
    }

//     __int64 hour = result/3600000;
//     __int64 minute = (result/60000)%60;
//     __int64 second = (result/1000)%60;
//     __int64 miliSecond = result%1000;

//     char buf[100] = "";
//     sprintf_s(buf,"%02d:%02d:%02d'%03d",(int)hour,(int)minute,(int)second,(int)miliSecond);
//     time_ = buf;
}

void UStopwatch::restart()
{
    QueryPerformanceCounter(&beginTime_);
}

std::string UStopwatch::stime()
{
    pause();
    double milliseconds = 0.0;
    if(index_ < -1 && index_ >= StopwatchCount)
    {
        index_ = -1;
    }
    if(index_ != -1)
    {
        milliseconds = s_milliseconds_[index_];
    }
    else
    {
        milliseconds = elapsedMilliseconds_;
    }
    __int64 hour = (__int64)milliseconds/3600000;
    __int64 minute = ((__int64)milliseconds/60000)%60;
    __int64 second = ((__int64)milliseconds/1000)%60;
    __int64 miliSecond = (__int64)milliseconds%1000;

    char buf[100] = "";
    sprintf_s(buf,"%02d:%02d:%02d'%03d",(int)hour,(int)minute,(int)second,(int)miliSecond);
    std::string time = buf;
    restart();
    return time;
}

double UStopwatch::milliseconds( int index /*= -1*/ )
{

    return s_milliseconds_[index];
}

}//namespace uni