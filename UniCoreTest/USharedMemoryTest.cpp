#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../UniCore/USharedMemory.h"
#include "../UniCore/UDebug.h"

using namespace std;
using namespace uni;

class StubMemoryManager : public UMemoryManager
{
public:
    enum {BufSize = 30};
    StubMemoryManager() {memset(buf,0,sizeof(buf));}
    virtual __int64 allocMemory(int size)
    {
        __int64 address = (__int64)(new char[size]);
        DebugMessage("address newed: %llx",address);
        return address;
    }
    virtual bool freeMemory(__int64 address)
    {
        DebugMessage("deleted: %llx",address);
        delete [](char *)address;
        address = 0;
        return true;
    }
    virtual __int64 fixedMemory()
    {
        return (__int64)&buf[0];
    }
    virtual __int64 fixedMemorySize()
    {
        return sizeof(buf);
    }
    char buf[BufSize];
};

class USharedMemoryTest : public ::testing::Test
{

};

TEST_F(USharedMemoryTest,StubMemoryManager_NewDeleteWorked)
{
    StubMemoryManager memoryManager;
    memoryManager.allocMemory(15);
    memoryManager.allocMemory(24);
    __int64 address = memoryManager.allocMemory(5);
    memoryManager.freeMemory(address);
}

TEST_F(USharedMemoryTest,NewString_NormalString_ReturnsValidAddress)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    __int64 address = sharedMemory.newString(L"test");
    EXPECT_TRUE(address);
    EXPECT_EQ(4,*(__int32 *)address);
    wchar_t *stringDataWritten = (wchar_t *)(address+sizeof(__int32));
    EXPECT_EQ(L't',stringDataWritten[0]);
    EXPECT_EQ(L'e',stringDataWritten[1]);
    EXPECT_EQ(L's',stringDataWritten[2]);
    EXPECT_EQ(L't',stringDataWritten[3]);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(USharedMemoryTest,GetStringByAddress_CraftedString_ReturnsCraftedString)
{
    wchar_t buf[] = L"ABCD";
    *(__int32 *)buf = 2;
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    std::wstring result = sharedMemory.getStringByAddress((__int64)&buf[0]);
    ASSERT_EQ(L"CD",result);
}

TEST_F(USharedMemoryTest,SetData_NormalStringData_SetSuccess)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setData(L"test",L"data");
    ASSERT_EQ(L"data",sharedMemory.data(L"test"));
}

TEST_F(USharedMemoryTest,SetData_ModifyExitsingData_ModifySuccess)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setData(L"test",L"data");
    sharedMemory.setData(L"test",L"data modified");
    ASSERT_EQ(L"data modified",sharedMemory.data(L"test"));
}

TEST_F(USharedMemoryTest,SetIntData_NormalInt_SetSuccess)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setIntData(L"test",22);
    ASSERT_EQ(22,sharedMemory.intData(L"test"));
}

TEST_F(USharedMemoryTest,Data_NotSuchEntry_ReturnEmptyString)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setIntData(L"test",22);
    std::wstring data = sharedMemory.data(L"dataNotExists");
    ASSERT_TRUE(data.empty());
}

TEST_F(USharedMemoryTest,findPair_OnePairExists_FindSuccess)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setIntData(L"test",22);
    ASSERT_TRUE(sharedMemory.findPair(L"test"));
}

TEST_F(USharedMemoryTest,findPair_TwoPairExists_FindSuccess)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setIntData(L"test",22);
    sharedMemory.setData(L"我们",L"data中文");
    ASSERT_TRUE(sharedMemory.findPair(L"我们"));
}

TEST_F(USharedMemoryTest,findPair_FindAnEntryDoesNotExists_NoFind)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    sharedMemory.setIntData(L"test",22);
    sharedMemory.setData(L"我们",L"data中文");
    ASSERT_EQ(0,sharedMemory.findPair(L"q"));
}

TEST_F(USharedMemoryTest,getEvent_ReturnsValidEvent)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    EXPECT_TRUE(sharedMemory.getEvent(L"new command"));
    EXPECT_EQ(0,*sharedMemory.getEvent(L"new command"));
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(USharedMemoryTest,WaitEvent_WithNormalEventNameAndZeroTimeout_ReturnsFalse)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    bool waitSuccess = sharedMemory.WaitEvent(L"EventA",0);
    ASSERT_FALSE(waitSuccess);
}

TEST_F(USharedMemoryTest,NotifyEvent_NormalEventName_NoThrow)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    ASSERT_NO_THROW(sharedMemory.NotifyEvent(L"aevent"));
}

bool CanExit()
{
    return true;
}
TEST_F(USharedMemoryTest,WaitEvent_WithNormalEventNameAndPredicate_ReturnsFalse)
{
    StubMemoryManager stubMemoryManager;
    USharedMemory sharedMemory(stubMemoryManager);
    bool waitSuccess = sharedMemory.WaitEvent(L"EventA",CanExit);
    ASSERT_FALSE(waitSuccess);
}

class USharedMemoryManagerTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        sharedMemoryManager_ = new USharedMemoryManager;
    }
    virtual void TearDown()
    {
        delete sharedMemoryManager_;
        sharedMemoryManager_ = 0;
    }
    UMemoryManager *sharedMemoryManager_;
};

TEST_F(USharedMemoryManagerTest,allocMemory_ZeroSize_ReturnsZero)
{
    __int64 address = sharedMemoryManager_->allocMemory(0);
    ASSERT_EQ(0,address);
}

TEST_F(USharedMemoryManagerTest,allocMemory_PositiveSize_ReturnsValidAddress)
{
    __int64 address = sharedMemoryManager_->allocMemory(5);
    ASSERT_NO_THROW(*(char *)address = 0);
}

TEST_F(USharedMemoryManagerTest,allocMemory_NegativeSize_ReturnsValidAddress)
{
    __int64 address = sharedMemoryManager_->allocMemory(-1);
    ASSERT_EQ(0,address);
}

TEST_F(USharedMemoryManagerTest,fixedMemory_ReturnsNonZero)
{
    ASSERT_NE(0,sharedMemoryManager_->fixedMemory());
}

TEST_F(USharedMemoryManagerTest,fixedMemory_CanBeAccessed)
{
    EXPECT_NE(0,sharedMemoryManager_->fixedMemory());
    EXPECT_NE(0,sharedMemoryManager_->fixedMemorySize());
    char *data = reinterpret_cast<char *>(sharedMemoryManager_->fixedMemorySize());
    EXPECT_NO_THROW(data[0]);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(USharedMemoryManagerTest,fixedMemorySize_ReturnsNonZero)
{
    ASSERT_NE(0,sharedMemoryManager_->fixedMemorySize());
}

TEST_F(USharedMemoryManagerTest,fixedMemory_FirstCharIsZero)
{
    char *data = reinterpret_cast<char *>(sharedMemoryManager_->fixedMemory());
    __int64 dataSize = sharedMemoryManager_->fixedMemorySize();
    EXPECT_NO_THROW(data[0]);
    EXPECT_EQ(0,data[0]);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(USharedMemoryManagerTest,allocMemory_PositiveSize_FirstCharEqualsZero)
{
    char *data = reinterpret_cast<char *>(sharedMemoryManager_->allocMemory(5));
    EXPECT_NO_THROW({data[0] == 0;});
    EXPECT_EQ(0,data[0]);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(USharedMemoryManagerTest,freeMemory_freedSuccess)
{
    __int64 address = sharedMemoryManager_->allocMemory(543);
    ASSERT_TRUE(sharedMemoryManager_->freeMemory(address));
}

TEST_F(USharedMemoryManagerTest,allocMemory_SizeEqualsOne_DataIsZeroInitialized)
{
    char *data = reinterpret_cast<char *>(sharedMemoryManager_->allocMemory(1));
    ASSERT_EQ(0,data[0]);
}

TEST_F(USharedMemoryManagerTest,CTor_ConstructedObjectIsValid)
{
    ASSERT_TRUE(sharedMemoryManager_->isValid());
}