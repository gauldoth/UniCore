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
		if (result.back() != L'\\')
		{
			result.push_back(L'\\');
		}
    }
    else
    {
        UERROR<<"当前进程镜象路径中找不到\\";
    }
    return result;
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

float GetCPUUsage( int pid )
{
	FILETIME sysIdle;
	FILETIME sysKernel;
	FILETIME sysUser;

	FILETIME procCreation;
	FILETIME procExit;
	FILETIME procKernel;
	FILETIME procUser;

	static bool firstRun = true;
	static float cpuUsage = 0;
	static FILETIME prevSysKernel;
	static FILETIME prevSysUser;
	static FILETIME prevProcKernel;
	static FILETIME prevProcUser;
	
	auto SubtractTimes = [](const FILETIME &t1, const FILETIME &t2)->ULONGLONG
	{
		LARGE_INTEGER a;
		LARGE_INTEGER b;
		a.LowPart = t1.dwLowDateTime;
		a.HighPart = t1.dwHighDateTime;

		b.LowPart = t2.dwLowDateTime;
		b.HighPart = t2.dwHighDateTime;

		return a.QuadPart-b.QuadPart;
	};

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pid);
	ON_OUT_OF_SCOPE(CloseHandle(hProcess));


	if(!GetSystemTimes(&sysIdle,&sysKernel,&sysUser) 
		|| !GetProcessTimes(hProcess,&procCreation,&procExit,&procKernel,&procUser))
	{
		return cpuUsage;
	}
	
	//
	if(!firstRun)
	{
		ULONGLONG sysKernelDiff = SubtractTimes(sysKernel,prevSysKernel);
		ULONGLONG sysUserDiff = SubtractTimes(sysUser,prevSysUser);
		ULONGLONG procKernelDiff = SubtractTimes(procKernel,prevProcKernel);
		ULONGLONG procUserDiff = SubtractTimes(procUser,prevProcUser);

		ULONGLONG totalSys = sysKernelDiff+sysUserDiff;
		ULONGLONG totalProc = procKernelDiff+procUserDiff;

		if(totalSys > 0)
		{
			cpuUsage = (float)totalProc/totalSys;
		}
	}
	else
	{
		firstRun = false;
	}

	prevSysKernel = sysKernel;
	prevSysUser = sysUser;
	prevProcKernel = procKernel;
	prevProcUser = procUser;

	return cpuUsage;
}

int GetProcessBitness(int pid)
{
	return 32;
}


}//namespace uni