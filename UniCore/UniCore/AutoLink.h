/*! \file AutoLink.h
    \brief  为静态库提供自动链接机制。

    该头文件允许被包含多次，因此没有包含保护。

    可以根据当前工程的编译选项连接到正确版本的静态库。
    具体作用请联系作者，或是参考boost库的自动链接机制。

    \code
    #define AUTO_LINK_LIB_NAME "UniCore"  //静态库的基础名，会根据当前工程的编译选项连接到"UniCoreMT.lib","UniCoreMTd.lib","UniCoreMD.lib","UniCoreMDd.lib"这4个静态库的其中一个。
    #include "AutoLink.h"
    \endcode

    \author     uni
    \date       2011-4-8
*/

#ifndef AUTO_LINK_LIB_NAME
#	error "宏AUTO_LINK_LIB_NAME未定义，再包含AutoLink.h前，必须先定义要自动连接的库名 (内部错误)"
#endif

#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#  pragma message("使用 /RTC 选项而不指定一个调试版本的运行时库会导致连接错误")
#  pragma message("提示： 在代码生成中选择一个调试版本的运行时库")
#  error "不兼容的编译选项"
#endif

#ifdef _DLL

#  if defined(_DEBUG)
#     define AUTO_LINK_RUNTIME_OPTION "MDd"
#  else
#     define AUTO_LINK_RUNTIME_OPTION "MD"
#  endif

#else

#  if defined(_DEBUG)
#      define AUTO_LINK_RUNTIME_OPTION "MTd"
#  else
#      define AUTO_LINK_RUNTIME_OPTION "MT"
#  endif

#endif

#pragma comment(lib,AUTO_LINK_LIB_NAME AUTO_LINK_RUNTIME_OPTION)

//定义AUTO_LINK_DIAGNOSTIC以显示实际连接到哪个lib文件。 
#ifdef AUTO_LINK_DIAGNOSTIC
#  pragma message("连接到lib文件: " AUTO_LINK_LIB_NAME AUTO_LINK_RUNTIME_OPTION ".lib")
#endif

#if defined(AUTO_LINK_LIB_NAME)
#  undef AUTO_LINK_LIB_NAME
#endif

#if defined(AUTO_LINK_RUNTIME_OPTION)
#  undef AUTO_LINK_RUNTIME_OPTION
#endif
