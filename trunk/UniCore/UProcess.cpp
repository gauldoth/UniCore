#include "UProcess.h"

#include <assert.h>

#include "psapi.h"

#include "ULog.h"
#include "UCommon.h"
#include <regex>

#pragma comment(lib,"psapi.lib")

using namespace std;

namespace uni
{

bool EnableDebugPrivilege()
{
    HANDLE hToken;

    if(!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES,
        &hToken))
    {
        UERROR<<"OpenProcessToken失败。"<<lasterr;
        return false;
    }

    bool result = SetTokenPrivilege(hToken,SE_DEBUG_NAME,true);

    if(!CloseHandle(hToken))
    {
        UERROR<<"CloseHandle失败。"<<lasterr;
        result = false;
    }

    return result;
}


bool FileExists( const std::wstring &fileName )
{
    ifstream ifs;
    ifs.open(fileName.c_str());
    return ifs.is_open();

}

bool FileExists( const std::string &fileName )
{
    ifstream ifs;
    ifs.open(fileName.c_str());
    return ifs.is_open(); 
}

wstring GetProcessName(unsigned int pid)
{
    wstring name;
    if(pid == 0)
    {
        name = L"System Idle Process"; 
        return name;
    }
    else if(pid == 4)
    {
        name = L"System";
        return name;
    }
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pid);

    if(!hProcess)
    {
        UERROR<<"OpenProcess失败。"<<lasterr;
    }
    else
    {
        wchar_t imageFileName[MAX_PATH] = L"";
        if(!GetProcessImageFileName(hProcess,imageFileName,MAX_PATH))
        {
            UERROR<<"GetProcessImageFileName失败。"<<lasterr;
        }
        else
        {
            wchar_t *fileName = wcsrchr(imageFileName,L'\\');
            if(fileName)
            {
                name = fileName+1;
            }
        }
        CloseHandle(hProcess);
    }

    return name;
}

bool SetTokenPrivilege(HANDLE hToken,wchar_t *privilege,bool enabled)
{
    LUID luid;
    if(!LookupPrivilegeValue(L"",privilege,&luid))
    {
        UERROR<<"LookupPrivilegeValue失败。"<<lasterr;
        return false;
    }
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;

    tp.Privileges[0].Attributes = 
        enabled ? SE_PRIVILEGE_ENABLED : 0;

    if(!AdjustTokenPrivileges(hToken,FALSE,&tp,
        sizeof(TOKEN_PRIVILEGES),(PTOKEN_PRIVILEGES)NULL,(PDWORD)NULL))
    {
        UERROR<<"AdjustTokenPrivileges失败。"<<lasterr;
        return false;
    }

    return true;
}

std::wstring GetCurrentProcessDirectory()
{
    std::wstring result;
    wchar_t path[MAX_PATH] = L"";
    int ret = GetModuleFileNameW(NULL,path,MAX_PATH);
    if(!ret)
    {
        UERROR<<"GetModuleFileName失败。"<<lasterr;
        return result;
    }
    else if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        UERROR<<"缓冲区不足。";
        return result;
    }

    wchar_t *exeFileName = wcsrchr(path,L'\\');
    if(exeFileName)
    {
        exeFileName[0] = L'\0';
        result = path;
    }
    else
    {
        UERROR<<"当前进程镜象路径中找不到\\";
    }
    return result;
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

std::wstring LCIDToRFC1766( LCID lcid )
{
    if(lcid == 0x540a || lcid == 0x580a)
    {
      return L"es";
    }
    //HKEY_LOCAL_MACHINE\SOFTWARE\Classes\MIME\Database\Rfc1766
    std::wstring result;
    HKEY hKeyRFC1766 = NULL;
    LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Classes\\MIME\\Database\\Rfc1766",
        0,KEY_READ,&hKeyRFC1766);

    if(ret != ERROR_SUCCESS)
    {
        assert(!"LCIDToRFC1766 failed, could not open HKLM\\SOFTWARE\\Classes\\MIME\\Database\\Rfc1766.");
        return result;
    }
    wchar_t buf[255] = L"";
    swprintf_s(buf,255,L"%04X",lcid);
    wchar_t valueBuf[MAX_PATH] = L"";
    DWORD bufSize = sizeof(valueBuf);
    ret = RegQueryValueEx(hKeyRFC1766,buf,0,NULL,(LPBYTE)valueBuf,&bufSize);
    if(ret != ERROR_SUCCESS)
    {
        //assert(!"LCIDToRFC1766 failed, could not query value in HKLM\\SOFTWARE\\Classes\\MIME\\Database\\Rfc1766.");
        RegCloseKey(hKeyRFC1766);
        return result;
    }

    result.assign(valueBuf,2);

    RegCloseKey(hKeyRFC1766);
    return result;
}

bool CreateDirectories( const std::wstring &path )
{
  //支持LFS(Local File System)和UNC(Universal Naming Convention).
  //"\\192.168.0.2\Home\【Share】\xiaodong_li"
  //"C:\Program Files\Foxit Software\Foxit PhantomPDF"
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
  wregex reVolumeName(L"\\w+?:\\\\");
  if(regex_search(path,result,reVolumeName))
  {
    volumeName = result[0];
    remaining = result.suffix();
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

}//namespace uni