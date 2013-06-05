// UniCoreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"
#include "ULiteTest.h"
#include "../UniCore/UCommon.h"

using namespace uni;
using namespace std;

void ULogTest()
{
    ULog::setProjectName("ULogTest");
    ULog::registerAppender("error",
        new ULog::FileAppender(L"error.log"));
    ULog::registerAppender("install",
        new ULog::FileAppender(L"install.log"));
    ULog::registerAppender("default",
        new ULog::DebuggerAppender);
    ULog::registerAppender("console",new ULog::ConsoleAppender);

     ULog::setAppender("安装","default console install");
//     ULog::setAppender("安装错误","error");
     ULog::setAppender("内存","console");

     ULog::enableOutput(ULog::ErrorType,false);
     UTRACE("内存")<<dumpmem((const char *)&ULogTest,100);
     UDEBUG<<1024<<"的16进制是"<<hexdisp(1024);
    UTRACE("安装")<<"开始安装...";

    UDEBUG("安装")<<"正在解压文件...";

    UDEBUG("安装")<<"解压文件结束.";
    for(int i = 0; i < 100; i++)
    {
        UINFO("安装")<<"正在安装第"<<i<<"个文件.";
        if(rand()%10 == 0)
        {
            UERROR("安装错误")<<"安装第"<<i<<"个文件时发生错误,忽略.";
            ULog::enableOutput("安装",false);
        }
    }
    UINFO("安装")<<"安装结束.";
    ULog::enableOutput(ULog::ErrorType,true);
}

void UCommonTest()
{
    ULog::setProjectName("ULogTest");
    ULog::registerAppender("default",
        new ULog::DebuggerAppender);
    ULog::registerAppender("console",new ULog::ConsoleAppender);

    ULog::setAppender("接口测试","default console");

    UINFO("接口测试")<<"UCommonTest 开始.";
    UINFO("接口测试")<<"1> trim ";
    std::string trimString = " ; 测试 ; ;  ";
    UDEBUG("接口测试")<<"trim space ["<<trimString<<"] ---> ["
        <<trim(trimString)<<"].";
    UDEBUG("接口测试")<<"trim space and ; ["<<trimString<<"] ---> ["
        <<trim(trimString," ;")<<"].";
}

int _tmain(int argc, _TCHAR* argv[])
{
    ULogTest();
    UCommonTest();

    ULiteTest();

    system("pause");
	return 0;
}

