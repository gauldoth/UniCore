// UniCoreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "../UniCore/ULog.h"
#include "../UniCore/UMemory.h"
#include "ULiteTest.h"
#include "../UniCore/UCommon.h"
#include "gtest/gtest.h"
#include "../UniCore/UBuffer.h"
#include "../UniCore/UCast.h"

using namespace uni;
using namespace std;

TEST(UBufferTest,TestAppendHexPattern)
{
    UBuffer buffer;
    buffer.appendHexPattern("31 31 31");
    EXPECT_STRCASEEQ("111",buffer.data());
    UBuffer buffer1;
    buffer1.appendHexPattern("34@2123");
    EXPECT_STRCASEEQ("4\x21\x23",buffer1.data());
    UBuffer buffer2;
    buffer2.appendHexPattern("3@4#5");
    EXPECT_STRCASEEQ("\x3\x4\x5",buffer2.data());
    UBuffer buffer3;
    buffer3.appendHexPattern("");
    EXPECT_STRCASEEQ("",buffer3.data());
}

TEST(UCastTest,Test_ws2s)
{
    EXPECT_STRCASEEQ("我们",ws2s(L"我们").c_str());
    EXPECT_STRCASEEQ("我们",ws2s(L"我们",".936").c_str());
    EXPECT_STRCASEEQ("ﾉﾁﾐﾄｼ",ws2s(L"ﾉﾁﾐﾄｼ",".932").c_str());
}

TEST(UCastTest,Test_i2s)
{
    EXPECT_STRCASEEQ("2",i2s(2).c_str());
    EXPECT_STRCASEEQ("-1",i2s(0xFFFFFFFF).c_str());
    EXPECT_STRCASEEQ("-25233",i2s(-25233).c_str());
    EXPECT_STRCASEEQ("25",i2s(25.6).c_str());
}

TEST(UStringTest,Test_trim)
{
    EXPECT_STRCASEEQ("my precious",trim("  my precious   ").c_str());
    EXPECT_EQ(L"my precious",trim(L"  my precious   "));
    EXPECT_EQ(L"我们",trim(L"@# $我们;  ",L"@#$ ;"));
    EXPECT_EQ(L"我们",trim(L" @ # $我们;  ",L"@#$$  @!$;; ;"));
}

int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc,argv);

    return RUN_ALL_TESTS();

    system("pause");
	return 0;
}

