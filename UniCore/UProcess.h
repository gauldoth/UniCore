/*! \file UProcess.h
    \brief 包含了一些进程相关的函数。

    \date 2011-9-6
*/
#ifndef UNICORE_UPROCESS_H
#define UNICORE_UPROCESS_H

#include <string>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace uni
{

    
//! 授予当前进程调试权限。
/*!
    \return 是否成功。
*/
bool EnableDebugPrivilege();

//! 获得当前进程所在路径。
/*!
    \return 当前进程的可执行文件所在路径。
*/
std::wstring GetCurrentProcessDirectory();

//! 获得进程名。
/*!
    \param pid 进程id。
    \return 进程名。
*/
std::wstring GetProcessName(unsigned int pid);

//! 设置AccessToken的权限。
/*!
    \param hToken AccessToken句柄，需要有TOKEN_ADJUST_PRIVILEGES访问权限。
    \param privilege 特权常量(Privilege Constants)。
    \param enabled 该参数为是时，设置特权；为否时，删除特权。
    \return 是否设置成功。
*/
bool SetTokenPrivilege(HANDLE hToken,wchar_t *privilege,bool enabled);


}//namespace uni

#endif//UNICORE_UPROCESS_H