#include "stdafx.h"

#include "gtest/gtest.h"

#include "..\UniCore\USystem.h"

TEST(USystemTest,GetDirectoryPart_ValidPath_ReturnsDirectoryPart)
{
  ASSERT_EQ(L"C:\\temp\\",GetDirectoryPart(L"C:\\temp\\a.txt"));
}

TEST(USystemTest,GetDirectoryPart_EmptyPath_ReturnsEmptyString)
{
  ASSERT_EQ(L"",GetDirectoryPart(L""));
}

TEST(USystemTest,GetFilePart_ValidPath_ReturnsFilePart)
{
  ASSERT_EQ(L"a.txt",GetFilePart(L"C:\\temp\\a.txt"));
}

TEST(USystemTest,GetFilePart_EmptyPath_ReturnsEmptyString)
{
  ASSERT_EQ(L"",GetFilePart(L""));
}