/*! \file UDebug.h
    \brief 提供了调试常用的一些函数.

    \author     uni
    \date       2011-7-4
*/
#ifndef UNICORE_UDEBUG_H
#define UNICORE_UDEBUG_H

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

namespace uni
{

/*! \fn void DebugMessage(const char *format,...)
    \brief 格式化输出调试信息到调试器，使用方法和printf类似。
    
    \param format   待格式化的字符串。
    \param ...      用于格式化的参数。
*/
void DebugMessage(const char *format,...);

//! 格式化输出调试信息到调试器，使用方法和printf类似。
/*!
    \param format 待格式化的字符串，这里不传引用，因为有用到可变参。
    \param ...用于格式化的参数。
*/
void DebugMessage(std::string format,...);

/*! \fn void DebugMessage(const wchar_t *format,...)
    \brief 格式化输出调试信息到调试器，使用方法和printf类似。

    
    \param format   待格式化的字符串。
    \param ...      用于格式化的参数。
*/
void DebugMessage(const wchar_t *format,...);

//! 格式化输出调试信息到调试器，使用方法和printf类似。
/*!
    \param format 待格式化的字符串，这里不传引用，因为有用到可变参。
    \param ...用于格式化的参数。
*/
void DebugMessage(std::wstring format,...);

//! 用于计时的秒表。
/*!
    通常用于统计代码执行时间。
    //局部使用的秒表。
    UStopwatch stopwatch;
    stopwatch.stime();  //获得经过时间。

    //全局使用的秒表。
    UStopwatch(2);

*/
class UStopwatch
{
public:
    enum {StopwatchCount = 50,};
    explicit UStopwatch(int index = -1);
    ~UStopwatch();

    //! 获得当前时间。
    std::string stime();
    //! 获得当前时间。
    static double milliseconds(int index = -1);
private:
    //! 开始计时。
    void start();
    //! 暂停计时。
    void pause();
    //! 继续计时。
    void restart();
    LARGE_INTEGER beginTime_;
    LARGE_INTEGER endTime_;
    LARGE_INTEGER freqTime_;
    int index_;
    static volatile double s_milliseconds_[StopwatchCount];
    double elapsedMilliseconds_;
};

}//namespace uni

#endif//UNICORE_UDEBUG_H