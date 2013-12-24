/*! \file USystem.h
    \brief 包含了一些系统API的简单封装.

    \date 2013-12-24
*/
#ifndef UNICORE_USYSTEM_H
#define UNICORE_USYSTEM_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#include <string>

std::wstring GetTempPathW();

#endif//UNICORE_USYSTEM_H