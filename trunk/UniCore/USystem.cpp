#include "USystem.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "UProcess.h"
#include "UCommon.h"

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


