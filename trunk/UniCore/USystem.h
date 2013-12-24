/*! \file USystem.h
    \brief 包含了一些系统API的简单封装.

    \date 2013-12-24
*/
#ifndef UNICORE_USYSTEM_H
#define UNICORE_USYSTEM_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#include <string>

//! Get temp path.
/*!
  The same as WinAPI GetTempPathW.
*/
std::wstring GetTempPathW();

//! Create a temp directory under temp directory.
/*!
  \return Returns empty string if failed.
*/
std::wstring CreateTempDirectory();

#endif//UNICORE_USYSTEM_H