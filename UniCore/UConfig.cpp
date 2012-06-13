#include "UConfig.h"

using namespace std;

namespace uni
{

UConfig::UConfig( const std::string &filePath )
:filePath_(filePath)
{

}

void UConfig::set( const std::string &key,const std::string &value )
{
    string sec;  //写配置档时的section name。  
    size_t slashIndex = key.find('/');
    if(slashIndex != string::npos)
    {
        sec = key.substr(0,slashIndex);
        key.erase(0,slashIndex);
    }
    if(sec.empty())
    {
        sec = "general";
    }

    if(!WritePrivateProfileStringA(sec.c_str(),key.c_str(),value,filePath_))
    {
        UERROR("UniCore")<<"WritePrivateProfileStringA失败。"<<lasterr;
    }
}

std::string UConfig::get( const std::string &key )
{
    string sec;  //写配置档时的section name。  
    size_t slashIndex = key.find('/');
    if(slashIndex != string::npos)
    {
        sec = key.substr(0,slashIndex);
        key.erase(0,slashIndex);
    }
    if(sec.empty())
    {
        sec = "general";
    }

    //if(!GetPrivateProfileString(sec.c_str(),key.c_str(),"",))
    return string();
}

}//namespace uni