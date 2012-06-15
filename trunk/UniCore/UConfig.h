#ifndef UNICORE_UCONFIG_H
#define UNICORE_UCONFIG_H

#include <string>

namespace uni
{

//! 配置类。
/*!
    简单的读写配置档的类。
    \code
    UConfig config("./配置.ini");
    int index = 4;  //存储第4个账号。
    config.set("通用配置/账号/%d","flower",index); 
    //--------------------------------
    //配置.ini
    //--------------------------
    //[通用配置]
    //账号/4 = flower
    //--------------------------------
    string account = config.get("通用配置/账号/%d",index);
    //account中为"flower"。
    \endcode
*/
class UConfig
{
public:
    //! 构造函数。
    /*!
        \param filePath 配置文件路径。
    */
    explicit UConfig(const std::string &filePath);
    //! 向配置档写入数据。
    /*!
        \param key 数据的键名，格式如下："通用配置/账号"，"网络/服务器/1/IP"。
        各个子项以'/'分隔。这个参数可以是类似printf函数中的格式字符串，可变参在
        最后指定。
        \param value 要写入的数据。
        \param ... 可变参，用于key中的格式字符串。
    */
    void set(const std::string &key,const std::string &value,...);
    
    //! 从配置档读取数据。
    /*!
        \param key 数据的键名，格式如下："通用配置/账号"，"网络/服务器/1/IP"。
        各个子项以'/'分隔。这个参数可以是类似printf函数中的格式字符串，可变参在
        最后指定。
        \param ... 可变参，用于key中的格式字符串。
        \return 取出的数据。
    */
    std::string get(const std::string &key,...);
private:
    std::string filePath_;
};

}//namespace uni

#endif//UNICORE_UCONFIG_H