#include "USystem.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "UProcess.h"
#include "UCommon.h"
#include <fstream>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

using namespace uni;

std::wstring GetTempPathW()
{
  const int bufSize = MAX_PATH+1;
  wchar_t buf[bufSize] = L"";
  if(GetTempPathW(bufSize,buf))
  {
    return buf;
  }
  else
  {
    return std::wstring();
  }
}

std::wstring CreateTempDirectory()
{
  std::wstring tempPath = GetTempPathW();
  if(tempPath.empty())
  {
    return L"";
  }

  if(tempPath.back() != L'\\')
  {
    tempPath.push_back(L'\\');
  }

  for(int i = 0; i < 10; i++)
  {
    std::wstring pathToCreate = tempPath + GetRandomAlnumName(10);
    if(!DirectoryExists(pathToCreate))
    {
      if(CreateDirectories(pathToCreate))
      {
        return pathToCreate;
      }
    }
  }

  return L"";
}

std::wstring GetDirectoryPart( const std::wstring &fullPath )
{
    size_t pos = fullPath.rfind(L'\\');
    if(pos == std::wstring::npos)
    {
        return L"";
    }

    return fullPath.substr(0,pos+1);
}

std::wstring GetFilePart( const std::wstring &fullPath )
{
  size_t pos = fullPath.rfind(L'\\');
  if(pos == std::wstring::npos)
  {
    return L"";
  }

  return fullPath.substr(pos+1);
}

std::wstring GetFilePart_WithoutExt( const std::wstring &fullPath )
{
    std::wstring filePart = GetFilePart(fullPath);
    size_t pos = filePart.rfind(L'.');
    if(pos != std::wstring::npos)
    {
        filePart = filePart.substr(0,pos);
    }

    return filePart;
}


long long int GetFileSize( const std::wstring &fullPath )
{
  std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
  return file.tellg();
}

std::wstring GetCurrentDir()
{
    std::wstring result;
    DWORD bufSize = GetCurrentDirectoryW(0, NULL);
    wchar_t *buf = new wchar_t(bufSize);
    DWORD ret = GetCurrentDirectoryW(bufSize, buf);
    if(ret == 0)
    {
        result = L"";
    }
    else
    {
        result = buf;
    }
    delete [] buf;
    return result;
}

