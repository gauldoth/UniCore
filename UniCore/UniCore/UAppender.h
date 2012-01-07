#ifndef UNICORE_UAPPENDER_H
#define UNICORE_UAPPENDER_H

#include <string>
#include <fstream>

namespace uni
{

//! UAppender为输出源的抽象类，用于ULog输出。
class UAppender
{
public:
    UAppender() {}
    virtual ~UAppender() {}
    //! 继承XLogAppender并实现该函数来自定义输出方式。
    /*!
        \param msg 待输出的日志信息。
    */
    virtual void append(const std::string &msg) = 0;
private:
    UAppender(const UAppender &);
    UAppender &operator=(const UAppender &);
};

//! UDebuggerAppender输出日志信息到调试器。
class UDebuggerAppender : public UAppender
{
public:
    UDebuggerAppender() {}
    virtual ~UDebuggerAppender() {}
    //! 使用OutputDebugMessageA输出。
    virtual void append(const std::string &msg);
private:
    UDebuggerAppender(const UDebuggerAppender &);
    UDebuggerAppender &operator=(const UDebuggerAppender &);
};

//! XLogConsoleAppender输出日志信息到控制台。
class UConsoleAppender : public UAppender
{
public:
    UConsoleAppender() {}
    virtual ~UConsoleAppender() {}
    //! 使用printf输出。
    virtual void append(const std::string &msg);
private:
    UConsoleAppender(const UConsoleAppender &);
    UConsoleAppender &operator=(const UConsoleAppender &);
};

//! XLogFileAppender输出日志信息到文件。
class UFileAppender : public UAppender
{
public:
    //! 构造函数。
    /*!
        \param file 文件地址。
        为XLog添加一个文件输出源。
        \code
        XLog::setAppender("file",new UFileAppender("C:\\a.txt"));
        \endcode
    */
    UFileAppender(const std::string &file);
    virtual ~UFileAppender();
    virtual void append(const std::string &msg);
private:
    UFileAppender(const UFileAppender&);
    UFileAppender &operator=(const UFileAppender &);
    std::ofstream ftm_;
};

}//namespace uni

#endif//UNICORE_UAPPENDER_H