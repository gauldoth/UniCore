/*! \file UCommon.h
    \brief 包含了一些常用的函数。

    \date       2011-8-5
*/
#ifndef UNICORE_UCOMMON_H
#define UNICORE_UCOMMON_H

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

#endif//UNICORE_UCOMMON_H