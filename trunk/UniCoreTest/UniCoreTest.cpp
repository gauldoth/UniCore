// UniCoreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"

using namespace uni;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

    UTRACE(流程)<<"线程开始执行."<<hexdisp(vbb);
    //UERROR(流程)<<"线程终止.";

    system("pause");
	return 0;
}

