#include "UConfig.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

using namespace std;

namespace uni
{

UConfig *UConfig::instance_ = 0;

std::wstring UIniConfig::get( std::wstring key,... )
{
    va_list ap;
    va_start(ap,key);
    std::wstring result =  get(key,ap);
    va_end(ap);

    return result;
}

std::wstring UIniConfig::get(std::wstring key,va_list ap)
{
    std::wstring formattedKey = formatKey(key,ap);
    wstring sectionName;  //写配置档时的section name。 
    wstring keyName;
    if(!splitSectionAndKey(formattedKey, sectionName, keyName))
    {
        return L"";
    }
    const int maxResultSize = 512;
    wchar_t result[maxResultSize] = L"";
    GetPrivateProfileStringW(sectionName.c_str(),keyName.c_str(),L"",result,maxResultSize,iniPath_.c_str());
    return result;
}

void UIniConfig::set(std::wstring key,std::wstring value,va_list ap)
{
    std::wstring formattedKey = formatKey(key,ap);
    wstring sectionName;  //写配置档时的section name。 
    wstring keyName;
    if(!splitSectionAndKey(formattedKey, sectionName, keyName))
    {
        return;
    }

    if(!WritePrivateProfileStringW(sectionName.c_str(),keyName.c_str(),value.c_str(),iniPath_.c_str()))
    {
        assert(!"WritePrivateProfileStringW失败。");
    }
}

bool UIniConfig::splitSectionAndKey( std::wstring &formattedKey, wstring &sectionName, wstring &keyName )
{
    size_t slashIndex = formattedKey.find('/');
    if(slashIndex == wstring::npos)
    {
        //没有找到'/'，sectionName为general。
        sectionName = L"";
        keyName = formattedKey;
    }
    else if(slashIndex == formattedKey.size()-1 || slashIndex == 0)
    {
        //假如第一个'/'出现在key的第一个字符或最后一个字符，则认为格式错误。
        assert(!"UConfig::set 键名格式错误，'/'不能为key的第一个或最后一个字符。");
        return false;
    }
    else
    {
        sectionName = formattedKey.substr(0,slashIndex);
        keyName = formattedKey.substr(slashIndex+1);
    }

    return true;
}

std::wstring UIniConfig::formatKey( std::wstring &key, va_list ap )
{
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscwprintf_p_l(key.c_str(),loc,ap)+1;
    wchar_t *buffer = new wchar_t[len];

    _vswprintf_p_l(buffer,len,key.c_str(),loc,ap);
    _free_locale(loc);

    wstring result = buffer;

    delete[] buffer;
    return result;
}

void UIniConfig::set( std::wstring key, std::wstring value, ... )
{
    va_list ap;
    va_start(ap,value);
    set(key,value,ap);
    va_end(ap);
}

std::vector<std::wstring> UIniConfig::getArray( std::wstring key, ... )
{
    throw std::exception("The method or operation is not implemented.");
    return std::vector<std::wstring>();
}

void UIniConfig::setArray( std::wstring key, std::vector<std::wstring> value, ... )
{
    throw std::exception("The method or operation is not implemented.");
}

void UIniConfig::setInt( std::wstring key, int value, ... )
{
    va_list ap;
    va_start(ap,value);
    wchar_t buf[255] = L"";
    _itow_s(value,buf,10);
    set(key,buf,ap);
    va_end(ap);
}

int UIniConfig::getInt( std::wstring key, ... )
{
    va_list ap;
    va_start(ap,key);
    std::wstring result =  get(key,ap);
    va_end(ap);

    return _wtoi(result.c_str());
}

}//namespace uni