#include "USystem.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "UProcess.h"
#include "UCommon.h"
#include <fstream>
#include <regex>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

using namespace uni;
using namespace std;

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
    wchar_t *buf = new wchar_t[bufSize];
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

bool CreateDirectories( const std::wstring &path )
{
	//支持LFS(Local File System)和UNC(Universal Naming Convention).
	//支持相对路径,'.'和'..'.
	//"\\192.168.0.2\Home\【Share】\xiaodong_li"
	//"C:\Program Files\Foxit Software\Foxit PhantomPDF"
	//"MsiLog\SubDir"
	std::wstring uncName;
	std::wstring volumeName;
	std::wstring remaining;
	//"\\.*?\.*?\",由于c++和regex都用到转义,因此一个'\'变成了四个.
	wregex reUncName(L"\\\\\\\\.*?\\\\.*?\\\\");
	wsmatch result;
	if(regex_search(path,result,reUncName))
	{
		uncName = result[0];
		remaining = result.suffix();
	}
	else
	{
		wregex reVolumeName(L"\\w+?:\\\\");
		if(regex_search(path,result,reVolumeName))
		{
			volumeName = result[0];
			remaining = result.suffix();
		}
		else
		{
			remaining = path;
		}
	}

	std::vector<std::wstring> pathFragments = split(remaining,L"\\");

	std::wstring directoryToCreate;
	if(!uncName.empty())
	{
		directoryToCreate = uncName;
	}
	else if(!volumeName.empty())
	{
		directoryToCreate = volumeName;
	}
	else
	{
		//directoryToCreate remains empty.
		directoryToCreate = GetCurrentDir();
	}

	if(directoryToCreate.back() != L'\\')
	{
		directoryToCreate.push_back(L'\\');
	}

	for(int i = 0; i < pathFragments.size(); i++)
	{
		if(!pathFragments[i].empty())
		{

			directoryToCreate += pathFragments[i];
			if(directoryToCreate.back() != L'\\')
			{
				directoryToCreate.push_back(L'\\');
			}
			if(!DirectoryExists(directoryToCreate.c_str()))
			{
				if(!CreateDirectoryW(directoryToCreate.c_str(),NULL))
				{
					return false;
				}
			}
		}
	}

	return true;

}

bool DirectoryExists( const std::wstring &directory )
{
	DWORD attribute = GetFileAttributesW(directory.c_str());
	if (attribute == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	if (attribute & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}

	return false;
}

bool DeleteDirectory(const std::wstring &path)
{
	if(path.empty())
	{
		return false;
	}
	else
	{
		if(path[path.size()-1] != L'\\')
		{
			return false;
		}
	}
	WIN32_FIND_DATA ffd = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;

	std::wstring filesToFind = path + L"*";
	hFind = FindFirstFile(filesToFind.c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind) 
	{
		return false;
	} 

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wcscmp(ffd.cFileName,L".") == 0
				|| wcscmp(ffd.cFileName,L"..") == 0)
			{
				continue;
			}
			if(!DeleteDirectory((path + ffd.cFileName + L"\\").c_str()))
			{
				return false;
			}
		}
		else
		{
			DeleteFileW((path + ffd.cFileName).c_str());
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return RemoveDirectoryW(path.c_str());
}
