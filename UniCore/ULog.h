/*! \file ULog.h
    \brief 日志相关。

    \author     uni
    \date       2011-7-15
*/
#ifndef UNICORE_ULOG_H
#define UNICORE_ULOG_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "windowsx.h"

#include <assert.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <vector>

#include "UDebug.h"
#include "ULock.h"
#include "UCast.h"

namespace uni
{

/*! \page ulog_page 日志系统说明
    ULog是一个简单的日志输出模块,用于弥补 DebugMessage() 功能上的不足.主要有以下特点:
        - 采用流的方式输出日志信息，并且兼容标准库中输入输出流支持的类型和操纵符。
        - 能够自定义输出源.
        - 日志按名字分组,并且有调试,警告,错误等多种日志类型.
        - 能够在编译时去除指定类型的日志的输出,提高代码效率,也能防止信息暴露.
        - 能够在运行时指定哪些分组,哪些类型的日志不输出.

    \section ulog_type_sec 日志类型
    日志共有以下几种类型:
        - \b Trace 最精细的调试信息，多用于定位错误，或者监视某些变量的值.
        - \b Debug 普通的调试信息，这类信息发布时一般不输出.
        - \b Info 表示程序当前状况的信息，例如"开始加载资源"，"开始出售物品"之类.
        - \b Warn 反映某些需要注意的可能有潜在危险的情况，可能会造成崩溃或逻辑错误之类.
          例如捕捉到异常并正确处理，这情况很常见，允许程序继续执行，但仍需注意.
        - \b Error 程序错误,例如一些API的调用失败,错误应当被妥善处理.
        - \b Fatal 致命错误,程序无法继续执行.
        - \b Hide 需要隐藏的信息,这类信息在Release版本(未定义_DEBUG)会自动被去除.
        建议把需要在编译时去除的信息归到这一类.
    使用UDEBUG,UTRACE,UINFO等宏进行输出的同时,也同时指定的了日志的类型.

    \section ulog_name_sec 日志名和日志分组.
    每条日志信息都可以设置一个名字,相同名字的日志属于同一个分组.
    在使用UDEBUG,UTRACE,UINFO等宏进行输出时,后面可以附加上日志名.例如:
    \code
    //日志名为"安装".
    UDEBUG("安装")<<"这是一条安装日志信息.";
    \endcode

    \section usage_sec 基本使用方法
    基本的使用方法有两种：
        -# 使用定义好的宏输出日志信息，对应每个日志类型，有以下宏可以使用：
            - UTRACE
            - UDEBUG
            - UINFO
            - UWARN
            - UERROR
            - UFATAL
            - UHIDE
            .
           采用流的方式输出,使用和标准库中流类似。
           \code
           char *name = "哥布林";
           UINFO<<"找到一个单位。";  
           UDEBUG<<"攻击单位:"<<name;
           UINFO("脚本")<<"调用攻击单位脚本.";
           //输入宽字符串(或者字符)时,会根据当前配置的区域(所包含的代码页)
           //转换成多字节字符串后再输出.
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

    \section ulog_enable_output_sec 指定日志是否输出
    可以在编译时以及运行时调整日志的输出.
    - 编译时设置某类型日志是否输出,默认情况下所有类型的日志都会被输出.
      编译时去除的日志输出不仅不会被指定,甚至可能不会被加到目标代码中,大多数
      编译器开启优化时都会将输出代码优化掉.
        - 去除所有类型的日志输出.
            - 在预编译定义(Preprocessor Definitions)中加入UNI_LOG_DISABLE_ALL,
              即可以去除本项目中所有类型的日志输出.
            - 在包含Ulog.h前#define UNI_LOG_DISABLE_ALL则可以去除本cpp中的所有
              类型的日志输出.如:
              \code
              #define UNI_LOG_DISABLE_ALL
              #include "ULog.h"
              \endcode
            .
        - 去除指定类型的日志输出和上述去除所有类型的日志输出方法相同.
          只是将UNI_LOG_DISABLE_ALL换成如下宏,对应不同的日志类型:
            - UNI_LOG_DISABLE_TRACE
            - UNI_LOG_DISABLE_DEBUG
            - UNI_LOG_DISABLE_INFO
            - UNI_LOG_DISABLE_WARN
            - UNI_LOG_DISABLE_ERROR
            - UNI_LOG_DISABLE_FATAL
            - UNI_LOG_DISABLE_HIDE
        - 类型为HideType的日志类型在Release版本(_DEBUG未被定义)时总是不输出的.
        - 通过这里的方式可以使指定类型的日志不输出,但是不能保证输出语句一定会被
          编译器优化没掉,while(false)后的语句是否被优化没掉和编译器及优化选项都有
          关系.
        .

    - 运行时设置日志输出.\n
      运行时通过静态函数enableOutput配置指定类型或者分组的日志是否输出.
        - 配置指定分组的日志是否输出.
          \code
          ULog::enableOutput("初始化",false);  //禁止名字为"初始化"的日志信息输出.
          ULog::enableOutput("流程",true);  //允许名字为"流程"的日志信息输出.
          \endcode
        - 配置指定类型的日志是否输出.
          \code
          ULog::enableOutput(ULog::TraceType,false);  //禁止Trace类型的日志信息输出.
          ULog::enableOutput(ULog::WarnType,true);  //允许Warn类型的日志信息输出.
          \endcode

    \section ulog_appender_sec 添加和设置输出源
    ULog可以输出到调试器，文件，控制台等不同的输出源. 你也可以自定义输出源输出到任意媒介.
        - 首先,你需要使用静态函数ULog::registerAppender注册输出源.
          \code
          //注册了一个文件输出源,名字为"errorlog",该输出源会输出到error.txt.
          ULog::registerAppender("errorlog",new ULog::FileAppender("error.txt"));
          //注册了一个控制台输出源,名字为"console",该输出源会输出到控制台.
          ULog::registerAppender("console",new ULog::ConsoleAppender);
          \endcode
          本日志模块提供了以下几种输出源：
          - DebuggerAppender 类，用于输出日志信息到调试器。
          - ConsoleAppender 类，用于输出日志信息到控制台。
          - FileAppender 类，用于输出日志信息到文件。
		  - EditControlAppender 类,用于输出日志到Edit控件.
		  - StaticControlAppender 类,用于输出日志到Static控件.
          .
        - 然后,使用setAppender设置指定分组的日志所要使用的输出源.
          \code
          //"初始化"日志仅输出到名字为"console"的输出源.
          ULog::setAppenders("初始化","console");
          //"流程"日志会输出到"debugger","console","errorlog"这几个输出源.
          //注意这里必须用空格分隔输出源的名字,如果格式错误,则不会输出或者输出
          //到错误的输出源.
          ULog::setAppenders("流程","debugger console errorlog");
          \endcode
          如果某分组的日志没有输出源,则使用无分组日志的输出源.\n
          若无分组日志也没有输出源,则为其初始化一个名为"default"的调试器输出源.

    \section manipulator_sec 操纵符
    ULog支持stringstream所支持的所有操纵符。此外，还支持以下操纵符。
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
      UTRACE<<delim(",")<<a<<b<<c;  //delim带参数时，以参数字符串分隔，输出的信息为"1,2,3"。
      \endcode
    - \b dumpmem Dump内存,并且以日志方式显示.
      \code
      UDEBUG<<dumpmem(0x00AC1032,100);  //输出如下
      //[5212] (ULogTest){内存}[trace][ULogTest] dumpmem(00AC1032,100) 
      //[5212] 00AC1032|| e9 99 2d 00 | 00 e9 e4 15 | 00 00 e9 6f | 1f 00 00 e9    ||  .  .  -  . |  .  .  .   |  .  .  .  o |    .  .  . 
      //[5212] 00AC1042|| 6a 4f 00 00 | e9 d5 52 00 | 00 e9 30 38 | 00 00 e9 fb    ||  j  O  .  . |  .  .  R  . |  .  .  0  8 |  .  .  .  . 
      //[5212] 00AC1052|| 10 00 00 e9 | a6 47 00 00 | e9 11 22 00 | 00 e9 3c 3e    ||    .  .  . |  .  G  .  . |  .    "  . |  .  .  <  > 
      //[5212] 00AC1062|| 00 00 e9 67 | 49 00 00 e9 | c2 2a 00 00 | e9 7d 41 00    ||  .  .  .  g |  I  .  .  . |  .  *  .  . |  .  }  A  . 
      //[5212] 00AC1072|| 00 e9 b8 28 | 00 00 e9 43 | 2a 00 00 e9 | de 4c 00 00    ||  .  .  .  ( |  .  .  .  C |  *  .  .  . |  .  L  .  . 
      //[5212] 00AC1082|| e9 89 21 00 | 00 e9 e4 53 | 00 00 e9 df | 36 00 00 e9    ||  .  .  !  . |  .  .  .  S |  .  .  .  . |  6  .  .  . 
      //[5212] 00AC1092|| 3a 40 00 00                                              ||  :  @  .  . 
      \endcode
    - \b hexdisp 以16进制显示数字.
      \code
      UDEBUG<<1024<<"的16进制是"<<hexdisp(1024);  //输出如下
      //[5492] (ULogTest){}[debug][ULogTest] 1024的16进制是0x00000400 <30>
      \endcode

    \section expand_sec 扩展ULog支持的类型
    ULog可以支持其他类型，只需要对该类型实现<<操纵符即可。例如以下代码实现对结构体POINT的支持：
    \code
    ULog operator<<(ULog log,POINT pt)
    {
        //组织信息，不添加分隔符，这句只能使用ULog内部支持的类型。
        log.disableDelim()<<"(POINT:x="<<pt.x<<",y="<<pt.y<<")";  
        return log.enableDelim();  
    }
    \endcode

*/


//! 日志类。
class ULog
{
public:
    //! 日志类型。
    enum Type
    {
        TraceType,    //!< 最精细的调试信息。
        DebugType,    //!< 普通的调试信息。
        InfoType,     //!< 这类信息表明了程序的大体流程。
        WarnType,     //!< 警告信息，描述了可能有害的情况。
        ErrorType,    //!< 错误，但仍然允许程序继续运行。例如调用API失败之类的错误。
        FatalType,    //!< 致命错误，会导致程序终止。
        HideType,     //!< 这类信息期望在编译的时候被优化掉。
    };

    //! 保存了一条日志信息的所有内容。
    struct Message
    {
        Message(Type type,const char *file,int line,const char *function)
            :ref_(1),type_(type),file_(file),line_(line),func_(function),delimEnabled_(true)
        {
        }
        int ref_;
        Type type_;
        std::string name_;  //!< 日志名.
        std::string file_;  //!< 这条日志输出所在的源文件.
        std::string func_;  //!< 这条日志输出所在的函数.
        int line_;          //!< 这条日志输出位置所在的行号.
        std::string delim_; //!< 流输出时所使用的分隔符.
        bool delimEnabled_; //!< 是否在两次插入间添加分隔符.
        std::ostringstream stm_;  //!< 保存了日志信息主体的流.
    };

    //! 输出源的基类.
    /*!
        可以派生这个类以实现自定义输出源.append函数负责将日志信息输出到输出源.
    */
    class Appender
    {
    public:
        Appender() {}
        virtual ~Appender() {}
        virtual void append(Message *message) = 0;
    private:
        Appender(const Appender &);
        Appender &operator=(const Appender &);
    };

    //! 调试器输出源.
    /*!
        通过OutputDebugString输出,推荐使用DebugView这个工具查看输出的信息.
    */
    class DebuggerAppender : public Appender
    {
    public:
        DebuggerAppender() {}
        virtual ~DebuggerAppender() {}
        virtual void append(Message *message);
    private:
        DebuggerAppender(const DebuggerAppender &);
        DebuggerAppender &operator=(const DebuggerAppender &);
    };

    class LoggerAppender : public Appender
    {
    public:
        LoggerAppender();
        virtual ~LoggerAppender();
        virtual void append(Message *message);
    private:
        LoggerAppender(const LoggerAppender &);
        LoggerAppender &operator=(const LoggerAppender &);
        std::ofstream logFile_;
    };

    //! 文件输出源
    /*!
        以文本格式将日志信息写入到文件,会从尾部追加.
    */
    class FileAppender : public Appender
    {
    public:
        explicit FileAppender(const wchar_t *fileName);
        virtual ~FileAppender();
        virtual void append(Message *message);
    private:
        FileAppender(const FileAppender &);
        FileAppender &operator=(const FileAppender &);
        std::ofstream file_;
    };

    //! 输出到控制台的输出源.
    /*!
        使用printf输出.
    */
    class ConsoleAppender : public Appender
    {
    public:
        ConsoleAppender() {}
        virtual ~ConsoleAppender(){}
        virtual void append(Message *message);
    private:
        ConsoleAppender(const ConsoleAppender &);
        ConsoleAppender &operator=(const ConsoleAppender &);
    };

    //! 输出到Edit控件的输出源.
    /*!
        每次输出都会显示在Edit控件的尾部.

		\note 必须在ui线程使用该Appender输出.否则可能导致死锁.
    */
    class EditCtrlAppender : public Appender
    {
    public:
        EditCtrlAppender(HWND hWnd)
            :hWnd_(hWnd)
        {

        }
        virtual void append(ULog::Message *message)
        {
            std::string s = message->stm_.str();
            s += "\r\n";

            int nLen = Edit_GetTextLength(hWnd_);
            Edit_SetSel(hWnd_, nLen, -1);
            Edit_ReplaceSel(hWnd_, uni::s2ws(s).c_str());
        }
    private:
        EditCtrlAppender(const EditCtrlAppender &);
        EditCtrlAppender &operator=(const EditCtrlAppender &);
        HWND hWnd_;
    };

	//! 输出到Static控件的输出源.
    /*!
        每次输出都会覆盖之前Static控件中的内容.

		\note 必须在ui线程使用该Appender输出.否则可能导致死锁.
    */
    class StaticCtrlAppender : public Appender
    {
    public:
        StaticCtrlAppender(HWND hWnd)
            :hWnd_(hWnd)
        {

        }
        virtual void append(ULog::Message *message)
        {
            std::wstring s;
			s = s2ws(message->stm_.str());
			DWORD_PTR result = 0;
            SendMessageTimeout(hWnd_,WM_SETTEXT,0,(LPARAM)s.c_str(),
				SMTO_ERRORONEXIT|SMTO_ABORTIFHUNG|SMTO_NORMAL,500,&result);
        }
    private:
        StaticCtrlAppender(const StaticCtrlAppender &);
        StaticCtrlAppender &operator=(const StaticCtrlAppender &);
        HWND hWnd_;
    };

    //! 无参数的操纵符。
    typedef ULog &(__cdecl *NullaryManipulator)(ULog &);

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

    //! 两个参数的操纵符。
    template<typename ArgAType,typename ArgBType>
    struct BinaryManipulator
    {
        BinaryManipulator(void (__cdecl *pfunc)(ULog &,ArgAType,ArgBType),ArgAType argA,ArgBType argB)
            :pfunc_(pfunc),argA_(argA),argB_(argB)
        {
        }
        void (__cdecl *pfunc_)(ULog &,ArgAType,ArgBType);
        ArgAType argA_;
        ArgBType argB_;
    };

    //! 给定日志类型，构造一个日志对象。
    /*!
        \param type 日志类型。
    */
    explicit ULog(Type type,const char *file,int line,const char *function);

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

    //! 配置指定名字的日志所使用的输出源.
    /*!
        \param name 日志分组名
        \param appenderList 输出源名字列表,以空格分隔的输出源名字.
        如"file debugger".

        设置名字为name的日志所使用的输出源,如果要设置多个输出源,
        则以空格分隔多个输出源的名字,例如:
        \code
        //所有名字为"安装"的日志将输出到名字为"setup","console",
        //"debugger"这3个输出源.
        setAppenders("安装","setup console debugger");
        //名字为"错误"的日志将输出到名字为"error"的输出源.
        setAppenders("错误","error");
        \endcode
    */
    static void setAppenders(const std::string &name,
        const std::string &appenderList);

    //! 获取指定名字的日志所使用的输出源.
    /*!
        \param name 日志分组名.
        \return 输出源名字列表.
    */
    static std::vector<std::string> getAppenders(const std::string &name);

    //! 判断输出源的名字是否有效.
    /*!
        \param name 输出源名字.
        \return 该输出源名字是否有效(是否允许被注册).

        名字只能由字母,数字和下划线组成,不能包含空格,也不能为空字符串.
    */
    static bool isAppenderNameValid(const std::string &appenderName);
    
    //! 注册一个输出源到日志系统.
    /*!
        \param appenderName 输出源的名字,名字只能由字母数字下划线组成,不能包含空格.
            假如传入空字符串,则注册失败.
        \param appender 输出源指针,必须是new出来的对象,删除由ULog负责,
            假如appender为0,或者是一个错误的指针,则行为未定义.

        若指定名字的输出源已经存在, 则替换掉存在的输出源.
    */
    static void registerAppender(const std::string &appenderName,Appender *appender);

    //! 获得当前注册的所有输出源的名字.
    /*!
        \return 返回的数组中包含了所有输出源的名字.
    */
    static std::vector<std::string> getRegisteredAppenderNames();

    //! 从日志系统删除一个输出源.
    /*!
        \param appenderName 输出源的名字,名字由字母数字下划线组成,不能包含空格.
    */
    static void unregisterAppender(const std::string &appenderName);

    //! 卸载所有的输出源.
    /*!
        该函数清除所有在日志系统中注册过的输出源.
    */
    static void unregisterAllAppenders();

    //! 判断指定类型的日志是否输出.
    /*!
        \param type 日志类型.
        \return 指定类型的日志是否输出.
    */
    static bool isOutputEnabled(Type type);

    //! 设置指定类型的日志是否输出.
    /*!
        \param type 日志类型
        \param enable 该类型的日志是否输出.
    */
    static void enableOutput(Type type,bool enable);

    //! 判断指定分组的日志是否输出.
    /*!
        \param name 日志分组的名字.
        \return 该分组的日志是否输出.
    */
    static bool isOutputEnabled(const std::string &name);

    //! 设置指定分组的日志是否输出.
    /*!
        \param name 日志分组的名字.
        \param enable 该分组的日志是否输出.
    */
    static void enableOutput(const std::string &name,bool enable);

    //! 获得输出过的日志名字。
    static std::set<std::string> names() {return names_;}

    //! 设置项目名。
    static void setProjectName(const std::string &projectName) {projectName_ = projectName;}

    //! 返回当前ULog使用的locale，默认情况是使用本机区域设置。
    static _locale_t locale();

    //! 还原回默认的设置.所有static状态将被恢复到初始值.
    static void restoreDefaultSettings();

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
        message_->stm_<<_Manip;
        return (*this);
    }

    //重新启用分隔符,假如有指定分隔符,那么之后的输出会输出分隔符.
    ULog &enableDelim()
    {
        message_->delimEnabled_ = true; 
        return mayHasDelim();
    }

    //禁用分隔符,即使有设定分隔符,之后的输出也不会添加分隔符.
    ULog &disableDelim() {message_->delimEnabled_ = false; return *this;}

    ULog &mayHasDelim() {if(!message_->delim_.empty() && message_->delimEnabled_) {message_->stm_<<message_->delim_;} return *this;}

	ULog &operator<<(bool t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(short t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned short t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(int t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned int t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(long t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(unsigned long t) {message_->stm_<<t; return mayHasDelim();}

 #ifdef _LONGLONG
	ULog &operator<<(_LONGLONG t) {message_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(_ULONGLONG t) {message_->stm_<<t; return mayHasDelim();}
 #endif /* _LONGLONG */

	ULog &operator<<(float t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(double t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(long double t) {message_->stm_<<t; return mayHasDelim();}

	ULog &operator<<(const void *t) {message_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(std::basic_streambuf<char> *t) {message_->stm_<<t; return mayHasDelim();}

    ULog &operator<<(char t) 
    {
        message_->stm_<<t; 
        return mayHasDelim();
    }
    
    //! 输入C格式字符串.
    /*!
        假如t为0,则输出(null).
    */
    ULog &operator<<(const char *t) 
    {
        if(!t) 
        {
            message_->stm_<<"(null)";
        }
        else
        {
            message_->stm_<<t; 
        }
        return mayHasDelim();
    }

    //! 写入一个宽字符。
#if _NATIVE_WCHAR_T_DEFINED
    ULog &operator<<(wchar_t t);
#endif

    //! 写入宽字符字串。
#if _NATIVE_WCHAR_T_DEFINED
    ULog &operator<<(const wchar_t *t);
#endif

    ULog &operator<<(const std::string &t) {message_->stm_<<t; return mayHasDelim();}

    //! 写入wstring。
    ULog &operator<<(const std::wstring &t);

    //! 接受ULog无参数操纵符。
    ULog &operator<<(NullaryManipulator m)
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

    //! 接受ULog两个参数操纵符。
    template<class ArgAType,class ArgBType>
    ULog &operator<<(BinaryManipulator<ArgAType,ArgBType> m)
    {
        m.pfunc_(*this,m.argA_,m.argB_);
        return (*this);
    }

    //! 返回保存的日志主体信息.
    std::string message();

    friend void ULogSetName(ULog &log,const char *name);

    friend ULog &lasterr(ULog &log);

    friend void ULogSetDelim(ULog &log,const char *delim);

    friend void ULogDumpMemory(ULog &log,const char *address,int len);

    friend void ULogHexDisp(ULog &log,int number);

private:
    unsigned long lastError_;
    Message *message_;
    static std::map<std::string,std::tr1::shared_ptr<Appender> > appenders_;
    static std::map<std::string,std::vector<std::string> > appendersForName_;
    static uni::ULock mutexForAppenders_;
    static std::map<Type,bool> typeFilter_;  //!< true则代表指定类型的日志将被过滤.
    static std::map<std::string,bool> nameFilter_;  //!< true则指定分组的日志将被过滤.
    static uni::ULock mutexForFilters_;
    static std::set<std::string> names_;  //!< 保存了输出过的日志的名字。
    static uni::ULock mutexForNames_;
    static _locale_t loc_;
    static std::string projectName_;
};

inline ULog &ULogSetName(ULog &log)
{
    return log;
}

ULog::SManipulator<const char *> ULogSetName(const char *name);

ULog::SManipulator<const char *> delim(const char *delim);

//! 将address开始len长度的数据输出。
/*!
    \param address dump内存的起始地址,内存必须要可读.
*/
inline ULog::BinaryManipulator<const char *,int> dumpmem(const char *address,int len)
{
    return ULog::BinaryManipulator<const char *,int>(&ULogDumpMemory,address,len);
}

inline ULog &delim(ULog &log)
{
    log<<delim(" ");
    return log;
}

inline ULog::SManipulator<int> hexdisp(int number)
{
    ULog::SManipulator<int> manipulator(&ULogHexDisp,number);
    return manipulator;
}

inline ULog &decdisp(ULog &log)
{
    log<<std::dec<<std::setfill(' ')<<std::setw(0);
    return log;
}

ULog uLog(ULog::Type type,const char *file,int line,const char *function);


#ifdef UNI_LOG_DISABLE_ALL
  #define UTRACE while(false) uLog(ULog::TraceType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UDEBUG while(false) uLog(ULog::DebugType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UINFO while(false) uLog(ULog::InfoType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UWARN while(false) uLog(ULog::WarnType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UERROR while(false) uLog(ULog::ErrorType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UFATAL while(false) uLog(ULog::FatalType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #define UHIDE while(false) uLog(ULog::HideType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
#else
  #ifdef UNI_LOG_DISABLE_TRACE
    #define UTRACE while(false) uLog(ULog::TraceType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UTRACE uLog(ULog::TraceType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_DEBUG
    #define UDEBUG while(false) uLog(ULog::DebugType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UDEBUG uLog(ULog::DebugType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_INFO
    #define UINFO while(false) uLog(ULog::InfoType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UINFO uLog(ULog::InfoType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_WARN
    #define UWARN while(false) uLog(ULog::WarnType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UWARN uLog(ULog::WarnType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_ERROR
    #define UERROR while(false) uLog(ULog::ErrorType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UERROR uLog(ULog::ErrorType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_FATAL
    #define UFATAL while(false) uLog(ULog::FatalType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #define UFATAL uLog(ULog::FatalType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #endif
  #ifdef UNI_LOG_DISABLE_HIDE
    #define UHIDE while(false) uLog(ULog::HideType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
  #else
    #ifdef _DEBUG
      #define UHIDE uLog(ULog::HideType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
    #else
      #define UHIDE while(false) uLog(ULog::HideType,__FILE__,__LINE__,__FUNCTION__)<<ULogSetName
    #endif
  #endif
#endif

}//namespace uni

#endif//UNICORE_ULOG_H