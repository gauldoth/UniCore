#include "stdafx.h"

#include "gtest/gtest.h"
#include "../UniCore/UDebug.h"

using namespace uni;

TEST(UIntegrationTest,DebugMessage_ValidCStr_Works)
{
    ASSERT_NO_THROW(DebugMessage("DebugMessage Works"));
}