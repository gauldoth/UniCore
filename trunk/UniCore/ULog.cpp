#include "ULog.h"

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <map>
#include <string>

#include "UCast.h"
#include "UDebug.h"
#include "UMemory.h"

using namespace std;
using namespace std::tr1;
using namespace boost::interprocess;

namespace uni
{

boost::interprocess::interprocess_mutex ULog::mutexForAppenders_;

std::map<std::string,std::tr1::shared_ptr<ULog::Appender> > ULog::appenders_;

boost::interprocess::interprocess_mutex ULog::mutexForLevels_;

std::map<std::string,ULog::Level> ULog::levels_;

std::set<std::string> ULog::names_;

boost::interprocess::interprocess_mutex ULog::mutexForNames_;

_locale_t ULog::loc_ = _create_locale(LC_ALL,"");

std::string ULog::projectName_;

ULog uLog(ULog::Type type,const char *file,int line,const char *function)
{
    return ULog(type,file,line,function);
}

ULog::ULog(Type type,const char *file,int line,const char *function)
:lastError_(GetLastError())
,message_(new Message(type,file,line,function))
{
}

ULog::ULog(const ULog &log)
:lastError_(log.lastError_)
,message_(log.message_)
{
    ++message_->ref_;
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
    if(!--message_->ref_)
    {
        //获得当前日志所允许的等级。
        Level minLevelAllowed = AllLevel;
        {
        scoped_lock<interprocess_mutex> lock(mutexForLevels_);
        if(levels_.count(message_->name_))
        {
            minLevelAllowed = levels_[message_->name_];
        }
        }
        if(minLevelAllowed <= message_->type_)
        {
            //允许输出。
            //检查是否有分隔符。
            string message = message_->stm_.str();
            if(!message_->delim_.empty())
            {
                string::size_type pos = message.rfind(message_->delim_);
                if(pos != string::npos
                    && pos + message_->delim_.size() == message.size())
                {
                    message_->stm_.str().erase(pos);
                }
            }
            
            scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
            if(!appenders_.count(message_->name_))
            {
                std::tr1::shared_ptr<Appender> appenderPtr(new DebuggerAppender);
                appenders_[message_->name_] = appenderPtr;
            }
            appenders_[message_->name_]->append(message_);
        }
        delete message_;
    }
}

void ULog::swap(ULog &log)
{
    std::swap(lastError_,log.lastError_);
    std::swap(message_,log.message_);
}

void ULog::setLogLevel(const std::string &name,Level level)
{
    scoped_lock<interprocess_mutex> lock(mutexForLevels_);
    levels_[name] = level;
}

ULog::Level ULog::logLevel(const std::string &name)
{
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    if(levels_.count(name))
    {
        return levels_[name];
    }
    else
    {
        return AllLevel;
    }
}

void ULog::setAppender(const std::string &name,AppenderType type)
{
    Appender *appender = 0;
    switch(type)
    {
    case DebuggerAppenderType:
        {
            appender = new DebuggerAppender;
            break;
        }
    default:
        {
            appender = new DebuggerAppender;
            break;
        }
    }

    std::tr1::shared_ptr<Appender> appenderPtr(appender);

    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    appenders_[name] = appenderPtr;
}

_locale_t ULog::locale()
{
    return loc_;
}

ULog &ULog::operator<<(std::ostream &(*ostreamManipulator)(std::ostream &))
{
    message_->stm_<<ostreamManipulator;
    return *this;
}

ULog &ULog::operator<<(std::basic_ios<char> &(*basicIosManipulator)(std::basic_ios<char> &))
{
    message_->stm_<<basicIosManipulator;
    return *this;
}

ULog &ULog::operator<<(std::ios_base &(*iosBaseManipulator)(std::ios_base &))
{
    message_->stm_<<iosBaseManipulator;
    return *this;
}

ULog &ULog::operator<<(const std::_Fillobj<char>& _Manip)
{
    message_->stm_<<_Manip;
    return *this;
}

ULog &ULog::operator<<(wchar_t t)
{
    wchar_t temp[2] = L"";
    temp[0] = t;
    temp[1] = L'\0';
    message_->stm_<<ws2s(temp,loc_);
    return mayHasDelim();
}

ULog &ULog::operator<<(const std::wstring &t) 
{
    message_->stm_<<ws2s(t);
    return mayHasDelim();
}

ULog &ULog::operator<<(const wchar_t *t)
{
    int ptr = reinterpret_cast<int>(t);
    ptr;
    assert(ptr >= 0x10000 && ptr < 0x80000000 || !"向ULog输入的宽字符指针为空。");
    message_->stm_<<ws2s(t,loc_);
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
        log.message_->stm_<<"{ 错误号："<<error<<" 错误描述："<<szErrorMsg<<"}";
    }
    else
    {
        log.message_->stm_<<"{ 错误号："<<error<<" 错误描述：找不到对应的错误描述。}";
    }

    if(hLocal)
    {
        LocalFree(hLocal);
    }

    return log.mayHasDelim();
}

void ULogSetDelim(ULog &log,const char *delim)
{
    log.message_->delim_ = delim;
}

ULog::SManipulator<const char *> delim(const char *delim)
{
    return ULog::SManipulator<const char *>(&ULogSetDelim,delim);
}

void ULogSetName(ULog &log,const char *name)
{
    log.message_->name_ = name;
    scoped_lock<interprocess_mutex> lock(ULog::mutexForNames_);
    if(!log.names_.count(name))
    {
        log.names_.insert(name);
    }
}

ULog::SManipulator<const char *> ULogSetName(const char *name)
{
    return ULog::SManipulator<const char *>(&ULogSetName,name);
}


void ULog::DebuggerAppender::append( Message *message )
{
    string type;
    switch(message->type_)
    {
    case TraceType:
        {
            type = "trace";
            break;
        }
    case DebugType:
        {
            type = "debug";
            break;
        }
    case InfoType:
        {
            type = "info";
            break;
        }
    case WarnType:
        {
            type = "warn";
            break;
        }
    case ErrorType:
        {
            type = "error";
            break;
        }
    case FatalType:
        {
            type = "fatal";
            break;
        }
    default:
        {
            assert(!"未知的日志类型。");
        }
    }
    DebugMessage("(%s){%s}[%s][%s] %s <%d>",projectName_.c_str(),message->name_.c_str(),type.c_str(),message->func_.c_str(),message->stm_.str().c_str(),message->line_);
}



void ULogDumpMemory( ULog &log,const char *address,int len )
{
    log<<"dumpmem("<<(void *)address<<","<<len<<")\r\n";
    char *beginAddress = const_cast<char *>(address);//reinterpret_cast<char *>(address);
    stringstream dumpInfo;
    stringstream dumpMessage;
    for(int i = 0; i < len; i++)
    {
        if(i%16 == 0)
        {
            dumpInfo<<reinterpret_cast<void *>(beginAddress+i);
            dumpInfo<<"||";
            dumpMessage<<"||";
        }
        else if(i%4 == 0)
        {
            dumpInfo<<" |";
            dumpMessage<<" |";
        }
        int charValue = GetAt<unsigned char>(beginAddress,i);
        dumpInfo<<" "<<hex<<noshowbase<<setw(2)<<setfill('0')<<charValue;

        char word = GetAt<char>(beginAddress,i);
        if(word == '\0')
        {
            //dumpMessage<<"0"<<setw(2)<<"\\0";
            dumpMessage<<setw(3)<<setfill(' ')<<"."<<setw(0);
        }
        else if(word == '\r')
        {
            dumpMessage<<setw(3)<<setfill(' ')<<"\r"<<setw(0);
        }
        else if(word == '\n')
        {
            dumpMessage<<setw(3)<<setfill(' ')<<"\n"<<setw(0);
        }
        else if(word < 0)
        {
            dumpMessage<<hex<<noshowbase<<setw(3)<<setfill(' ')
                <<"."<<setw(0);
        }
        else
        {
            dumpMessage<<setw(3)<<word<<setw(0);
        }
        if(i%16 == 15 || i == len-1)
        {
            log<<setw(68)<<left<<dumpInfo.str()<<dumpMessage.str()<<"\r\n";
            dumpInfo.clear();
            dumpInfo.str("");
            dumpMessage.clear();
            dumpMessage.str("");
        }
    }
}


}//namespace uni