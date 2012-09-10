/*! \file UCommon.h
    \brief 包含了一些常用的函数。

    \date       2011-8-5
*/
#ifndef UNICORE_UCOMMON_H
#define UNICORE_UCOMMON_H

#include <string>
#include <vector>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

/*! \def DISALLOW_COPY_AND_ASSIGN
    该宏用于禁止类拷贝复制。
    \code
    class Server
    {
    public:
        Server();
        ~Server();
    private:
        SOCKET socket;
        DISALLOW_COPY_AND_ASSIGN(Server);  //在private处加入该宏以禁止类Server复制，Server无法被放入标准库容器。
    };
    \endcode
*/
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    TypeName& operator=(const TypeName&);

/*! \def USE_DEFAULT_COPY
    类Class使用编译器自动生成的拷贝构造函数和赋值符号。
    \code
    class Server
    {
    public:
        Server();
        ~Server();
    private:
        SOCKET socket;
        USE_DEFAULT_COPY(Server);  //在private处加入该宏以禁止类Server复制，Server无法被放入标准库容器。
    };
*/
#define USE_DEFAULT_COPY(Class);

namespace uni
{
//! 获得一个随机的角色名字。
/*! 
    \param minSize 名字允许的最小长度，假如生成的名字长度小于该值，则重新生成。
    假如为0（默认值）则表示不限制最小长度。
    \param maxSize 名字允许的最大长度，假如生成的名字长度大于该值，则重新生成。
    假如为0（默认值）则表示不限制最大长度。
    \return 随机的角色名。
*/
std::string GetRandomRoleName(int minSize = 0,int maxSize = 0,int wordCount = 1,int randomSuffixCount = 0);

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
std::vector<std::string> split(const std::string &s,const std::string &delim = " ");


}//namespace uni

#endif//UNICORE_UCOMMON_H