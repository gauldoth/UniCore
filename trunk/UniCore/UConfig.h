#ifndef UNICORE_UCONFIG_H
#define UNICORE_UCONFIG_H

#include <string>

#define theConfig uni::UConfig::instance()

namespace uni
{

//! 配置类。
/*!
    用于读写配置档。
    
    \code
    theConfig.setFile("./配置.lua");
    theConfig.bind(accountEdit,"账号");
    \endcode
*/
class UConfig
{
public:
    static UConfig &instance();
    void setFile(const std::string &filePath);
    void set(const std::string &key,const std::string &value);
    std::string get(const std::string &key);
protected:
    UConfig();
private:
    static UConfig *instance_;
};

}//namespace uni

#endif//UNICORE_UCONFIG_H