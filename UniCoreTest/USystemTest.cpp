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

TEST(USystemTests, GetCurrentDirectory_ReturnsNonEmptyString)
{
    ASSERT_TRUE(!GetCurrentDir().empty());
}

TEST(USystemTest,DirectoryExists_RootDirectory_ReturnsTrue)
{
	ASSERT_TRUE(DirectoryExists(L"C:"));
}

TEST(USystemTest,DirectoryExists_SystemDirectory_ReturnsTrue)
{
	ASSERT_TRUE(DirectoryExists(L"C:\\Windows\\System32"));
}

TEST(USystemTest,DirectoryExists_FictionalDirectory_ReturnsFalse)
{
	ASSERT_FALSE(DirectoryExists(L"C:\\Program Files\\Foxit Software\\Foxit PhantomPDF\\sss"));
}

TEST(USystemTest,DirectoryExists_DirectoryWithBackSlash_ReturnsTrue)
{
	ASSERT_TRUE(DirectoryExists(L"C:\\Windows\\System32\\"));
}

TEST(USystemTest,DirectoryExists_FilePath_ReturnsFalse)
{
	ASSERT_FALSE(DirectoryExists(L"C:\\Windows\\System32\\ntdll.dll"));
}