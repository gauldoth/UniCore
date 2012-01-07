/*! \file ULog.h
    \brief 日志相关。

    \author     uni
    \date       2011-7-15
*/
#ifndef UNICORE_ULOG_H
#define UNICORE_ULOG_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <assert.h>
#include <errno.h>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>

namespace uni
{

class UAppender;


//! ULog用于输出调试信息。
/*!
    ULog采用流的方式输出日志信息，并且兼容标准库中流支持的类型和操纵符。
    实际使用时请使用UTRACE，XDBG等宏来输出日志信息，使用宏还可以使用类似printf的方式输出日志。

    \section ulog_type_sec 日志类型和日志级别
    日志共有6种类型，Trace为最低级别，往下级别越来越高：
        - \b Trace 最精细的调试信息，多用于定位错误，查看某些变量的值。
        - \b Debug 普通的调试信息，这类信息发布时一般不输出。
        - \b Info 表示程序进程的信息，例如"开始打怪"，"开始出售物品"之类。
        - \b Warn 反映某些需要注意的可能有潜在危险的情况，可能会造成崩溃或逻辑错误之类。
          例如取内存值时捕捉到异常，这情况很常见，允许程序继续执行，但仍需注意。
        - \b Error 反映错误，例如一些API的调用失败。
        - \b Fatal 致命错误，程序无法继续执行。

    我们通过设置日志级别来禁止某些日志的输出，日志级别有8种：
        - \b AllLevel   所有的日志都输出。
        - \b TraceLevel Trace及比Trace等级高的日志信息都会输出。
        - \b DebugLevel Debug及比Debug等级高的日志信息都会输出。
        - \b InfoLevel  Info及比Info等级高的日志信息都会输出。
        - \b WarnLevel  Warn及比Warn等级高的日志信息都会输出。
        - \b ErrorLevel Error及比Error等级高的日志信息都会输出。
        - \b FatalLevel 只有Fatal信息会输出。
        - \b OffLevel   所有日志信息都不输出。

    有两种方法设置日志级别：
        - 动态设置日志级别，通过这种方式屏蔽的日志信息仍会被编到可执行文件中，
          可能造成信息暴露。通过调用静态函数 ULog::setLogLevel() 进行设置。
        - 静态设置日志级别，通过这种方式屏蔽的日志信息不会被编到可执行文件中，
          更加安全。通过定义以下宏以静态设置日志级别：
            - UNI_LOG_LEVEL_ALL
            - UNI_LOG_LEVEL_TRACE
            - UNI_LOG_LEVEL_DEBUG
            - UNI_LOG_LEVEL_INFO
            - UNI_LOG_LEVEL_WARN
            - UNI_LOG_LEVEL_FATAL
            - UNI_LOG_LEVEL_OFF

    \section usage_sec 基本使用方法
    基本的使用方法有两种：
        -# 使用定义好的宏输出日志信息，对应6种日志类型，有以下6个宏可以使用：
            - UTRACE
            - UDEBUG
            - UINFO
            - UWARN
            - UERROR
            - UFATAL
            .
           可以采用格式化输出，也可以采用流的方式输出：
            - 格式化输出，使用和printf类似。
            \code
            char *name = "哥布林";
            UINFO("找到一个单位。");  //char *版本。
            UDEBUG("攻击单位:%s",name);  //char *版本，格式化。
            UTRACE(L"攻击单位结束。");  //wchar_t *版本。
            \endcode
            - 流方式输出，使用和标准库中流类似。
            \code
            char *name = "哥布林";
            UINFO<<"找到一个单位。";
            UDEBUG<<"攻击单位:"<<name;
            UTRACE<<L"攻击单位结束。";
            \endcode
            .
        -# 直接使用ULog类（不常用，请咨询作者）。
          \code
          ULog log = ULog();  //创建一个日志对象，类型为默认类型（ULog::Info）。
          int a = 2;
          char *buf = "some info";
          log<<a<<buf;  //向log输入日志信息。日志在log对象的所有引用都消失时才会输出。
          \endcode

    \section appender_sec 添加输出源
    ULog可以输出到调试器，文件，控制台等，这里把这些输出方式撑做输出源。
    可以使用静态函数 ULog::setAppender(const std::string &name,UAppender *appender) 设置或添加输出源。
    ULog默认使用一个名为"default"的 UDebuggerAppender 将日志信息输出到调试器。
    本库还提供了以下几种输出源：
        - UDebuggerAppender 类，用于输出日志信息到调试器。
        - UConsoleAppender 类，用于输出日志信息到控制台。
        - UFileAppender 类，用于输出日志信息到文件。
    使用方法：
    \code
    ULog::setAppender("default",new UFileAppender("C:\\a.txt"));  //将默认的输出源改为一个文件输出源。
    ULog::setAppender("console",new UConsoleAppender());  //为ULog添加一个输出源，名字为"console"，实际输出到控制台。
    \endcode

    \section layout_sec 日志的输出格式
    ULog可以自定义各种类型日志的输出格式，例如UTRACE可以输出具体行和函数，而UINFO则只输出信息。
    默认情况下会以"[%t] %m"的格式输出，可以使用静态函数 ULog::setLayout 修改任意类型的输出格式，看下面例子：
    \code 
    UTRACE<<"abc";  //使用默认的"[%t] %m"输出，其中%t被替换为日志类型，%m替换为日志信息，结果为"[trace] abc"。
    ULog::setLayout(ULog::TraceType,"[%t]%m - %f %l");  
    UTRACE<<"abc";  //输出"[trace]abc - d:\myproject\test\testasm\testasm\testasm.cpp 62"。
    ULog::setLayout(ULog::DebugType,"[%t]%m - %n");
    UDEBUG<<"abc";  //输出"[debug]abc - main"。
    \endcode
    ULog::setLayout 支持下面几种转义符：
    - %%t 日志类型。
    - %%m 日志的信息主体部分。
    - %%n 输出日志语句所在函数。
    - %%l 输出日志语句所在行数。
    - %%f 输出日志语句所属源文件。

    \section manipulator_sec 操纵符
    ULog支持streamstring所支持的所有操纵符。此外，还支持以下操纵符。
    - \b lasterr 输出GetLastError返回的错误信息。
      \code
      HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,2718);
      if(!hProcess)
      {
          UERROR<<"OpenProcess失败。"<<lasterr;  
          //输出"[error]OpenProcess失败。{ 错误号：183 错误描述：当文件已存在时，无法创建该文件。}"
      }
      \endcode
    - \b delim 设置分隔符。例如，
      \code
      int a = 1;
      int b = 2;
      int c = 3;
      //分别输出a,b,c的值。
      UTRACE<<delim<<a<<b<<c;  //delim不带参数时，以空格分隔，输出的信息为"1 2 3"。
      TTRACE<<delim(",")<<a<<b<<c;  //delim带参数时，以参数字符串分隔，输出的信息为"1,2,3"。
      \endcode

    \section expand_sec 扩展ULog支持的类型
    ULog可以支持其他类型，只需要对该类型实现<<操纵符即可。例如以下代码实现对结构体POINT的支持：
    \code
    ULog operator<<(ULog log,POINT pt)
    {
        //组织信息，不添加分隔符，这句只能使用ULog内部支持的类型。
        log.noDelim()<<"(POINT:x="<<pt.x<<",y="<<pt.y<<")";  
        return log.hasDelim();  
    }
    \endcode

*/
class ULog
{
    struct Stream;
public:
    //! 日志类型。
    enum Type
    {
        TraceType = 100,    //!< 最精细的调试信息。
        DebugType = 200,    //!< 普通的调试信息。
        InfoType = 300,     //!< 这类信息表明了程序的大体流程。
        WarnType = 400,     //!< 警告信息，描述了可能有害的情况。
        ErrorType = 500,    //!< 错误，但仍然允许程序继续运行。例如调用API失败之类的错误。
        FatalType = 600,    //!< 致命错误，会导致程序终止。
    };

    //! 日志级别。
    /*!
        日志类型高于或等于日志级别的日志才允许被输出。
    */
    enum LogLevel
    {
        AllLevel = 0,       //!< 所有级别的日志都允许输出。
        TraceLevel = 100,   //!< Trace级别及更高级别的日志允许输出。
        DebugLevel = 200,   //!< Debug级别及更高级别的日志允许输出。
        InfoLevel = 300,    //!< Info级别及更高级别的日志允许输出。
        WarnLevel = 400,    //!< Warn级别及更高级别的日志允许输出。
        ErrorLevel = 500,   //!< Error级别及更高级别的日志允许输出。
        FatalLevel = 600,   //!< Fatal级别及更高级别的日志允许输出。
        OffLevel = 700,     //!< 全部日志都不允许输出。
    };

    //! 无参数的操纵符。
    typedef ULog &(__cdecl *Manipulator)(ULog &);

    //! 一个参数的操纵符。
    template<typename ArgType>
    struct SManipulator
    {
        SManipulator(void (__cdecl *pfunc)(ULog &,ArgType),ArgType arg)
            :pfunc_(pfunc),arg_(arg)
        {
        }
        void (__cdecl *pfunc_)(ULog &,ArgType);
        ArgType arg_;
    };

    //! 给定日志类型，构造一个日志对象。
    /*!
        \param type 日志类型。
    */
    explicit ULog(Type type = InfoType);

    //! 拷贝构造函数。
    ULog(const ULog &log);

    ULog &operator=(const ULog &log);

    //! 析构函数。
    /*!
        假如内部流的引用变为0，则输出日志信息。
    */
    virtual ~ULog();

    //! 将当前对象和log对象互换。
    void swap(ULog &log);

    //! 设置动态日志级别。
    /*!
        类型大于等于日志级别的日志才会被输出。
        \param level 要设置的日志级别。
    */
    static void setLogLevel(LogLevel level);

    //! 获得当前的动态日志级别。
    static LogLevel logLevel();

    //! 增加一个输出源。
    /*!
        ULog默认使用一个名为"default"的输出源输出信息到调试器，调用该函数以增加其他输出途径。
        \param name 输出源名字。
        \param appender 输出源指针。
    */
    static void setAppender(const std::string &name,UAppender *appender);

    //! 增加一个输出源。
    /*!
        ULog默认使用一个名为"default"的输出源输出信息到调试器，调用该函数以增加其他输出途径。
        \param name 输出源名字。
        \param appender 输出源指针。
    */
    static void setAppender(const std::wstring &name,UAppender *appender);

    //! 移除所有的输出源。
    static void removeAllAppenders();

    //! 移除单个输出源。
    /*!
        默认情况下ULog有一个名字为"default"的输出源。
        \param name 要移除的输出源的名字。
    */
    static void removeAppender(const std::string &name);

    //! 移除单个输出源。
    /*!
        默认情况下ULog有一个名字为"default"的输出源。
        \param name 要移除的输出源的名字。
    */
    static void removeAppender(const std::wstring &name);

    //! 设置输出信息的格式。
    /*!
        指定一个格式字符串，以设置对应日志类型的输出格式。可用的转义符号有以下几种：
            - %%t 日志类型。
            - %%m 日志的信息主体部分。
            - %%n 输出日志语句所在函数。
            - %%l 输出日志语句所在行数。
            - %%f 输出日志语句所属源文件。
        \param type 日志类型。
        \param format 日志类型对应的输出格式。
    */
    static void setLayout(ULog::Type type,const std::string &format);

    //! 设置输出信息的格式。
    /*!
        指定一个格式字符串，以设置对应日志类型的输出格式。可用的转义符号有以下几种：
            - %%t 日志类型。
            - %%m 日志的信息主体部分。
            - %%n 输出日志语句所在函数。
            - %%l 输出日志语句所在行数。
            - %%f 输出日志语句所属源文件。
        \param type 日志类型。
        \param format 日志类型对应的输出格式。
    */
    static void setLayout(ULog::Type type,const std::wstring &format);

    //! 返回当前ULog使用的locale，默认情况是使用本机区域设置。
    static _locale_t locale();

    //! 接受ostream的操纵符。
    /*!
        \param ostreamManipulator ostream操纵符，例如endl。
    */
    ULog &operator<<(std::ostream &(*ostreamManipulator)(std::ostream &));

    //! 接受basic_ios操纵符。
    ULog &operator<<(std::basic_ios<char> &(*basicIosManipulator)(std::basic_ios<char> &));

    //! 接受ios_base操纵符，例如hex，showbase等。
    ULog &operator<<(std::ios_base &(*iosBaseManipulator)(std::ios_base &));

    //! 接受setfill操纵符。
    ULog &operator<<(const std::_Fillobj<char>& _Manip);

    //! 接受一元操纵符，如setw，setprecision之类。
    template<typename _Arg>
    ULog &operator<<(const std::_Smanip<_Arg>& _Manip)
    {
        stream_->stm_<<_Manip;
        return (*this);
    }

    ULog &hasDelim() 
    {
        stream_->outputDelim_ = true; 
        if(!stream_->delim_.empty() && stream_->outputDelim_)
        {
            stream_->stm_<<stream_->delim_;
        }
        return *this;
    }

    ULog &noDelim() {stream_->outputDelim_ = false; return *this;}

    ULog &mayHasDelim() {if(!stream_->delim_.empty() && stream_->outputDelim_) {stream_->stm_<<stream_->delim_;} return *this;}

	ULog &operator<<(bool t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(short t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned short t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(int t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned int t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(long t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned long t) {stream_->stm_<<t; return mayHasDelim();}

 #ifdef _LONGLONG
	ULog &operator<<(_LONGLONG t) {stream_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(_ULONGLONG t) {stream_->stm_<<t; return mayHasDelim();}
 #endif /* _LONGLONG */

	ULog &operator<<(float t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(double t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(long double t) {stream_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(const void *t) {stream_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(std::basic_streambuf<char> *t) {stream_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(char t) {stream_->stm_<<t; return mayHasDelim();}
    
    ULog &operator<<(const char *t) {stream_->stm_<<t; return mayHasDelim();}

    //! 写入一个宽字符。
    ULog &operator<<(wchar_t t);

    //! 写入宽字符字串。
    ULog &operator<<(const wchar_t *t);

    ULog &operator<<(const std::string &t) {stream_->stm_<<t; return mayHasDelim();}

    //! 写入wstring。
    ULog &operator<<(const std::wstring &t);

    //! 接受ULog无参数操纵符。
    ULog &operator<<(Manipulator m)
    {
        return (*m)(*this);
    }

    //! 接受ULog一个参数操纵符。
    template<class ArgType>
    ULog &operator<<(SManipulator<ArgType> m)
    {
        m.pfunc_(*this,m.arg_);
        return (*this);
    }

    friend void ULogSetFile(ULog &log,const char *file);

    friend void ULogSetFunc(ULog &log,const char *func);

    friend void ULogSetLine(ULog &log,int line);

    friend ULog &lasterr(ULog &log);

    friend void ULogSetDelim(ULog &log,const char *delim);

    //! 该静态常量用于宏定义中代替while(false)，因为while(false)会产生一个警告。
    static const bool noWarnFalse_;
private:
    //! ULog内部用到的流结构，保存了日志信息的内容。
    struct Stream
    {
        Stream(Type type)
            :ref_(1),type_(type),line_(0),outputDelim_(true)
        {
        }
        int ref_;
        Type type_;
        std::string file_;
        std::string func_;
        int line_;
        std::string delim_;
        bool outputDelim_;
        std::ostringstream stm_;
    };
    unsigned long lastError_;
    Stream *stream_;
    struct Impl;
    static std::tr1::shared_ptr<Impl> pimpl_;
};

std::string ULogSprintf(const char *format,...);

std::wstring ULogSprintf(const wchar_t *format,...);

inline ULog &ULogSprintf(ULog &log)
{
    return log;
}

ULog::SManipulator<const char *> ULogSetFile(const char *file);

ULog::SManipulator<const char *> ULogSetFunc(const char *func);

ULog::SManipulator<int> ULogSetLine(int line);

ULog::SManipulator<const char *> delim(const char *delim);

inline ULog &delim(ULog &log)
{
    log<<delim(" ");
    return log;
}

ULog uLog(ULog::Type type = ULog::InfoType);

#define ULOG_NO_OUTPUT_MACRO while(ULog::noWarnFalse_) uLog()<<ULogSprintf

#if defined UNI_LOG_LEVEL_OFF       //无输出。
#define UNI_LOG_LEVEL 700
#elif defined UNI_LOG_LEVEL_FATAL   //严重的错误，导致程序终止。
#define UNI_LOG_LEVEL 600
#elif defined UNI_LOG_LEVEL_ERROR   //错误，允许程序继续运行。
#define UNI_LOG_LEVEL 500
#elif defined UNI_LOG_LEVEL_WARN    //警告，反应潜在的有害的状态。
#define UNI_LOG_LEVEL 400
#elif defined UNI_LOG_LEVEL_INFO    //表明程序进程。
#define UNI_LOG_LEVEL 300
#elif defined UNI_LOG_LEVEL_DEBUG   //调试信息。
#define UNI_LOG_LEVEL 200
#elif defined UNI_LOG_LEVEL_TRACE   //更精细的调试信息。
#define UNI_LOG_LEVEL 100
#elif defined UNI_LOG_LEVEL_ALL
#define UNI_LOG_LEVEL 0
#endif

#if UNI_LOG_LEVEL <= 100
#  define UTRACE uLog(ULog::TraceType)<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UTRACE ULOG_NO_OUTPUT_MACRO
#endif

#if UNI_LOG_LEVEL <= 200
#  define UDEBUG uLog(ULog::DebugType)<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UDEBUG ULOG_NO_OUTPUT_MACRO
#endif

#if UNI_LOG_LEVEL <= 300
#  define UINFO uLog()<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UINFO ULOG_NO_OUTPUT_MACRO
#endif

#if UNI_LOG_LEVEL <= 400
#  define UWARN uLog(ULog::WarnType)<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UWARN ULOG_NO_OUTPUT_MACRO
#endif

#if UNI_LOG_LEVEL <= 500
#  define UERROR uLog(ULog::ErrorType)<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UERROR ULOG_NO_OUTPUT_MACRO
#endif

#if UNI_LOG_LEVEL <= 600
#  define UFATAL uLog(ULog::FatalType)<<ULogSetFile(__FILE__)<<ULogSetFunc(__FUNCTION__)<<ULogSetLine(__LINE__)<<ULogSprintf
#else
#  define UFATAL ULOG_NO_OUTPUT_MACRO
#endif

}//namespace uni

#endif//UNICORE_ULOG_H