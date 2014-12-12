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

TEST(UStringTest,split_NormalStringWithTwoSpace_ReturnsThreeString)
{
    vector<string> result = split("apple  banana");
    ASSERT_EQ(3,result.size());
    EXPECT_EQ("apple",result[0]);
    EXPECT_EQ("",result[1]);
    EXPECT_EQ("banana",result[2]);
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
    ASSERT_EQ(5,result.size());
    EXPECT_EQ("",result[0]);
    EXPECT_EQ("cup inside",result[1]);
    EXPECT_EQ("next",result[2]);
    EXPECT_EQ("",result[3]);
    EXPECT_EQ("",result[4]);
}

TEST(UStringTest,split_OnlyOneDelim_ReturnsTwoEmptyString)
{
    vector<string> result = split(";",";");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("",result[0]);
    EXPECT_EQ("",result[1]);
}

TEST(UStringTest,split_DelimSizeGreaterThanTwo_SplitSuccess)
{
    vector<string> result = split("**cup inside***next;***;*","**");
    ASSERT_EQ(4,result.size());
    EXPECT_EQ("",result[0]);
    EXPECT_EQ("cup inside",result[1]);
    EXPECT_EQ("*next;",result[2]);
    EXPECT_EQ("*;*",result[3]);
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

TEST(UStringTest,split_w_NormalStringWithTwoSpace_ReturnsThreeString)
{
    vector<wstring> result = split(L"apple  banana");
    ASSERT_EQ(3,result.size());
    EXPECT_EQ(L"apple",result[0]);
    EXPECT_EQ(L"",result[1]);
    EXPECT_EQ(L"banana",result[2]);
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
    ASSERT_EQ(5,result.size());
    EXPECT_EQ(L"",result[0]);
    EXPECT_EQ(L"cup inside",result[1]);
    EXPECT_EQ(L"next",result[2]);
    EXPECT_EQ(L"",result[3]);
    EXPECT_EQ(L"",result[4]);
}

TEST(UStringTest,split_w_OnlyOneDelim_ReturnsTwoEmptyString)
{
    vector<wstring> result = split(L";",L";");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ(L"",result[0]);
    EXPECT_EQ(L"",result[1]);
}

TEST(UStringTest,split_w_DelimSizeGreaterThanTwo_SplitSuccess)
{
    vector<wstring> result = split(L"**cup inside***next;***;*",L"**");
    ASSERT_EQ(4,result.size());
    EXPECT_EQ(L"",result[0]);
    EXPECT_EQ(L"cup inside",result[1]);
    EXPECT_EQ(L"*next;",result[2]);
    EXPECT_EQ(L"*;*",result[3]);
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

TEST(UStringTest,trim_CheckTrimLeftFirst)
{
  string trimmed = trim("appappa","appa");
  ASSERT_EQ("ppa",trimmed);
}

TEST(UStringTest,trim_NormalStringWithPaddingSpace_TrimSuccess)
{
    string trimmed = trim("  glamorous sky ");
    ASSERT_EQ("glamorous sky",trimmed);
}

TEST(UStringTest,trim_NoTrimChars_ReturnInputString)
{
    string trimmed = trim(" \tcoffee\tall\t  ","");
    ASSERT_EQ(" \tcoffee\tall\t  ",trimmed);
}

TEST(UStringTest,trim_tIsNormalString_TrimSuccess)
{
  string trimmed = trim("{quote}hunter{quote}","{quote}");
  ASSERT_EQ("hunter",trimmed);
}

TEST(UStringTest,trim_tIsLongerThanS_NotTrimmed)
{
  string trimmed = trim("apple","sixnum");
  ASSERT_EQ("apple",trimmed);
}

TEST(UStringTest,trim_sIsEmpty_tIsNotEmpty_ReturnsEmptyString)
{
  ASSERT_EQ("",trim("","Wispering Forest"));
}

TEST(UStringTest,trim_w_Emptywstring_ReturnsEmptywstring)
{
    wstring trimmed = trim(L"");
    ASSERT_EQ(L"",trimmed);
}

TEST(UStringTest,trim_w_Normalwstring_ReturnsNormalwstring)
{
    wstring trimmed = trim(L"vodoo");
    ASSERT_EQ(L"vodoo",trimmed);
}

TEST(UStringTest,trim_w_NormalwstringWithPaddingSpace_TrimSuccess)
{
    wstring trimmed = trim(L"  glamorous sky ");
    ASSERT_EQ(L"glamorous sky",trimmed);
}

TEST(UStringTest,trim_w_NoTrimChars_ReturnInputwstring)
{
    wstring trimmed = trim(L" \tcoffee\tall\t  ",L"");
    ASSERT_EQ(L" \tcoffee\tall\t  ",trimmed);
}

TEST(UStringTest,trim_w_tIsNormalString_TrimSuccess)
{
  wstring trimmed = trim(L"{quote}hunter{quote}",L"{quote}");
  ASSERT_EQ(L"hunter",trimmed);
}

TEST(UStringTest,trim_w_CheckTrimLeftFirst)
{
  wstring trimmed = trim(L"appappa",L"appa");
  ASSERT_EQ(L"ppa",trimmed);
}

TEST(UStringTest,trim_w_tIsLongerThanS_NotTrimmed)
{
  wstring trimmed = trim(L"apple",L"sixnum");
  ASSERT_EQ(L"apple",trimmed);
}

TEST(UStringTest,join_Empty_ReturnEmptyString)
{
    vector<string> ss;
    
    ASSERT_TRUE(join(ss,"l").empty());
}

TEST(UStringTest,join_w_Empty_ReturnsEmptyString)
{
    vector<wstring> ss;
    ASSERT_TRUE(join(ss,L";").empty());
}

TEST(UStringTest,trim_w_sIsEmpty_tIsNotEmpty_ReturnsEmptyString)
{
  ASSERT_EQ(L"",trim(L"",L"Wispering Forest"));
}


TEST(UStringTest,join_OneString_JoinSuccess)
{
    vector<string> ss;
    ss.push_back("asd");
    ASSERT_EQ("asd",join(ss,","));
}

TEST(UStringTest,join_w_OneString_JoinSuccess)
{
    vector<wstring> ss;
    ss.push_back(L"asd");
    ASSERT_EQ(L"asd",join(ss,L","));
}

TEST(UStringTest,join_TwoStrings_JoinSuccess)
{
    vector<string> ss;
    ss.push_back("asd");
    ss.push_back("ko");
    ASSERT_EQ("asd,ko",join(ss,","));
}

TEST(UStringTest,join_w_TwoStrings_JoinSuccess)
{
    vector<wstring> ss;
    ss.push_back(L"asd");
    ss.push_back(L"ko");
    ASSERT_EQ(L"asd ko",join(ss,L" "));
}

TEST(UStringTest,starts_with_NormalUsage)
{
    EXPECT_TRUE(starts_with(L"hello world!",L"hello"));
    EXPECT_TRUE(starts_with("hello world!","hello "));

    EXPECT_FALSE(starts_with(L"hello world!",L"ello"));
    EXPECT_FALSE(starts_with("hello world!","h8ello "));
}

TEST(UStringTest,starts_with_CaseInsensitive_Works)
{
    EXPECT_TRUE(starts_with(L"hello world!",L"Hello",CaseInsensitive));
    EXPECT_TRUE(starts_with("hello world!","heLLo W",CaseInsensitive));

    EXPECT_FALSE(starts_with(L"hello world!",L"Ello",CaseInsensitive));
    EXPECT_FALSE(starts_with("hello world!","h8Ello ",CaseInsensitive));
}

TEST(UStringTest,contains_CaseSensitive_Works)
{
    EXPECT_TRUE(contains(L"hello world!",L"world"));
    EXPECT_TRUE(contains("hello world!"," "));

    EXPECT_FALSE(contains(L"hello world!",L"ellao"));
    EXPECT_FALSE(contains("hello world!","  orld"));
}

TEST(UStringTest,constains_CaseInsensitive_Works)
{
    EXPECT_TRUE(contains(L"hello world!",L"wORld",CaseInsensitive));
    EXPECT_TRUE(contains("hello world!","HELLO",CaseInsensitive));

    EXPECT_FALSE(contains(L"hello world!",L"EllAo",CaseInsensitive));
    EXPECT_FALSE(contains("hello world!","  orLd",CaseInsensitive));
}

TEST(UStringTest,to_lower_Works)
{
	EXPECT_EQ(L"acb",to_lower(L"ACB"));
	EXPECT_EQ("acb",to_lower("AcB"));
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

TEST(UStringTest,to_upper_Works)
{
	EXPECT_EQ(L"ACB",to_upper(L"AcB"));
	EXPECT_EQ("ACB",to_upper("acB"));
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}