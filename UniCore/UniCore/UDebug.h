/*! \file UDebug.h
    \brief 提供了调试常用的一些函数。

    \author     uni
    \date       2011-7-4
*/
#ifndef UNICORE_UDEBUG_H
#define UNICORE_UDEBUG_H

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

/*! \fn void DebugMessage(const wchar_t *format,...)
    \brief 格式化输出调试信息到调试器，使用方法和printf类似。

    
    \param format   待格式化的字符串。
    \param ...      用于格式化的参数。
*/
void DebugMessage(const wchar_t *format,...);

}//namespace uni

#endif//UNICORE_UDEBUG_H