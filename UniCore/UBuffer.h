/*! \file UBuffer.h
    \brief 简单的Buffer类。

    \author uni
    \date 2012-3-12
*/
#ifndef UNICORE_UBUFFER_H
#define UNICORE_UBUFFER_H

#include <string>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

namespace uni
{

//! 缓冲区简单抽象。
/*!
    
*/
class UBuffer
{
public:
    UBuffer() {}
    virtual ~UBuffer() {}
    const char *data() const {return buf_.c_str();}
    int size() const {return buf_.size();}
    void appendChar(char data) {buf_.push_back(data);}
    //! 保存short数据到缓冲区中.
    void appendShort(short data)
    {
        buf_.append((char *)&data,sizeof(data));
    }
    //! 保存int数据到缓冲区中.
    void appendInt(int data)
    {
        buf_.append((char *)&data,sizeof(data));
    }
    //! 追加string数据。
    /*!
        \param data 要添加到缓冲区的数据的起始地址,中间可以包含0.
        \param size 数据的长度.
    */
    void appendString(const char *data, int size)
    {
        buf_.append(data,size);
    }
    //! 从由16进制数字组成的字符串,追加16进制数据。
    /*!
        \param pattern 16进制数字组成的字符串.例如"0C 0B 25 34 BF"这样的由16进制数字组成的字符串.

        appendHexPattern会分析pattern字符串,提取数据存放到buffer中.
        任何在0-9,a-f,A-F之外的字符都会被当作分隔符处理.
        取到两个字符,例如"2C",则把2C保存到Buffer中.
        假如取不够两个字符(已经到结尾,或者下个字符是分隔符),则在左边补"0",例如"C"会被作为"0C"处理.
        遇到分隔符则把分隔符之前的16进制数字(无论是一个还是两个字符)保存到Buffer中.
    */
    void appendHexPattern(const std::string &pattern);
private:
    std::string buf_;
};

}//namespace uni

#endif//UNICORE_UBUFFER_H