#include "stdafx.h"

#include <fstream>
#include "gtest/gtest.h"
#include "../UniCore/UConfig.h"

using namespace std;
using namespace uni;

class UIniConfigTest : public ::testing::Test
{
public:
    virtual void SetUp() 
    {
        iniConfig = new UIniConfig(L"./config.ini");
        UConfig::create(iniConfig);
    }

    virtual void TearDown() 
    {
        UConfig::destroy();
        ::_wremove(L"./config.ini");
    }
    UIniConfig *iniConfig;
};

class URegConfigTest : public ::testing::Test
{

};

TEST_F(UIniConfigTest,get_KeyNotExists_ReturnsEmptyString)
{
    ASSERT_TRUE(theConfig.get(L"testkey").empty());
}

TEST_F(UIniConfigTest,get_KeyExists_ReturnsSavedString)
{
    theConfig.set(L"testkey",L"test");
    ASSERT_EQ(L"test",theConfig.get(L"testkey"));
}

TEST_F(UIniConfigTest,IniFileCreateTest)
{
    theConfig.set(L"create",L"");
    ifstream file(L"./config.ini");
    bool isOpen = file.is_open();
    ASSERT_TRUE(isOpen);
}

TEST_F(UIniConfigTest,getInt_KeyNoExists_ReturnsZero)
{
    ASSERT_EQ(0,theConfig.getInt(L"tea"));
}