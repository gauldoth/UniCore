#include "UConfig.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "ULog.h"

using namespace std;

namespace uni
{

UConfig::UConfig( const std::string &filePath )
:filePath_(filePath)
{

}

void UConfig::set( const std::string &key,const std::string &value )
{
    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = key.find('/');
    if(slashIndex != string::npos)
    {
        sectionName = key.substr(0,slashIndex);
        keyName = key.substr(slashIndex+1);
    }
    if(sectionName.empty())
    {
        sectionName = "general";
    }
    if(keyName.empty())
    {
        keyName = key;
    }

    if(!WritePrivateProfileStringA(sectionName.c_str(),keyName.c_str(),value.c_str(),filePath_.c_str()))
    {
        UERROR("UniCore")<<"WritePrivateProfileStringA失败。"<<lasterr;
    }
}

std::string UConfig::get( const std::string &key )
{
    string sectionName;  //写配置档时的section name。 
    string keyName;
    size_t slashIndex = key.find('/');
    if(slashIndex != string::npos)
    {
        sectionName = key.substr(0,slashIndex);
        keyName = key.substr(slashIndex+1);
    }
    if(sectionName.empty())
    {
        sectionName = "general";
    }
    if(keyName.empty())
    {
        keyName = key;
    }

    //if(!GetPrivateProfileString(sec.c_str(),key.c_str(),"",))
    return string();
}

}//namespace uni