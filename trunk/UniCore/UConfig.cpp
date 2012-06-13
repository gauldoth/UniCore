#include "UConfig.h"

using namespace std;

namespace uni
{

UConfig *UConfig::instance_ = 0;

UConfig::UConfig()
{

}

void UConfig::setFile( const std::string &filePath )
{

}

void UConfig::set( const std::string &key,const std::string &value )
{

}

std::string UConfig::get( const std::string &key )
{
    return string();
}

UConfig & UConfig::instance()
{
    if(!instance_)
    {
        instance_ = new UConfig;
    }
    return *instance_;
}

}//namespace uni