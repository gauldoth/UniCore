#include "ULog.h"

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <map>
#include <string>

#include "UCast.h"
#include "UAppender.h"

using namespace std;
using namespace std::tr1;
using namespace boost::interprocess;

namespace uni
{

const bool ULog::noWarnFalse_ = false;

struct ULog::Impl
{
    Impl()
    {
        logLevel_ = ULog::AllLevel;
        tr1::shared_ptr<UAppender> appender(new UDebuggerAppender);
        appenders_["default"] = appender;
        layout_[ULog::TraceType] = "[%t][%n][%l]%m";
        layout_[ULog::DebugType] = "[%t][%n][%l]%m";
        layout_[ULog::InfoType] = "[%t]%m";
        layout_[ULog::WarnType] = "[%t]%m";
        layout_[ULog::ErrorType] = "[%t]%m";
        layout_[ULog::FatalType] = "[%t]%m";
        loc_ = _create_locale(LC_ALL,"");
    }
    ~Impl()
    {
        _free_locale(loc_);
    }
    interprocess_mutex mutex4Appenders_;
    interprocess_mutex mutex4Layout_;
    LogLevel logLevel_;
    std::map<string,tr1::shared_ptr<UAppender> > appenders_;
    std::map<ULog::Type,std::string> layout_;
    _locale_t loc_;
};

std::tr1::shared_ptr<ULog::Impl> ULog::pimpl_(new ULog::Impl);

ULog uLog(ULog::Type type /*= ULog::InfoType*/)
{
    return ULog(type);
}

ULog::ULog(Type type /*= InfoType*/)
:lastError_(GetLastError())
,stream_(new Stream(type))
{
}

ULog::ULog(const ULog &log)
:lastError_(log.lastError_)
,stream_(log.stream_)
{
    ++stream_->ref_;
}

ULog & ULog::operator=( const ULog &log )
{
    if(this != &log)
    {
        ULog copy(log);
        swap(copy);
    }
    return (*this);
}


ULog::~ULog()
{
    if(!--stream_->ref_)
    {
        if(pimpl_->logLevel_ <= stream_->type_)
        {
            //允许输出。
            //检查是否有分隔符。
            string message = stream_->stm_.str();
            if(!stream_->delim_.empty())
            {
                string::size_type pos = message.rfind(stream_->delim_);
                if(pos != string::npos
                    && pos + stream_->delim_.size() == message.size())
                {
                    message.erase(pos);
                }
            }
            string layoutString;
            {
                //加锁。
                scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Layout_);
                layoutString = pimpl_->layout_[stream_->type_];
            }
            //找到layoutString的%,然后替换，然后继续找%，然后替换。
            string::size_type index = 0;
            index = layoutString.find('%',index);
            while(index != string::npos && index+1 < layoutString.size())
            {
                switch(layoutString[index+1])
                {
                case 'f':
                    {
                        //%f 当前文件。
                        layoutString = layoutString.replace(index,2,stream_->file_);
                        index += stream_->file_.size();
                        break;
                    }
                case 'n':
                    {
                        //%r 当前函数。
                        layoutString = layoutString.replace(index,2,stream_->func_);
                        index += stream_->func_.size();
                        break;
                    }
                case 'l':
                    {
                        //%l 当前行。
                        char buf[20] = "";
                        _itoa_s(stream_->line_,buf,10);
                        buf[19] = 0;
                        layoutString = layoutString.replace(index,2,buf);
                        index += strlen(buf);
                        break;
                    }
                case 'm':
                    {
                        layoutString = layoutString.replace(index,2,message);
                        index += message.size();
                        break;
                    }
                case 't':
                    {
                        string typeString;
                        switch(stream_->type_)
                        {
                        case ULog::TraceType:
                            {
                                typeString = "trace";
                                break;
                            }
                        case ULog::DebugType:
                            {
                                typeString = "debug";
                                break;
                            }
                        case ULog::InfoType:
                            {
                                typeString = "info";
                                break;
                            }
                        case ULog::WarnType:
                            {
                                typeString = "warn";
                                break;
                            }
                        case ULog::ErrorType:
                            {
                                typeString = "error";
                                break;
                            }
                        case ULog::FatalType:
                            {
                                typeString = "fatal";
                                break;
                            }
                        }
                        layoutString = layoutString.replace(index,2,typeString);
                        index += typeString.size();
                        break;
                    }
                default:
                    {
                        index ++;
                        break;
                    }
                }
                index = layoutString.find('%',index);
            }
            
            scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
            map<string,tr1::shared_ptr<UAppender> >::const_iterator it;
            for(it = pimpl_->appenders_.begin(); it != pimpl_->appenders_.end(); ++it)
            {
                it->second->append(layoutString);
            }
        }
        delete stream_;
    }
}

void ULog::swap(ULog &log)
{
    std::swap(lastError_,log.lastError_);
    std::swap(stream_,log.stream_);
}

void ULog::setLogLevel(LogLevel level)
{
    pimpl_->logLevel_ = level;
}

ULog::LogLevel ULog::logLevel()
{
    return pimpl_->logLevel_;
}

void ULog::setAppender(const std::string &name,UAppender *appender)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
    std::tr1::shared_ptr<UAppender> pAppender(appender);
    pimpl_->appenders_[name] = pAppender;
}

void ULog::setAppender(const std::wstring &name,UAppender *appender)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
    std::tr1::shared_ptr<UAppender> pAppender(appender);
    pimpl_->appenders_[ws2s(name,pimpl_->loc_)] = pAppender;
}

void ULog::removeAllAppenders()
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
    pimpl_->appenders_.clear();
}

void ULog::removeAppender(const string &name)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
    pimpl_->appenders_.erase(name);
}

void ULog::removeAppender(const wstring &name)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Appenders_);
    pimpl_->appenders_.erase(ws2s(name));
}


void ULog::setLayout(ULog::Type type,const std::string &format)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Layout_);
    pimpl_->layout_[type] = format;
}

void ULog::setLayout(ULog::Type type,const std::wstring &format)
{
    scoped_lock<interprocess_mutex> lock(pimpl_->mutex4Layout_);
    pimpl_->layout_[type] = ws2s(format,pimpl_->loc_);
}

_locale_t ULog::locale()
{
    return pimpl_->loc_;
}

ULog &ULog::operator<<(std::ostream &(*ostreamManipulator)(std::ostream &))
{
    stream_->stm_<<ostreamManipulator;
    return *this;
}

ULog &ULog::operator<<(std::basic_ios<char> &(*basicIosManipulator)(std::basic_ios<char> &))
{
    stream_->stm_<<basicIosManipulator;
    return *this;
}

ULog &ULog::operator<<(std::ios_base &(*iosBaseManipulator)(std::ios_base &))
{
    stream_->stm_<<iosBaseManipulator;
    return *this;
}

ULog &ULog::operator<<(const std::_Fillobj<char>& _Manip)
{
    stream_->stm_<<_Manip;
    return *this;
}

ULog &ULog::operator<<(wchar_t t)
{
    wchar_t temp[2] = L"";
    temp[0] = t;
    temp[1] = L'\0';
    stream_->stm_<<ws2s(temp,pimpl_->loc_);
    return mayHasDelim();
}

ULog &ULog::operator<<(const std::wstring &t) 
{
    stream_->stm_<<ws2s(t);
    return mayHasDelim();
}

ULog &ULog::operator<<(const wchar_t *t)
{
    int ptr = reinterpret_cast<int>(t);
    ptr;
    assert(ptr >= 0x10000 && ptr < 0x80000000 || !"向ULog输入的宽字符指针为空。");
    stream_->stm_<<ws2s(t,pimpl_->loc_);
    return mayHasDelim();
}

ULog &lasterr(ULog &log)
{
    DWORD error = log.lastError_;
    HLOCAL hLocal = NULL;
    DWORD systemLocale = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);
    BOOL ok = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,error,systemLocale,
        (LPSTR)&hLocal,0,NULL);

    if(!ok)
    {
        HMODULE hDll = LoadLibraryExW(L"netmsg.dll",NULL,LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if(hDll)
        {
            ok = FormatMessageA(
                FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER,
                hDll,error,systemLocale,(LPSTR)&hLocal,0,NULL);

            FreeLibrary(hDll);
        }
    }

    if(ok && hLocal)
    {
        char *szErrorMsg = (LPSTR)LocalLock(hLocal);
        int len = strlen(szErrorMsg);
        if(szErrorMsg[len-2] == '\r')
        {
            szErrorMsg[len-2] = 0;
        }
        if(szErrorMsg[len-1] == '\n')
        {
            szErrorMsg[len-1] = 0;
        }
        log.stream_->stm_<<"{ 错误号："<<error<<" 错误描述："<<szErrorMsg<<"}";
    }
    else
    {
        log.stream_->stm_<<"{ 错误号："<<error<<" 错误描述：找不到对应的错误描述。}";
    }

    if(hLocal)
    {
        LocalFree(hLocal);
    }

    return log.mayHasDelim();
}

std::string ULogSprintf(const char *format,...)
{
    va_list ap;
    va_start(ap,format);

    const int len = _vscprintf_p_l(format,ULog::locale(),ap)+1;
    char *buffer = new char[len];
    //locale可能影响到数字的显示，暂时无需用_l版本。
    _vsprintf_p_l(buffer,len,format,ULog::locale(),ap);
    va_end(ap);

    std::string result(buffer);
    delete []buffer;
    return result;
}

std::wstring ULogSprintf(const wchar_t *format,...)
{
    va_list ap;
    va_start(ap,format);

    const int len = _vscwprintf_p_l(format,ULog::locale(),ap)+1;
    wchar_t *buffer = new wchar_t[len];
    //locale可能影响到数字的显示，暂时无需用_l版本。
    _vswprintf_p_l(buffer,len,format,ULog::locale(),ap);
    va_end(ap);

    std::wstring result(buffer);
    delete []buffer;
    return result;
}

void ULogSetFile(ULog &log,const char *file)
{
    log.stream_->file_ = file;
}

void ULogSetFunc(ULog &log,const char *func)
{
    log.stream_->func_ = func;
}

void ULogSetLine(ULog &log,int line)
{
    log.stream_->line_ = line;
}

void ULogSetDelim(ULog &log,const char *delim)
{
    log.stream_->delim_ = delim;
}

ULog::SManipulator<const char *> ULogSetFile(const char *file)
{
    return ULog::SManipulator<const char *>(&ULogSetFile,file);
}

ULog::SManipulator<const char *> ULogSetFunc(const char *func)
{
    return ULog::SManipulator<const char *>(&ULogSetFunc,func);
}

ULog::SManipulator<int> ULogSetLine(int line)
{
    return ULog::SManipulator<int>(&ULogSetLine,line);
}

ULog::SManipulator<const char *> delim(const char *delim)
{
    return ULog::SManipulator<const char *>(&ULogSetDelim,delim);
}

}//namespace uni