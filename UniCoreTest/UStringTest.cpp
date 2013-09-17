#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include "../UniCore/UCommon.h"

using namespace std;
using namespace uni;

TEST(UStringTest,split_EmptyString_ReturnsZeroResult)
{
    vector<string> result = split("");
    ASSERT_TRUE(result.empty());
}

TEST(UStringTest,split_NormalString_ReturnsOneResult)
{
    vector<string> result = split("apple");
    ASSERT_EQ(1,result.size());
    EXPECT_EQ("apple",result[0]);
}

TEST(UStringTest,split_NormalStringWithTwoSpace_ReturnsTwoString)
{
    vector<string> result = split("apple  banana");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("apple",result[0]);
    EXPECT_EQ("banana",result[1]);
}

TEST(UStringTest,split_SourceStringAndDelimAreEmptyString_ReturnsNothing)
{
    vector<string> result = split("","");
    ASSERT_EQ(0,result.size());
}

TEST(UStringTest,split_DelimSizeGreatThanSource_ReturnsNothing)
{
    vector<string> result = split("cup","    ");
    ASSERT_EQ(0,result.size());
}

TEST(UStringTest,split_DelimAtBeginAndEnd_ReturnsValidResult)
{
    vector<string> result = split(";cup inside;next;;",";");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("cup inside",result[0]);
    EXPECT_EQ("next",result[1]);
}

TEST(UStringTest,split_DelimSizeGreaterThanTwo_SplitSuccess)
{
    vector<string> result = split("**cup inside***next;***;*","**");
    ASSERT_EQ(3,result.size());
    EXPECT_EQ("cup inside",result[0]);
    EXPECT_EQ("*next;",result[1]);
    EXPECT_EQ("*;*",result[2]);
}

TEST(UStringTest,split_w_EmptyString_ReturnsZeroResult)
{
    vector<wstring> result = split(L"");
    ASSERT_TRUE(result.empty());
}

TEST(UStringTest,split_w_NormalString_ReturnsOneResult)
{
    vector<wstring> result = split(L"apple");
    ASSERT_EQ(1,result.size());
    EXPECT_EQ(L"apple",result[0]);
}

TEST(UStringTest,split_w_NormalStringWithTwoSpace_ReturnsTwoString)
{
    vector<wstring> result = split(L"apple  banana");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ(L"apple",result[0]);
    EXPECT_EQ(L"banana",result[1]);
}

TEST(UStringTest,split_w_SourceStringAndDelimAreEmptyString_ReturnsNothing)
{
    vector<wstring> result = split(L"",L"");
    ASSERT_EQ(0,result.size());
}

TEST(UStringTest,split_w_DelimSizeGreatThanSource_ReturnsNothing)
{
    vector<wstring> result = split(L"cup",L"    ");
    ASSERT_EQ(0,result.size());
}

TEST(UStringTest,split_w_DelimAtBeginAndEnd_ReturnsValidResult)
{
    vector<wstring> result = split(L";cup inside;next;;",L";");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ(L"cup inside",result[0]);
    EXPECT_EQ(L"next",result[1]);
}

TEST(UStringTest,split_w_DelimSizeGreaterThanTwo_SplitSuccess)
{
    vector<wstring> result = split(L"**cup inside***next;***;*",L"**");
    ASSERT_EQ(3,result.size());
    EXPECT_EQ(L"cup inside",result[0]);
    EXPECT_EQ(L"*next;",result[1]);
    EXPECT_EQ(L"*;*",result[2]);
}

TEST(UStringTest,trim_EmptyString_ReturnsEmptyString)
{
    string trimmed = trim("");
    ASSERT_EQ("",trimmed);
}

TEST(UStringTest,trim_NormalString_ReturnsNormalString)
{
    string trimmed = trim("vodoo");
    ASSERT_EQ("vodoo",trimmed);
}

TEST(UStringTest,trim_NormalStringWithPaddingSpace_TrimSuccess)
{
    string trimmed = trim("  glamorous sky ");
    ASSERT_EQ("glamorous sky",trimmed);
}

TEST(UStringTest,trim_TwoTrimChars_TrimSuccess)
{
    string trimmed = trim(" \tcoffee\tall\t  "," \t");
    ASSERT_EQ("coffee\tall",trimmed);
}

TEST(UStringTest,trim_NoTrimChars_ReturnInputString)
{
    string trimmed = trim(" \tcoffee\tall\t  ","");
    ASSERT_EQ(" \tcoffee\tall\t  ",trimmed);
}

TEST(UwstringTest,trim_w_Emptywstring_ReturnsEmptywstring)
{
    wstring trimmed = trim(L"");
    ASSERT_EQ(L"",trimmed);
}

TEST(UwstringTest,trim_w_Normalwstring_ReturnsNormalwstring)
{
    wstring trimmed = trim(L"vodoo");
    ASSERT_EQ(L"vodoo",trimmed);
}

TEST(UwstringTest,trim_w_NormalwstringWithPaddingSpace_TrimSuccess)
{
    wstring trimmed = trim(L"  glamorous sky ");
    ASSERT_EQ(L"glamorous sky",trimmed);
}

TEST(UwstringTest,trim_w_TwoTrimChars_TrimSuccess)
{
    wstring trimmed = trim(L" \tcoffee\tall\t  ",L" \t");
    ASSERT_EQ(L"coffee\tall",trimmed);
}

TEST(UwstringTest,trim_w_NoTrimChars_ReturnInputwstring)
{
    wstring trimmed = trim(L" \tcoffee\tall\t  ",L"");
    ASSERT_EQ(L" \tcoffee\tall\t  ",trimmed);
}