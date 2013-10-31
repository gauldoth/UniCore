/*! \file UConfig.h
\brief  用于保存程序配置.

\author  uni
\date  2013-10-31
*/
#ifndef UNICORE_UCONFIG_H
#define UNICORE_UCONFIG_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#include <string>
#include <vector>
#include <cassert>

#define theConfig UConfig::instance()

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

\todo Add a method: UConfig::setArray
\todo Add a method: UConfig::getArray
\todo Change UConfig to a abstract base class.
*/
class UConfig
{
public:
    static void create(UConfig *config)
    {
        assert(!instance_ && "instance_ already exists, could not create more.");
        if(!instance_)
        {
            instance_ = config;
        }
    }
    static void destroy()
    {
        if(instance_)
        {
            delete instance_;
            instance_ = 0;
        }
    }
    static UConfig &instance()
    {
        assert(instance_ && "Must call UConfig::create before using theConfig.");
        return *instance_;
    }
    virtual ~UConfig() {}
    //! 从配置档读取数据。
    /*!
    \param key 数据的键名，格式如下："通用配置/账号"，"网络/服务器/1/IP"。
    各个子项以'/'分隔。这个参数可以是类似printf函数中的格式字符串，可变参在
    最后指定。这里不传引用，因为有用到可变参。
    \param ... 可变参，用于key中的格式字符串。
    \return 取出的数据。
    */
    virtual std::wstring get(std::wstring key, ...) = 0;
    //! 向配置档写入数据。
    /*!
    \param key 数据的键名，格式如下："通用配置/账号"，"网络/服务器/1/IP"。
    各个子项以'/'分隔。这个参数可以是类似printf函数中的格式字符串，可变参在
    最后指定。这里不传引用，因为有用到可变参。
    \param value 要写入的数据。
    \param ... 可变参，用于key中的格式字符串。
    */
    virtual void set(std::wstring key,
        std::wstring value, ...) = 0;
    virtual std::vector<std::wstring> getArray(std::wstring key, ...) = 0;
    virtual void setArray(std::wstring key,
        std::vector<std::wstring> value, ...) = 0;
    //! 从配置档读取整型数据.
    /*!
    \param key 数据的键名,参考UConfig键名格式.
    \param ... 可变参,用于key中的格式字符串.
    \return 取出的数据.
    */
    virtual int getInt(std::wstring key, ...) = 0;
    //! 向配置档写入整型.
    /*!
    \param key 数据的键名.
    \param value 要写入的数据.
    \param ... 可变参,用于key中的格式字符串.
    */
    virtual void setInt(std::wstring key, int value, ...) = 0;
protected:
    UConfig() {}
    static UConfig *instance_;
private:
    UConfig(const UConfig &);
    UConfig &operator = (const UConfig &);
};

class UIniConfig : public UConfig
{
public:
    //! 构造函数。
    /*!
    \param filePath 配置文件路径。
    */
    UIniConfig(const std::wstring &iniPath)
        :iniPath_(iniPath)
    {

    }
    virtual std::wstring get(std::wstring key,...);

    virtual void set( std::wstring key, std::wstring value, ... );

    virtual std::vector<std::wstring> getArray( std::wstring key, ... );

    virtual void setArray( std::wstring key, std::vector<std::wstring> value, ... );

    virtual int getInt( std::wstring key, ... );

    virtual void setInt( std::wstring key, int value, ... );

private:
    UIniConfig(const UIniConfig &);
    UIniConfig &operator = (const UIniConfig &);
    std::wstring get(std::wstring key,va_list ap);
    bool splitSectionAndKey( std::wstring &formattedKey, std::wstring &sectionName, std::wstring &keyName );
    std::wstring formatKey( std::wstring &key, va_list ap);

    std::wstring iniPath_;
};

}//namespace uni

#endif//UNICORE_UCONFIG_H