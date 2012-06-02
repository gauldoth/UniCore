﻿/*! \file UBuffer.h
    \brief 简单的Buffer类。

    \author uni
    \date 2012-3-12
*/
#ifndef UNICORE_UBUFFER_H
#define UNICORE_UBUFFER_H

#include <string>

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
    void appendShort(short data)
    {
        buf_.append((char *)&data,sizeof(data));
    }
    void appendInt(int data)
    {
        buf_.append((char *)&data,sizeof(data));
    }
    //! 追加字符串。
    void appendString(const std::string &data)
    {
        buf_.append(data);
    }
    //! 追加16进制数据，这里以字符串方式添加。
    void appendHexPattern(const std::string &pattern);
private:
    std::string buf_;
};

}//namespace uni

#endif//UNICORE_UBUFFER_H