// UniCoreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"

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

     ULog::setAppender("��װ","default console install");
//     ULog::setAppender("��װ����","error");
     ULog::setAppender("�ڴ�","console");

     ULog::enableOutput(ULog::ErrorType,false);
     UTRACE("�ڴ�")<<dumpmem((const char *)&ULogTest,100);
     UDEBUG<<1024<<"��16������"<<hexdisp(1024);
    UTRACE("��װ")<<"��ʼ��װ...";

    UDEBUG("��װ")<<"���ڽ�ѹ�ļ�...";

    UDEBUG("��װ")<<"��ѹ�ļ�����.";
    for(int i = 0; i < 100; i++)
    {
        UINFO("��װ")<<"���ڰ�װ��"<<i<<"���ļ�.";
        if(rand()%10 == 0)
        {
            UERROR("��װ����")<<"��װ��"<<i<<"���ļ�ʱ��������,����.";
            ULog::enableOutput("��װ",false);
        }
    }
    UINFO("��װ")<<"��װ����.";
}

int _tmain(int argc, _TCHAR* argv[])
{
    ULogTest();

    system("pause");
	return 0;
}

