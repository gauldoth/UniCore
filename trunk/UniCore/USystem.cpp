#include "USystem.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

std::wstring GetTempPathW()
{
  const int bufSize = MAX_PATH+1;
  wchar_t buf[bufSize] = L"";
  if(GetTempPathW(bufSize,buf))
  {
    return buf;
  }
  else
  {
    return std::wstring();
  }
}

