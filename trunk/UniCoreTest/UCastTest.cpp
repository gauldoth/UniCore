#include "stdafx.h"

#include "gtest/gtest.h"
#include <string>
#include "../UniCore/UCast.h"

using namespace std;
using namespace uni;

TEST(UCastTest,ws2s_AsciiString_ConvertSucceeded)
{
    std::string s = ws2s(L"apple");
    ASSERT_EQ("apple",s);
}

TEST(UCastTest,ws2s_NonAsciiString_ConvertSucceeded)
{
    std::string s = ws2s(L"我们",".936");
    ASSERT_EQ("\xCE\xD2\xC3\xC7",s);
}

TEST(UCastTest,ws2s_EmptyString_ReturnsEmptyString)
{
    std::string s = ws2s(L"");
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_MultiLanguage_ReturnEmptyString)
{
    std::string s = ws2s(L"我们じゃない향찰/鄕札");
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_ContainsZero_ConvertUntilZero)
{
    std::wstring ws;
    
    ws.assign(L"Contains\0Zero",sizeof(L"Contains\0Zero")/sizeof(wchar_t));
    std::string s = ws2s(ws);
    ASSERT_EQ("Contains",s);
}

TEST(UCastTest,ws2s_LocaleInvalid_ReturnsEmptyString)
{
    std::string s = ws2s(L"won khim lau","won khim lau");
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_locale_t_AsciiString_ConvertSucceeded)
{
    _locale_t loc = _create_locale(LC_CTYPE,"");
    EXPECT_TRUE(loc);
    std::string s = ws2s(L"apple",loc);
    ASSERT_EQ("apple",s);
}

TEST(UCastTest,ws2s_locale_t_NonAsciiString_ConvertSucceeded)
{
    _locale_t loc = _create_locale(LC_CTYPE,".936");
    EXPECT_TRUE(loc);
    std::string s = ws2s(L"我们",loc);
    ASSERT_EQ("\xCE\xD2\xC3\xC7",s);
}

TEST(UCastTest,ws2s_locale_t_EmptyString_ReturnsEmptyString)
{
    _locale_t loc = _create_locale(LC_CTYPE,".936");
    EXPECT_TRUE(loc);
    std::string s = ws2s(L"",loc);
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_locale_t_MultiLanguage_ReturnsEmptyString)
{
    _locale_t loc = _create_locale(LC_CTYPE,"");
    EXPECT_TRUE(loc);
    std::string s = ws2s(L"我们じゃない향찰/鄕札",loc);
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_locale_t_LocaleInvalid_ReturnsEmptyString)
{
    _locale_t loc = _create_locale(LC_CTYPE,"maximus");
    std::string s = ws2s(L"我们",loc);
    ASSERT_TRUE(s.empty());
}


TEST(UCastTest,ws2s_codepage_AsciiString_ConvertSuccessfully)
{
    std::string s = ws2s(L"ascii",CP_ACP);
    ASSERT_EQ("ascii",s);
}

TEST(UCastTest,ws2s_codepage_NonAsciiString_ConvertSuccessful)
{
    std::string s = ws2s(L"我们",".936");
    
    ASSERT_EQ("\xCE\xD2\xC3\xC7",s);
}

TEST(UCastTest,ws2s_codepage_EmptyString_ReturnsEmptyString)
{
    std::string s = ws2s(L"",CP_ACP);
    ASSERT_TRUE(s.empty());
}

TEST(UCastTest,ws2s_codepage_MultiLanguage_NonUtf8CodePage_ReturnsStringContainingQuestionMark)
{
    //codepage 936 does not contain 향찰.
    std::string s = ws2s(L"我们じゃない향찰/鄕札",936);  
    ASSERT_EQ("我们じゃない??/鄕札",s)<<s;
}

TEST(UCastTest,ws2s_codepage_MultiLanguage_Utf8CodePage_ReturnsStringContainingQuestionMark)
{
    //codepage 936 does not contain 향찰.
    std::string s = ws2s(L"我们じゃない향찰/鄕札",CP_UTF8);  
    //我们じゃない향찰/鄕札
    ASSERT_EQ("\xe6\x88\x91\xe4\xbb\xac\xe3\x81\x98\xe3\x82\x83\xe3\x81\xaa\xe3\x81\x84\xed\x96\xa5\xec\xb0\xb0\x2f\xe9\x84\x95\xe6\x9c\xad",s)<<s;
}

TEST(UCastTest,ws2s_codepage_CodepageInvalid_ReturnsEmptyString)
{
    std::string s = ws2s(L"我们",10086);
    ASSERT_TRUE(s.empty())<<s;
}

TEST(UCastTest,ws2s_CStyle_AsciiString_ConvertSuccessfully)
{
    char buf[1024] = "";
    ws2s(buf,1024,L"eagle",".936");
    ASSERT_STRCASEEQ("eagle",buf);
}

TEST(UCastTest,ws2s_CStyle_NonAsciiStrinig_ConvertSuccessfully)
{
    char buf[1024] = "";
    ws2s(buf,1024,L"我们",".936");
    ASSERT_STRCASEEQ("\xCE\xD2\xC3\xC7",buf);
}

TEST(UCastTest,ws2s_CStyle_EmptyString_ReturnsEmptyString)
{
    char buf[1024] = "";
    ws2s(buf,1024,L"","");
    ASSERT_EQ(0,strlen(buf));
}

TEST(UCastTest,ws2s_CStyle_MultiLanguage_ReturnsEmptyString)
{
    char buf[1024] = "";
    ws2s(buf,1024,L"我们じゃない향찰/鄕札","");
    ASSERT_EQ(0,strlen(buf));
}

TEST(UCastTest,ws2s_CStyle_CodePageInvalid_ReturnsEmptyString)
{
    char buf[1024] = "";
    ws2s(buf,1024,L"我们","rondal's locket");
    ASSERT_EQ(0,strlen(buf));
}

TEST(UCastTest,ws2s_CStyle_destAndLenEqualsToZero_NoThrow)
{
    ASSERT_NO_THROW(ws2s(0,0,L"asd",""));
}

TEST(UCastTest,ws2s_CStyle_destAndLenEqualsToZero_InvalidLocale_NoThrow)
{
    ASSERT_NO_THROW(ws2s(0,0,L"asd","throw away"));
}

TEST(UCastTest,ws2s_CStyle_sourceEqualsToZero_NoThrow)
{
    char buf[1] = "";
    ASSERT_NO_THROW(ws2s(buf,1,0,""));
}

TEST(UCastTest,ws2s_CStyle_bufferTooSmall_ReturnsTruncatedString)
{
    char buf[2] = "";
    ws2s(buf,2,L"apple","");
    ASSERT_EQ('a',buf[0]);
    ASSERT_EQ(0,buf[1]);
}

TEST(UCastTest,s2ws_AsciiString_ConvertSuccessfully)
{
    std::wstring ws = s2ws("apple");
    ASSERT_EQ(L"apple",ws);
}

TEST(UCastTest,s2ws_NonAsciiString_ConvertSuccessfully)
{
    std::wstring ws = s2ws("\xCE\xD2\xC3\xC7");
    ASSERT_EQ(L"我们",ws);
}

TEST(UCastTest,s2ws_EmptyString_ReturnsEmptyString)
{
    std::wstring ws = s2ws("");
    ASSERT_TRUE(ws.empty());
}

TEST(UCastTest,s2ws_LocaleInvalid_ReturnsEmptyString)
{
    std::wstring ws = s2ws("我们","ourself");
    ASSERT_TRUE(ws.empty());
}

TEST(UCastTest,s2ws_locale_t_AsciiString_ConvertSuccessfully)
{
    _locale_t loc = _create_locale(LC_CTYPE,"");
    std::wstring ws = s2ws("apple",loc);
    ASSERT_EQ(L"apple",ws);
}

TEST(UCastTest,s2ws_locale_t_NonAsciiString_ConvertSuccessfully)
{
    _locale_t loc = _create_locale(LC_CTYPE,".936");
    std::wstring ws = s2ws("\xCE\xD2\xC3\xC7",loc);
    ASSERT_EQ(L"我们",ws);
}

TEST(UCastTest,s2ws_locale_t_LocaleInvalid_ReturnsEmptyString)
{
    _locale_t loc = _create_locale(LC_CTYPE,"crypt of the antient");
    ASSERT_FALSE(loc)<<loc;
    std::wstring ws = s2ws("apple",loc);
    ASSERT_TRUE(ws.empty());
}

TEST(UCastTest,s2ws_codepage_AsciiString_ConvertSuccessfully)
{
    std::wstring ws = s2ws("apple",CP_ACP);
    ASSERT_EQ(L"apple",ws);
}

TEST(UCastTest,s2ws_codepage_NonAsciiString_ConvertSuccessfully)
{
    //"\xCE\xD2\xC3\xC7" is "我们" in 936 codepage
    std::wstring ws = s2ws("\xCE\xD2\xC3\xC7",936);
    ASSERT_EQ(L"我们",ws);
}

TEST(UCastTest,s2ws_codepage_CodePageInvalid_ReturnsEmptyString)
{
    std::wstring ws = s2ws("apple",10086);
    ASSERT_TRUE(ws.empty())<<"ws:"<<ws;
}

TEST(UCastTest,s2ws_codepage_Utf8String_CodePageIsUtf8_ConvertSuccessfully)
{
    std::wstring ws = s2ws("\xe6\x88\x91\xe4\xbb\xac\xe3\x81\x98\xe3\x82\x83\xe3\x81\xaa\xe3\x81\x84\xed\x96\xa5\xec\xb0\xb0\x2f\xe9\x84\x95\xe6\x9c\xad",CP_UTF8);
    ws = L"我们じゃない향찰/鄕札";
    ASSERT_EQ(L"我们じゃない향찰/鄕札",ws);
}

TEST(UCastTest,s2ws_CStyle_AsciiString_ConvertSuccessfully)
{
    wchar_t buf[1024] = L"";
    s2ws(buf,1024,"apple","");
    ASSERT_STREQ(L"apple",buf);
}

TEST(UCastTest,s2ws_CStyle_NonAsciiString_ConvertSuccessfully)
{
    wchar_t buf[1024] = L"";
    s2ws(buf,1024,"\xCE\xD2\xC3\xC7","");
    ASSERT_STREQ(L"我们",buf);
}

TEST(UCastTest,s2ws_CStyle_CodePageInvalid_ReturnsEmptyString)
{
    wchar_t buf[1024] = L"";
    s2ws(buf,1024,"\xCE\xD2\xC3\xC7","invalid codepage");
    ASSERT_STREQ(L"",buf);
}

TEST(UCastTest,s2ws_CStyle_destEqualsToZero_NoThrow)
{
    ASSERT_NO_THROW(s2ws(0,1024,"\xCE\xD2\xC3\xC7",""));
}

TEST(UCastTest,s2ws_CStyle_lenEqualsToZero_NoThrow)
{
    wchar_t buf[1024] = L"";
    ASSERT_NO_THROW(s2ws(buf,0,"\xCE\xD2\xC3\xC7",""));
}

TEST(UCastTest,s2ws_CStyle_BufferTooSmall_Truncated)
{
    wchar_t buf[1] = L"";
    s2ws(buf,1,"apple","");
    ASSERT_EQ(0,buf[0]);
}

TEST(UCastTest,i2s_0_Returns0)
{
    ASSERT_EQ("0",i2s(0));
}

TEST(UCastTest,i2s_minusOne_ReturnsMinusOne)
{
    ASSERT_EQ("-1",i2s(-1));
}

TEST(UCastTest,i2s_1_Returns1)
{
    ASSERT_EQ("1",i2s(1));
}

TEST(UCastTest,i2s_MaxPositive_ReturnsMaxPositive)
{
    ASSERT_EQ("2147483647",i2s(2147483647));
}

TEST(UCastTest,i2s_MinNegative_ReturnsMinNegative)
{
    ASSERT_EQ("-2147483648",i2s(-2147483647-1));
}

TEST(UCastTest,stringOperatorAddInt_NormalInt_ConcatenateSuccess)
{
    std::string s;
    ASSERT_EQ("24",s+24);
}

TEST(UCastTest,stringOperatorAddInt_NegativeInt_ConcatenateSuccess)
{
    std::string s = "asd";
    int number = -99;
    ASSERT_EQ("asd-99",s+number);
}
