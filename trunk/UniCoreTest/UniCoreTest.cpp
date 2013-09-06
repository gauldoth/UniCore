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


TEST(UStringTest,trim_EmptyString_ReturnsEmptyString)
{
    string result = trim("");
    ASSERT_EQ("",result);
}

TEST(UStringTest,trim_StringWithSpace_ReturnsStringTrimmed)
{
    string result = trim(" asd s   ");
    ASSERT_EQ("asd s",result);
}

TEST(UStringTest,trim_StringWithSpaceAndTab_ReturnsStringTrimmed)
{
    string result = trim("  asd\t "," \t");
    ASSERT_EQ("asd",result);
}

TEST(UStringTest,trim_AllCharsInStringIsInTrimChars_ReturnsEmptyString)
{
    string result = trim("asd","asd");
    ASSERT_TRUE(result.empty());
}

TEST(UStringTest,trim_TrimCharsIsEmpty_ReturnsOriginString)
{
    string result = trim("asd","");
    ASSERT_EQ("asd",result);
}


TEST(UMemoryTest,GetAt_BaseAndOffsetAreZero_ThrowsException)
{
    ASSERT_ANY_THROW(GetAt<int>(0,0)= 0);
}


TEST(UMemoryTest,GetAt_BaseAndOffset1AndOffset2AreZero_ThrowsException)
{
    ASSERT_ANY_THROW(GetAt<int>(0,0,0));
}

int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    system("pause");
    return 0;
}

