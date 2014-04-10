/*! \file UMiniLog.h
    \brief 日志相关。

    使用MINILOG<<"asd",这种类似cout的方式记录日志.
    调试版本,未定义NDEBUG时,日志将会通过OutputDebugStringA输出到调试器.
    发布版本,定义了NDEBUG,日志将会写入到文件.
    可以通过定义DISABLE_MINILOG来禁止日志的输出.
    日志的保存位置可以自行修改代码.
    

    \author     unigauldoth@gmail.com
    \date       2014-2-18
*/
#ifndef UNICORE_UMINILOG_H
#define UNICORE_UMINILOG_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <shlobj.h>
#include <regex>
#include <string>

//生成的日志文件的全路径由3部分组成.
//基础路径,子路径,日志名.

//生成日志的基础文件名.
#define MINILOG_BASENAME L"log"
#ifndef MINILOG_BASENAME
#define MINILOG_BASENAME L"log"
#endif

static const wchar_t * const MINILOG_SUBDIR = L"mylog";  //创建日志时创建的子目录.

static bool DirectoryExists( const std::wstring &directory )
{
    DWORD attribute = GetFileAttributesW(directory.c_str());
    if (attribute == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    if (attribute & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    return false;
}

static std::vector<std::wstring> split( const std::wstring &s,const std::wstring &delim /*= L" "*/ )
{

    std::vector<std::wstring> results;
    if(s.empty() || delim.empty())
    {
        //源字符串为空或者分隔符为空都无法进行分割.
        return results;
    }
    if(s.size() < delim.size())
    {
        //源字符串长度小于分隔符长度时也无法分割.
        return results;
    }
    std::wstring::size_type currentPos = 0;
    std::wstring::size_type delimPos = 0;
    while(true)
    {
        delimPos = s.find(delim,currentPos);
        if(delimPos == std::wstring::npos)
        {
            //从当前位置开始找不到分隔符,将剩余的字符串加入结果数组.
            results.push_back(s.substr(currentPos));
            break;
        }
        else
        {
            //找到分隔符
            assert(delimPos >= currentPos);
            if(delimPos >= currentPos)
            {
                //分隔符位置如果等于当前位置,则保存一个空字符串.
                results.push_back(s.substr(currentPos,delimPos-currentPos));
            }
            currentPos = delimPos+delim.length();  //当前位置指向分隔符后面一个位置.
        }

    }
    return results;
}


static bool CreateDirectories( const std::wstring &path )
{
    //支持LFS(Local File System)和UNC(Universal Naming Convention).
    //"\\192.168.0.2\Home\【Share】\xiaodong_li"
    //"C:\Program Files\Foxit Software\Foxit PhantomPDF"
    std::wstring uncName;
    std::wstring volumeName;
    std::wstring remaining;
    //"\\.*?\.*?\",由于c++和regex都用到转义,因此一个'\'变成了四个.
    std::wregex reUncName(L"\\\\\\\\.*?\\\\.*?\\\\");
    std::wsmatch result;
    if(regex_search(path,result,reUncName))
    {
        uncName = result[0];
        remaining = result.suffix();
    }
    std::wregex reVolumeName(L"\\w+?:\\\\");
    if(std::regex_search(path,result,reVolumeName))
    {
        volumeName = result[0];
        remaining = result.suffix();
    }
    std::vector<std::wstring> pathFragments = split(remaining,L"\\");

    std::wstring directoryToCreate;
    if(!uncName.empty())
    {
        directoryToCreate = uncName;
    }
    else if(!volumeName.empty())
    {
        directoryToCreate = volumeName;
    }
    else
    {
        //directoryToCreate remains empty.
    }

    for(int i = 0; i < pathFragments.size(); i++)
    {
        if(!pathFragments[i].empty())
        {
            directoryToCreate += pathFragments[i];
            if(directoryToCreate.back() != L'\\')
            {
                directoryToCreate.push_back(L'\\');
            }
            if(!DirectoryExists(directoryToCreate.c_str()))
            {
                if(!CreateDirectoryW(directoryToCreate.c_str(),NULL))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

class UMiniLog
{
public:

    enum LogPath
    {
        CurrentDir,
        TempFolder,
        LocalAppDataFolder,
    };
    class Lock
    {
    public:
        Lock():atomic_(0) {}
        ~Lock() {}
        //! 上锁.
        void lock() 
        {
            do 
            {
                long prev = InterlockedCompareExchange(&atomic_,1,0);
                if(atomic_ == 1 && prev == 0)
                {
                    break;
                }
                if(!SwitchToThread())
                {
                    Sleep(1);
                }
            } while (true);
        }
        //! 解锁.
        void unlock()
        {
            InterlockedCompareExchange(&atomic_,0,1);
        }
    private:
        Lock(const Lock&);
        Lock& operator=(const Lock&);
        volatile long atomic_;

    };

    struct Message
    {
        Message(const char *file,int line,const char *function)
            :ref_(1),file_(file),line_(line),func_(function)
        {
        }
        int ref_;
        std::string file_;  //!< 这条日志输出所在的源文件.
        std::string func_;  //!< 这条日志输出所在的函数.
        int line_;          //!< 这条日志输出位置所在的行号.
        std::ostringstream stm_;  //!< 保存了日志信息主体的流.
    };

    explicit UMiniLog(const char *file,int line,const char *function)
        :message_(new Message(file,line,function))
    {

    }
    UMiniLog(const UMiniLog &log)
        :message_(log.message_)
    {
        ++message_->ref_;
    }
    UMiniLog &operator=(const UMiniLog &log)
    {
        if(this != &log)
        {
            UMiniLog copy(log);
            swap(copy);
        }
        return (*this);
    }
    void swap(UMiniLog &log)
    {
        std::swap(message_,log.message_);
    }
    virtual ~UMiniLog()
    {
        //确保以下的代码不会抛出异常.
        if(!--message_->ref_)
        {
            //加锁并输出.
            {
                static Lock lock;
                lock.lock();

#ifndef NDEBUG
                {
                    std::string message = "[";
                    message += message_->func_;
                    message += "]";

                    message += " ";
                    message += message_->stm_.str();
                    message += " ";

                    char buf[255] = "";
                    _itoa_s(message_->line_,buf,10);
                    message += "<"; message += buf; message += ">";

                    OutputDebugStringA(message.c_str());
                }
#else


                static std::ofstream logFile;
                if(!logFile.is_open())
                {
                    //这里可以根据需要改变日志保存的路径.
                    
                    //获得日志所在的根目录.
                    std::wstring logBasePath;
                    if(MINILOG_PATH == UMiniLog::CurrentDir)
                    {
                        wchar_t buf[MAX_PATH] = L"";
                        GetCurrentDirectory(MAX_PATH,buf);
                        logBasePath = buf;
                    }
                    else if(MINILOG_PATH == UMiniLog::TempFolder)
                    {
                        wchar_t buf[MAX_PATH] = L"";
                        GetTempPathW(MAX_PATH,buf);
                        logBasePath = buf;
                    }
                    else if(MINILOG_PATH == UMiniLog::LocalAppDataFolder)
                    {
                        wchar_t buf[MAX_PATH] = L"";
                        if(SHGetFolderPath(0,CSIDL_LOCAL_APPDATA,NULL,SHGFP_TYPE_CURRENT,buf) == S_OK)
                        {
                            logBasePath = buf;  
                        }
                    }
                    else
                    {
                        assert(!"MINILOG_PATH is invalid.");
                    }

                    //获得日志名.
                    std::wstring logFileName = MINILOG_BASENAME;
                    wchar_t currentTime[MAX_PATH] = L"";
                    time_t t;
                    tm timeStruct;
                    time(&t);
                    localtime_s(&timeStruct,&t);
                    wcsftime(currentTime,MAX_PATH,L"%Y_%m_%d",&timeStruct);

                    logFileName += L"_";
                    logFileName += currentTime;
                    logFileName += L".txt";

                    //获得日志的中间目录.
                    std::wstring logSubDir = MINILOG_SUBDIR;

                    //生成日志的全路径.
                    std::wstring logFullPath;
                    if(!logBasePath.empty())
                    {
                        logFullPath += logBasePath;
                        if(logFullPath.back() != L'\\')
                        {
                            logFullPath += L'\\';
                        }
                    }

                    if(!logSubDir.empty())
                    {
                        if(logSubDir.front() == L'\\')
                        {
                            logSubDir.erase(0,1);
                        }
                        logFullPath += logSubDir;
                        if(logFullPath.back() != L'\\')
                        {
                            logFullPath += L'\\';
                        }
                    }

                    CreateDirectories(logFullPath);

                    if(!logFileName.empty())
                    {
                        logFullPath += logFileName;
                    }

                    logFile.open(logFullPath.c_str(),std::ofstream::out|std::ofstream::app);
                }

                if(logFile.is_open())
                {
                    time_t t;
                    tm timeStruct;
                    time(&t);
                    localtime_s(&timeStruct,&t);
                    char currentTime[255] = "";
                    asctime_s(currentTime,&timeStruct);

                    logFile<<currentTime<<" "
                        <<"["<<message_->func_<<"]"
                        <<" "<<message_->stm_.str()<<" "
                        <<"<"<<message_->line_<<">"<<std::endl;
                }
#endif

                lock.unlock();
            }

            delete message_;
            message_ = 0;
        }
    }

    //! 接受ostream的操纵符。
    /*!
        \param ostreamManipulator ostream操纵符，例如endl。
    */
    UMiniLog &operator<<(std::ostream &(*ostreamManipulator)(std::ostream &))
    {
        message_->stm_<<ostreamManipulator;
        return *this;
    }

    //! 接受basic_ios操纵符。
    UMiniLog &operator<<(std::basic_ios<char> &(*basicIosManipulator)(std::basic_ios<char> &))
    {
        message_->stm_<<basicIosManipulator;
        return *this;
    }

    //! 接受ios_base操纵符，例如hex，showbase等。
    UMiniLog &operator<<(std::ios_base &(*iosBaseManipulator)(std::ios_base &))
    {
        message_->stm_<<iosBaseManipulator;
        return *this;
    }

    //! 接受setfill操纵符。
    UMiniLog &operator<<(const std::_Fillobj<char>& _Manip)
    {
        message_->stm_<<_Manip;
        return *this;
    }

    //! 接受一元操纵符，如setw，setprecision之类。
    template<typename _Arg>
    UMiniLog &operator<<(const std::_Smanip<_Arg>& _Manip)
    {
        message_->stm_<<_Manip;
        return (*this);
    }

    UMiniLog &operator<<(bool t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(short t) {message_->stm_<<t; return *this;}

#if _NATIVE_WCHAR_T_DEFINED
	UMiniLog &operator<<(unsigned short t) {message_->stm_<<t; return *this;}
#endif

	UMiniLog &operator<<(int t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(unsigned int t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(long t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(unsigned long t) {message_->stm_<<t; return *this;}

 #ifdef _LONGLONG
	UMiniLog &operator<<(_LONGLONG t) {message_->stm_<<t; return *this;}

    UMiniLog &operator<<(_ULONGLONG t) {message_->stm_<<t; return *this;}
 #endif /* _LONGLONG */

	UMiniLog &operator<<(float t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(double t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(long double t) {message_->stm_<<t; return *this;}

	UMiniLog &operator<<(const void *t) {message_->stm_<<t; return *this;}

    UMiniLog &operator<<(std::basic_streambuf<char> *t) {message_->stm_<<t; return *this;}

    UMiniLog &operator<<(char t) 
    {
        message_->stm_<<t; 
        return *this;
    }
    
    //! 输入C格式字符串.
    /*!
        假如t为0,则输出(null).
    */
    UMiniLog &operator<<(const char *t) 
    {
        if(!t) 
        {
            message_->stm_<<"(null)";
        }
        else
        {
            message_->stm_<<t; 
        }
        return *this;
    }

    //! 将std::wstring转换为std::string。
    /*!
        ws2s使用指定的locale转换Unicode字符串到MBCS字符串。
        \param ws 源字符串。
        \param locale locale名字，请参考setlocale函数。
        \return 转换后的字符串。
        使用方法：
        \code
        //Test.cpp
        //Test.cpp使用到<string>中定义的类，应该包含<string>。
        #include <string>
        #include "UCast.h"

        using namespace std;
        string temp = ws2s(L"我们");  //使用系统当前的区域设置进行转换。
        \endcode
    */
    std::string ws2s(const std::wstring &ws,const char *locale = "")
    {
        using namespace std;
        string result;
        const size_t len = ws.size()*4+1;
        char *dest = new char[len];
        size_t numOfCharConverted = 0;
        _locale_t loc = _create_locale(LC_CTYPE,locale);
        if(!loc)
        {
            loc = _create_locale(LC_CTYPE,"");
        }
        errno_t err = _wcstombs_s_l(&numOfCharConverted,dest,len,ws.c_str(),_TRUNCATE,loc);
        _free_locale(loc);
        if(err == 0)
        {
            result = dest;
        }
        else if(err == STRUNCATE)
        {
            OutputDebugStringA("UniCore ws2s 目标缓冲区不足，字符串被截断。");
            result = dest;
        }
        else
        {
            assert(!"UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
            OutputDebugStringA("UniCore ws2s 转换Unicode字符串到MBCS字符串时失败。");
        }
        delete []dest;
        return result;
    }


    //! 写入一个宽字符。
    UMiniLog &operator<<(wchar_t t)
    {
        wchar_t temp[2] = L"";
        temp[0] = t;
        temp[1] = L'\0';
        message_->stm_<<ws2s(temp);
        return *this;
    }

    //! 写入宽字符字串。
    UMiniLog &operator<<(const wchar_t *t)
    {
        message_->stm_<<ws2s(t,"");
        return *this;
    }

    UMiniLog &operator<<(const std::string &t) {message_->stm_<<t; return *this;}

    //! 写入wstring。
    UMiniLog &operator<<(const std::wstring &t) {message_->stm_<<ws2s(t); return *this;}

private:
    Message *message_;
    static const UMiniLog::LogPath MINILOG_PATH = UMiniLog::CurrentDir;  //日志所在的根目录
};



inline UMiniLog uMiniLog(const char *file,int line,const char *function)
{
    return UMiniLog(file,line,function);
}

#ifndef DISABLE_MINILOG
#define MINILOG uMiniLog(__FILE__,__LINE__,__FUNCTION__)
#else
#define MINILOG while(false) uMiniLog(__FILE__,__LINE__,__FUNCTION__)
#endif

#endif