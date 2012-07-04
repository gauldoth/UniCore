#include "UProcess.h"

#include <assert.h>

#include "psapi.h"

#include "ULog.h"

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


}//namespace uni