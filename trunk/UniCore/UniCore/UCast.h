/*! \file UCast.h
    \brief 类型转换相关。

    \date       2011-8-8
*/
#ifndef UNICORE_UCAST_H
#define UNICORE_UCAST_H

#include <string>

namespace uni
{

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
std::string ws2s(const std::wstring &ws,const char *locale = "");


//! 将std::wstring转换为std::string。
/*!
    ws2s使用指定的locale转换Unicode字符串到MBCS字符串。
    \param ws 源字符串。
    \param locale 使用_create_locale函数可以创建，使用完后需调用_free_locale释放。
    \return 转换后的字符串。
    使用方法：
    \code
    #include <locale.h>
    #include <string>
    #include "UCast.h"

    using namespace std;
    _locale_t loc = _create_locale(LC_ALL,"");
    string temp = ws2s(L"我们",loc);  //使用系统当前的区域设置进行转换。
    _free_locale(loc);
    \endcode
*/
std::string ws2s(const std::wstring &ws,_locale_t locale);


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
std::wstring s2ws(const std::string &s,const char *locale = "");


//! 将std::string转换为std::wstring。
/*!
    ws2s使用指定的locale转换MBCS字符串到Unicode字符串。
    \param s 源字符串。
    \param locale 使用_create_locale函数可以创建，使用完后需调用_free_locale释放。
    \return 转换后的字符串。
    使用方法：
    \code
    //Test.cpp
    //Test.cpp使用到<string>中定义的类，应该包含<string>。
    #include <locale.h>
    #include <string>
    #include "UCast.h"

    using namespace std;
    _locale_t loc = _create_locale(LC_ALL,"");
    wstring temp = s2ws("我们",loc);  //使用系统当前的区域设置进行转换。
    _free_locale(loc);
    \endcode
*/
std::wstring s2ws(const std::string &s,_locale_t locale);

}//namespace uni

#endif//UNICORE_UCAST_H