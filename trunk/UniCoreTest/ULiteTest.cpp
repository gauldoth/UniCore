#include "ULiteTest.h"

#include "../UniCore/ULite.h"
#include <string>

void ULiteTest()
{
    DebugMessage("ULite %s","Test");
    DebugMessage(L"ULite %d",25);
    std::string str = ws2s(L"string text");
    std::wstring wstr = s2ws("string text");
}

