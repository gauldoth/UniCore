#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include "../UniCore/UProcess.h"
#include "../UniCore/UCommon.h"

using namespace uni;

TEST(UMiscTest,LCIDToRFC1766_SystemDefaultLCID_ReturnsNonEmptyString)
{
    std::wstring langName = LCIDToRFC1766(GetSystemDefaultLCID());
    ASSERT_FALSE(langName.empty());
}

TEST(UMiscTest,LCIDToRFC1766_InvalidLCID_ReturnsEmptyString)
{
    std::wstring langName = LCIDToRFC1766(999);
    ASSERT_TRUE(langName.empty());
}

TEST(UMiscTest,LCIDToRFC1766_LCID_es)
{
    int lcids[] = {0x580a,0x080a,0x4c0a,0x180a,0x3c0a,0x280a,0x500a,0x540a,0x380a,0x200a,0x140a,0x1c0a,0x300a,
        0x440a,0x100a,0x480a,0x0c0a,0x040a,0x2c0a,0x400a,0x340a,0x240a,0};
    int i = 0;
    while(lcids[i])
    {
        wchar_t buf[255] = L"";
        swprintf_s(buf,255,L"%04X",lcids[i]);
        EXPECT_EQ(L"es",LCIDToRFC1766(lcids[i]))<<buf;
        i++;
    }
}

TEST(UMiscTest,GenerateUUID_NotEmpty)
{
    std::wstring guid = GenerateUUID();
    ASSERT_TRUE(!guid.empty());
}

TEST(UMiscTest,GenerateUUID_NotRepeat)
{
    std::wstring uuidA = GenerateUUID();
    std::wstring uuidB = GenerateUUID();
    ASSERT_NE(uuidA,uuidB);
}