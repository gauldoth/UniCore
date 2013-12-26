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

//! Get directory part from a full path.
/*!
    \param fullPath Path to analyze.
    \return Directory part of fullPath.
    \code
    std::wstring fullPath = L"D:\\temp\\a.txt";
    std::wstring dirPart = GetDirectoryPart(fullPath);
    //dirPart equals L"D:\\temp".
    \endcode
*/
std::wstring GetDirectoryPart(const std::wstring &fullPath);

//! Get file part from a full path, include file extension.
/*!
  \param fullPath Path to analyze.
  \return File part of fullPath.
  \code
  std::wstring fullPath = L"D:\\temp\\a.txt";
  std::wstring filePart = GetFilePart(fullPath);
  //filePart equals L"a.txt".
  \endcode
*/
std::wstring GetFilePart(const std::wstring &fullPath);

#endif//UNICORE_USYSTEM_H