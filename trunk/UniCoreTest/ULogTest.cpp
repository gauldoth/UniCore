#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include "../UniCore/ULog.h"

using namespace std;
using namespace uni;

class MockAppender : public ULog::Appender
{
public:
    MOCK_METHOD1(append, void(ULog::Message *message));

};

class StubAppender : public ULog::Appender
{
public:
    virtual void append(ULog::Message *message)
    {
        // Do nothing.
    }
};

class ULogTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        // Clear ULog's static state.

    }
    virtual void TearDown()
    {
        // Clear ULog's static state.
        ULog::restoreDefaultSettings();
    }
};

TEST(ULogMessageTest,Ctor_Valid)
{
    ULog::Message message(ULog::DebugType,"file",56,"TestFunction");
    EXPECT_EQ(1,message.ref_);
    EXPECT_EQ(ULog::DebugType,message.type_);
    EXPECT_EQ("",message.name_);
    EXPECT_EQ("file",message.file_);
    EXPECT_EQ("TestFunction",message.func_);
    EXPECT_EQ(56,message.line_);
    EXPECT_TRUE(message.delim_.empty());
    EXPECT_TRUE(message.delimEnabled_);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(ULogTest,setAppender_GlobalLoggerAndTwoAppender_SetSuccessful)
{
    ULog::setAppenders("","debug console");
    vector<string> appenders = ULog::getAppenders("");
    EXPECT_EQ("debug",appenders[0]);
    EXPECT_EQ("console",appenders[1]);
    EXPECT_EQ(2,appenders.size());
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

bool CheckMessage(ULog::Message *message)
{
    if(message->stm_.str() == "abc")
    {
        return true;
    }
    else
    {
        return false;
    }
}

TEST_F(ULogTest,NormalLogging_NormalString_OutputSuccess)
{
    using ::testing::_;

   MockAppender *appender = new MockAppender; 
    ULog::registerAppender("mock",appender);
    ULog::setAppenders("","mock");
    
    EXPECT_CALL(*appender,append(_)).Times(1);
        UDEBUG<<"abc";
    ULog::unregisterAppender("mock");
}

TEST_F(ULogTest,dumpmem_MemoryDataWithCR_DataValid)
{
    char data[] = {'\r'};
    ULog log(ULog::InfoType,"",0,"");
    log<<dumpmem(data,sizeof(data));
    string message = log.message();

    EXPECT_TRUE(message.find("|| 0D") != string::npos);
    EXPECT_TRUE(message.find("|| \\r") != string::npos);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(ULogTest,dumpmem_MemoryDataWithLF_DataValid)
{
    char data[] = {'\n'};
    ULog log(ULog::InfoType,"",0,"");
    log<<dumpmem(data,sizeof(data));
    string message = log.message();

    EXPECT_TRUE(message.find("|| 0A") != string::npos);
    EXPECT_TRUE(message.find("|| \\n") != string::npos);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(ULogTest,dumpmem_MemoryDataWithZero_DataValid)
{
    char data[] = {'\0'};
    ULog log(ULog::InfoType,"",0,"");
    log<<dumpmem(data,sizeof(data));
    string message = log.message();

    EXPECT_TRUE(message.find("|| 00") != string::npos);
    EXPECT_TRUE(message.find("|| \\0") != string::npos);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(ULogTest,dumpmem_AddressEqualsZero_AccessViolation)
{
    const char *address = 0;
    ULog log(ULog::InfoType,"",0,"");
    ASSERT_ANY_THROW(log<<dumpmem(address,10));
}

TEST_F(ULogTest,dumpmem_SizeEqualsZero_OutputOnlyAddressAndSize)
{
    char data[] = {'a'};
    ULog log(ULog::InfoType,"",0,"");
    log<<dumpmem(data,0);
    string message = log.message();

    EXPECT_TRUE(message.find("|") == string::npos);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(ULogTest,dumpmem_NegativeValue_DataValid)
{
    char data[] = {-1};
    ULog log(ULog::InfoType,"",0,"");
    log<<dumpmem(data,sizeof(data));
    string message = log.message();

    EXPECT_TRUE(message.find("  .") != string::npos);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}