#include "stdafx.h"

#include "gtest/gtest.h"
#include "../UniCore/UDebug.h"
#include "../UniCore/USharedMemory.h"

using namespace uni;

TEST(UIntegrationTest,DebugMessage_ValidCStr_Works)
{
    ASSERT_NO_THROW(DebugMessage("DebugMessage Works"));
}

//TEST(UIntegrationTest,USharedMemory_BasicUsage)
//{
//    {
//        //Client
//        USharedMemory sharedMemory;
//        sharedMemory.setData(L"command",L"beep");
//        sharedMemory.setData(L"frequency",1000);
//        sharedMemory.setData(L"duration",2000);
//        sharedMemory.NotifyEvent(L"new command");
//        sharedMemory.WaitEvent(L"command finished");
//    }
//    {
//        //Server
//        USharedMemory sharedMemory;
//        sharedMemory.WaitEvent(L"new command");
//        std::wstring command = sharedMemory.data(L"command");
//        int frequency = sharedMemory.intData("command");
//        int duration = sharedMemory.intData("duration");
//        sharedMemory.NotifyEvent(L"command finished");
//    }
//}