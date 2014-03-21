#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include "../UniCore/UMiniLog.h"

TEST(UMiniLogTest,Compile_OK)
{
    MINILOG<<"test";
}
