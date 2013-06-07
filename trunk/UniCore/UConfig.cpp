#include "UConfig.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "UCast.h"
#include "ULog.h"

using namespace std;

namespace uni
{

UConfig::UConfig( const std::string &filePath )
:filePath_(filePath)
{

}

void UConfig::set( std::string key,std::string value,...)
{
    va_list ap;
    va_start(ap,value);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(key.c_str(),loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,key.c_str(),loc,ap);
    _free_locale(loc);
    va_end(ap);

    string formattedKey = buffer;

    delete[] buffer;

    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = formattedKey.find('/');
    if(slashIndex == string::npos)
    {
        //没有找到'/'，sectionName为general。
        sectionName = "general";
        keyName = formattedKey;
    }
    else if(slashIndex == formattedKey.size()-1 || slashIndex == 0)
    {
        //假如第一个'/'出现在key的第一个字符或最后一个字符，则认为格式错误。
        UERROR("UniCore")<<"UConfig::set 键名格式错误，'/'不能为key的第一个或最后一个字符。";
        return;
    }
    else
    {
        sectionName = formattedKey.substr(0,slashIndex);
        keyName = formattedKey.substr(slashIndex+1);
    }

    if(!WritePrivateProfileStringA(sectionName.c_str(),keyName.c_str(),value.c_str(),filePath_.c_str()))
    {
        UERROR("UniCore")<<"WritePrivateProfileStringA失败。"<<lasterr;
    }
}

std::string UConfig::get( std::string key,...)
{
    va_list ap;
    va_start(ap,key);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(key.c_str(),loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,key.c_str(),loc,ap);
    _free_locale(loc);
    va_end(ap);

    string formattedKey = buffer;

    delete[] buffer;

    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = formattedKey.find('/');
    if(slashIndex == string::npos)
    {
        //没有找到'/'，sectionName为general。
        sectionName = "general";
        keyName = formattedKey;
    }
    else if(slashIndex == formattedKey.size()-1 || slashIndex == 0)
    {
        //假如第一个'/'出现在key的第一个字符或最后一个字符，则认为格式错误。
        UERROR("UniCore")<<"UConfig::set 键名格式错误，'/'不能为key的第一个或最后一个字符。";
        return string();
    }
    else
    {
        sectionName = formattedKey.substr(0,slashIndex);
        keyName = formattedKey.substr(slashIndex+1);
    }

    const int maxResultSize = 512;
    char result[maxResultSize] = "";
    GetPrivateProfileStringA(sectionName.c_str(),keyName.c_str(),"",result,maxResultSize,filePath_.c_str());
    return result;
}

void UConfig::setInt( std::string key,int value ,...)
{
    va_list ap;
    va_start(ap,value);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(key.c_str(),loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,key.c_str(),loc,ap);
    _free_locale(loc);
    va_end(ap);

    string formattedKey = buffer;

    delete[] buffer;

    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = formattedKey.find('/');
    if(slashIndex == string::npos)
    {
        //没有找到'/'，sectionName为general。
        sectionName = "general";
        keyName = formattedKey;
    }
    else if(slashIndex == formattedKey.size()-1 || slashIndex == 0)
    {
        //假如第一个'/'出现在key的第一个字符或最后一个字符，则认为格式错误。
        UERROR("UniCore")<<"UConfig::set 键名格式错误，'/'不能为key的第一个或最后一个字符。";
        return;
    }
    else
    {
        sectionName = formattedKey.substr(0,slashIndex);
        keyName = formattedKey.substr(slashIndex+1);
    }

    string strValue = i2s(value);
    if(!WritePrivateProfileStringA(sectionName.c_str(),keyName.c_str(),strValue.c_str(),filePath_.c_str()))
    {
        UERROR("UniCore")<<"WritePrivateProfileStringA失败。"<<lasterr;
    }
}

int UConfig::getInt( std::string key,... )
{
    va_list ap;
    va_start(ap,key);
    //该函数返回格式化后的字符串长度，不包括0结束符。
    _locale_t loc = _create_locale(LC_ALL,"");
    const int len = _vscprintf_p_l(key.c_str(),loc,ap)+1;
    char *buffer = new char[len];

    _vsprintf_p_l(buffer,len,key.c_str(),loc,ap);
    _free_locale(loc);
    va_end(ap);

    string formattedKey = buffer;

    delete[] buffer;

    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = formattedKey.find('/');
    if(slashIndex == string::npos)
    {
        //没有找到'/'，sectionName为general。
        sectionName = "general";
        keyName = formattedKey;
    }
    else if(slashIndex == formattedKey.size()-1 || slashIndex == 0)
    {
        //假如第一个'/'出现在key的第一个字符或最后一个字符，则认为格式错误。
        UERROR("UniCore")<<"UConfig::set 键名格式错误，'/'不能为key的第一个或最后一个字符。";
        return 0;
    }
    else
    {
        sectionName = formattedKey.substr(0,slashIndex);
        keyName = formattedKey.substr(slashIndex+1);
    }

    const int maxResultSize = 512;
    char result[maxResultSize] = "";
    GetPrivateProfileStringA(sectionName.c_str(),keyName.c_str(),"",result,maxResultSize,filePath_.c_str());
    return atoi(result);
}

}//namespace uni