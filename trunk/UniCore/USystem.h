/*! \file USystem.h
    \brief 包含了一些系统API的简单封装.

    \date 2013-12-24
    \author unigauldoth@gmail.com
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
    //dirPart equals L"D:\\temp\\".
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

//! Get file part from a full path, without file extension.
/*!
  \param fullPath Path to analyze.
  \return File part of fullPath, without file extension.
  \code
  std::wstring fullPath = L"D:\\temp\\a.txt";
  std::wstring filePart = GetFilePart_WithoutExt(fullPath);
  //filePart equals L"a".
  \endcode
*/
std::wstring GetFilePart_WithoutExt(const std::wstring &fullPath);

//! Get file size.
long long int GetFileSize(const std::wstring &fullPath);

//! Get the current directory.
/*!
*/
std::wstring GetCurrentDir();

//! 删除文件夹及所有包含内容.
/*!
    \param directory 要删除的目录,必须以'\'结尾.
    \return 是否删除成功.
*/
bool DeleteDirectory(const std::wstring &directory);

//! Checks whether a specified directory exists.
/*!
  \param directory Directory to check.
  \return Whether the specified directory exists.
*/
bool DirectoryExists(const std::wstring &directory);

//! Create directories.
/*!
  \param path For example,"D:\temp","D:\tools\msihack\",...
         参数可以为绝对路径,相对路径.
  \return Whether all directories are created successfully.
*/
bool CreateDirectories(const std::wstring &path);


#endif//UNICORE_USYSTEM_H