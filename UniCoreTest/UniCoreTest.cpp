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
    cout<<"DumpString"<<endl;
    char *a = "asd";
    cout<<DumpString((int)0xFFFFF02)<<endl;
	return 0;
}

