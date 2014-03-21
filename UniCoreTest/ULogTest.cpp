#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include "../UniCore/ULog.h"
#include "../UniCore/UMiniLog.h"

using namespace std;
using namespace uni;

class MockAppender : public ULog::Appender
{
public:
    MockAppender() :appendCount_(0){}
    virtual void append(ULog::Message *message)
    {
        message_ = message->stm_.str();
        appendCount_ ++;
    }
    std::string message_;
    int appendCount_;
};

class StubAppender : public ULog::Appender
{
public:
    virtual void append(ULog::Message *message)
    {
        
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
}

TEST_F(ULogTest,setAppender_ChangeExisting_ChangeSuccessfully)
{
    ULog::setAppenders("abc","debug console");
    vector<string> appenders = ULog::getAppenders("abc");
    ASSERT_EQ(2,appenders.size());
    EXPECT_EQ("debug",appenders[0]);
    EXPECT_EQ("console",appenders[1]);

    ULog::setAppenders("abc","file");
    appenders = ULog::getAppenders("abc");
    ASSERT_EQ(1,appenders.size());
    EXPECT_EQ("file",appenders[0]);
}

TEST_F(ULogTest,registerAppender_StubAppender_Success)
{
    ULog::Appender *appender = new StubAppender;
    ULog::registerAppender("stub",appender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(1,registeredAppenderNames.size());
    EXPECT_EQ("stub",registeredAppenderNames[0]);
}

TEST_F(ULogTest,registerAppender_RegisterAppenderWithoutName_RegisterFailed)
{
    ULog::registerAppender("",new StubAppender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(0,registeredAppenderNames.size());
}

TEST_F(ULogTest,registerAppender_AppenderWithInvalidName_RegisterFailed)
{
    ULog::registerAppender("1h0 ",new StubAppender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(0,registeredAppenderNames.size());
}

TEST_F(ULogTest,getRegisteredAppenderNames_TwoAppenderExists_ReturnTwoNames)
{
    ULog::registerAppender("stub1",new StubAppender);
    ULog::registerAppender("stub2",new StubAppender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(2,registeredAppenderNames.size());
    EXPECT_EQ("stub1",registeredAppenderNames[0]);
    EXPECT_EQ("stub2",registeredAppenderNames[1]);
}

TEST_F(ULogTest,unregisterAppender_AppenderNameExists_UnregisterSuccess)
{
    ULog::registerAppender("stub",new StubAppender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(1,registeredAppenderNames.size());
    EXPECT_EQ("stub",registeredAppenderNames[0]);

    ULog::unregisterAppender("stub");
    registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(0,registeredAppenderNames.size());
}

TEST_F(ULogTest,unregisterAllAppenders_TwoAppenderRegistered_UnregisterSuccess)
{
    ULog::registerAppender("stub1",new StubAppender);
    ULog::registerAppender("stub2",new StubAppender);
    vector<string> registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(2,registeredAppenderNames.size());
    EXPECT_EQ("stub1",registeredAppenderNames[0]);
    EXPECT_EQ("stub2",registeredAppenderNames[1]);

    ULog::unregisterAllAppenders();
    registeredAppenderNames = 
        ULog::getRegisteredAppenderNames();
    ASSERT_EQ(0,registeredAppenderNames.size());
}



TEST_F(ULogTest,NormalLogging_NormalString_OutputSuccess)
{
    using ::testing::_;

   MockAppender *appender = new MockAppender; 
    ULog::registerAppender("mock",appender);
    ULog::setAppenders("","mock");
    
    UDEBUG<<"abc";
    ASSERT_EQ("abc",appender->message_);
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

TEST_F(ULogTest,streamInBool_true_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    UINFO<<true;
    ASSERT_EQ("1",mockAppender->message_);
}

TEST_F(ULogTest,streamInBool_false_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    UINFO<<false;
    ASSERT_EQ("0",mockAppender->message_);
}

TEST_F(ULogTest,streamInShort_NormalShortNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    short shortNumber = 155;
    UINFO<<shortNumber;
    ASSERT_EQ("155",mockAppender->message_);
}

TEST_F(ULogTest,streamInUnsignedShort_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    unsigned short shortNumber = 65535;
    UINFO<<shortNumber;
    ASSERT_EQ("65535",mockAppender->message_);
}

TEST_F(ULogTest,streamInInt_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    int number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInUnsignedInt_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    unsigned int number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInLong_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    long number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInUnsignedLong_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    unsigned long number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInLongLong_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    long long number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInUnsignedLongLong_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    unsigned long long number = 100;
    UINFO<<number;
    ASSERT_EQ("100",mockAppender->message_);
}

TEST_F(ULogTest,streamInFloat_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    float number = 6.02e32f;
    UINFO<<number;
    ASSERT_EQ("6.02e+032",mockAppender->message_);
}

TEST_F(ULogTest,streamInDouble_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    double number = 6.02e32;
    UINFO<<number;
    ASSERT_EQ("6.02e+032",mockAppender->message_);
}

TEST_F(ULogTest,streamInVoidPtr_NormalNumber_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    void * number = reinterpret_cast<void *>(0xFAC2);
    UINFO<<number;
    ASSERT_EQ("0000FAC2",mockAppender->message_);
}

TEST_F(ULogTest,streamInChar_NormalChar_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    char c = 'x';
    UINFO<<c;
    ASSERT_EQ("x",mockAppender->message_);
}

TEST_F(ULogTest,streamInCStr_NormalCStr_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    char * s = "identity";
    UINFO<<s;
    ASSERT_EQ("identity",mockAppender->message_);
}

TEST_F(ULogTest,streamInCStr_Zero_OutputNull)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    char * s = 0;
    UINFO<<s;
    ASSERT_EQ("(null)",mockAppender->message_);
}

#if _NATIVE_WCHAR_T_DEFINED
TEST_F(ULogTest,streamInWChar_NormalWChar_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    wchar_t c = L'x';
    UINFO<<c;
    ASSERT_EQ("x",mockAppender->message_);
}
#endif//_NATIVE_WCHAR_T_DEFINED

#if _NATIVE_WCHAR_T_DEFINED
TEST_F(ULogTest,streamInWCharPtr_NormalString_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    wchar_t *s = L"tal rasha";
    UINFO<<s;
    ASSERT_EQ("tal rasha",mockAppender->message_);
}
#endif

TEST_F(ULogTest,streamInString_NormalString_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    string s = "leoric";
    UINFO<<s;
    ASSERT_EQ("leoric",mockAppender->message_);
}

TEST_F(ULogTest,streamInWString_NormalString_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    wstring s = L"leoric";
    UINFO<<s;
    ASSERT_EQ("leoric",mockAppender->message_);
}

TEST_F(ULogTest,delim_SpaceDelim_ThreeInput_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    UINFO<<delim<<200<<"abc"<<L'C';
    ASSERT_EQ("200 abc C",mockAppender->message_);
}

TEST_F(ULogTest,delim_SemicolonDelim_ZeroInput_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    UINFO<<delim(";");
    ASSERT_EQ("",mockAppender->message_);
}

TEST_F(ULogTest,delim_EmptyStringDelim_TwoInput_DataValid)
{
    MockAppender *mockAppender = new MockAppender;
    ULog::registerAppender("mock",mockAppender);
    ULog::setAppenders("","mock");
    UINFO<<delim("")<<"butter"<<"fly";
    ASSERT_EQ("butterfly",mockAppender->message_);
}