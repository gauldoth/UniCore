/*! \file UCast.h
    \brief 类型转换相关。

    \date       2011-8-8
*/
#ifndef UNICORE_UCAST_H
#define UNICORE_UCAST_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
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

//! 将std::wstring转换为std::string.
/*!
    根据指定代码页将wstring转换为string.
    \param ws utf-16编码的字符串.
    \param codepage 根据哪个代码页来转换.
    \return 转换后的字符串.
*/
std::string ws2s(const std::wstring &ws,int codepage);


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
	\pre locale为有效的字符串。
	\post 如果source可以通过指定locale转换，则返回转换后的字符串。否则返回空字符串。
	这个函数只能保证尽量完成转换，但是始终会返回一个有效的字符串。
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

//! 将std::string转换为std::wstring.
/*!
    根据指定代码页将std::string转换为std::wstring.
    \param s 源字符串.
    \param codepage 转换时采用的代码页.
    \return 转换后的unicode字串.
*/
std::wstring s2ws(const std::string &s,int codepage);

//! 将std::string转换为std::wstring。
/*!
    ws2s使用指定的locale转换MBCS字符串到Unicode字符串。
    \param s 源字符串。
    \param locale 使用_create_locale函数可以创建，使用完后需调用_free_locale释放。
    \return 转换后的字符串。
	该函数不保证一定能转换成功，在无法转换成功时，。
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


//! 整形转成字符串。
/*!
    \param i 要转换的整形。
    \return 字符串表示的数字。
*/
std::string i2s(long long int i);

//! 整形转成宽字符串.
/*!
	\param i 要转换的整形.
	\return 字符串表示的数字.
*/
std::wstring i2ws(long long int i);

//! 在string右边添加整型值.
std::string operator+ (const std::string &s,int i);

//! guid转换成std::string
/*!
	\param guid 待转换的guid.
	\return 字符串格式的guid,格式为{00000000-0000-0000-0000-000000000000}.
*/
std::string guid2s(GUID guid);


}//namespace uni

#endif//UNICORE_UCAST_H