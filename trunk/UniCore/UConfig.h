#ifndef UNICORE_UCONFIG_H
#define UNICORE_UCONFIG_H

#include <string>

namespace uni
{

//! 配置类。
/*!
    用于读写配置档。

*/
class UConfig
{
public:
    //! 构造函数。
    /*!
        \param filePath 配置文件路径。
    */
    UConfig(const std::string &filePath);
    //! 向配置档写入数据。
    /*!
        \param key 数据的键名，由section name和key name组成。
        以'/'分隔，如："通用配置/账号"。其中"通用配置"是section name；
        "账号"是key name。
        \param value 要写入的数据。
    */
    void set(const std::string &key,const std::string &value);
    std::string get(const std::string &key);
private:
    std::string filePath_;
};

}//namespace uni

#endif//UNICORE_UCONFIG_H