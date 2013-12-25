#include "stdafx.h"

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include "../UniCore/UProcess.h"

using namespace uni;

TEST(UProcessTest,DirectoryExists_RootDirectory_ReturnsTrue)
{
  ASSERT_TRUE(DirectoryExists(L"C:"));
}

TEST(UProcessTest,DirectoryExists_SystemDirectory_ReturnsTrue)
{
  ASSERT_TRUE(DirectoryExists(L"C:\\Windows\\System32"));
}

TEST(UProcessTest,DirectoryExists_FictionalDirectory_ReturnsFalse)
{
  ASSERT_FALSE(DirectoryExists(L"C:\\Program Files\\Foxit Software\\Foxit PhantomPDF\\sss"));
}

TEST(UProcessTest,DirectoryExists_DirectoryWithBackSlash_ReturnsTrue)
{
  ASSERT_TRUE(DirectoryExists(L"C:\\Windows\\System32\\"));
}

TEST(UProcessTest,DirectoryExists_FilePath_ReturnsFalse)
{
  ASSERT_FALSE(DirectoryExists(L"C:\\Windows\\System32\\ntdll.dll"));
}