#include "stdafx.h"

#include "gtest/gtest.h"
#include "../UniCore/UEnum.h"

enum RWBY
{
    Ross,
    Weiss,
    Yang,
    Blake,
};

UENUM(RWBY)
    UENUM_ENTRY(Ross)
    UENUM_ENTRY(Weiss)
    UENUM_ENTRY(Yang)
    UENUM_ENTRY(Blake)
UENUM_END

enum Fruit
{
    Apple,
    Banana,
};

UENUM(Fruit)
    UENUM_ENTRY(Apple)
    UENUM_ENTRY(Banana)
UENUM_END

TEST(UEnumTest,EnumToString_NonIntrusive_ConvertSuccess)
{
    RWBY rwby;
    rwby = Ross;
    EXPECT_EQ(L"Ross",e2s(rwby));
    rwby = Weiss;
    EXPECT_EQ(L"Weiss",e2s(rwby));
    rwby = Yang;
    EXPECT_EQ(L"Yang",e2s(rwby));
    rwby = Blake;
    EXPECT_EQ(L"Blake",e2s(rwby));
}

TEST(UEnumTest,e2s_TwoEnums_ConvertSuccess)
{
    Fruit f;
    f = Apple;
    EXPECT_EQ(L"Apple",e2s(f));
}