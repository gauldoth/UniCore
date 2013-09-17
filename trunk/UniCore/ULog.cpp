#include "ULog.h"

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <map>
#include <string>

#include "UCast.h"
#include "UCommon.h"
#include "UDebug.h"
#include "UMemory.h"

using namespace std;
using namespace std::tr1;
using namespace boost::interprocess;

namespace uni
{

std::map<std::string,std::tr1::shared_ptr<ULog::Appender> > ULog::appenders_;
std::map<std::string,std::vector<std::string> > ULog::appendersForName_;
boost::interprocess::interprocess_mutex ULog::mutexForAppenders_;

std::map<ULog::Type,bool> ULog::typeFilter_;
std::map<std::string,bool> ULog::nameFilter_;
boost::interprocess::interprocess_mutex ULog::mutexForFilters_;

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
        bool filtered = false;
        {
            scoped_lock<interprocess_mutex> lock(mutexForFilters_);
            filtered = typeFilter_[message_->type_] || nameFilter_[message_->name_];
        }
        if(!filtered)
        {
            //允许输出。
            //检查是否有分隔符。
            std::string message = message_->stm_.str();
            if(!message_->delim_.empty())
            {
                //删除尾部多余的分隔符.
                string::size_type pos = message.rfind(message_->delim_);
                if(pos != string::npos
                    && pos + message_->delim_.size() == message.size())
                {
                    message.erase(pos);
                    message_->stm_.str(message);
                }
            }
            scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
            vector<string> appenderNames = appendersForName_[message_->name_];
            if(appenderNames.empty())
            {
                //当前分组没有输出源,使用无分组日志的输出源.
                if(appendersForName_[""].empty())
                {
                    //无分组日志也没有输出源,添加一个默认输出源.
                    if(!appenders_.count("default"))
                    {
                        appenders_["default"] = 
                            std::tr1::shared_ptr<Appender>(new DebuggerAppender);
                    }
                    appendersForName_[""].push_back("default");
                }
                appenderNames = appendersForName_[""];
            }
            for(int i = 0; i < appenderNames.size(); i++)
            {
                if(appenders_.count(appenderNames[i]))
                {
                    appenders_[appenderNames[i]]->append(message_);
                }
            }
        }
        delete message_;
        message_ = 0;
    }
}

void ULog::swap(ULog &log)
{
    std::swap(lastError_,log.lastError_);
    std::swap(message_,log.message_);
}

void ULog::setAppenders(const std::string &name,const std::string &appenderList)
{
    vector<string> appenderNames = split(appenderList);

    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    appendersForName_[name] = appenderNames;
}

vector<std::string> ULog::getAppenders( const std::string &name )
{
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    return appendersForName_[name];
}

void ULog::registerAppender( const std::string &appenderName,Appender *appender )
{
    if(!isAppenderNameValid(appenderName))
    {
        return;
    }
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    std::tr1::shared_ptr<Appender> p(appender);
    appenders_[appenderName] = p;
}

bool ULog::isAppenderNameValid(const std::string &appenderName)
{
    if(appenderName.empty())
    {
        return false;
    }
    for(int i = 0; i < appenderName.size(); i++)
    {
        if(!isalnum(appenderName[i]) && appenderName[i] != '_')
        {
            return false;
        }
    }
    return true;
}

vector<string> ULog::getRegisteredAppenderNames()
{
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    vector<string> result;
    map<string,tr1::shared_ptr<Appender> >::const_iterator it;
    for(it = appenders_.begin(); it != appenders_.end(); ++it)
    {
        result.push_back(it->first);
    }
    return result;
}

void ULog::unregisterAppender( const std::string &appenderName )
{
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    appenders_.erase(appenderName);
}

void ULog::unregisterAllAppenders()
{
    scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
    map<std::string,std::tr1::shared_ptr<Appender> >::const_iterator it;
    appenders_.erase(appenders_.begin(),appenders_.end());
}

bool ULog::isOutputEnabled( Type type )
{
    scoped_lock<interprocess_mutex> lock(mutexForFilters_);
    return !typeFilter_[type];
}

void ULog::enableOutput( Type type,bool enable )
{
    scoped_lock<interprocess_mutex> lock(mutexForFilters_);
    typeFilter_[type] = !enable;
}

bool ULog::isOutputEnabled( const std::string &name )
{
    scoped_lock<interprocess_mutex> lock(mutexForFilters_);
    return !nameFilter_[name];
}

void ULog::enableOutput( const std::string &name,bool enable )
{
    scoped_lock<interprocess_mutex> lock(mutexForFilters_);
    nameFilter_[name] = !enable;
}

_locale_t ULog::locale()
{
    return loc_;
}

void ULog::restoreDefaultSettings()
{
    {
        scoped_lock<interprocess_mutex> lock(mutexForAppenders_);
        appenders_.erase(appenders_.begin(),appenders_.end());
        appendersForName_.erase(appendersForName_.begin(),appendersForName_.end());
    }
    {
        scoped_lock<interprocess_mutex> lock(mutexForFilters_);
        typeFilter_.erase(typeFilter_.begin(),typeFilter_.end());
        nameFilter_.erase(nameFilter_.begin(),nameFilter_.end());
    }
    {
        scoped_lock<interprocess_mutex> lock(mutexForNames_);
        names_.erase(names_.begin(),names_.end());
    }
    loc_ = _create_locale(LC_ALL,"");    
    projectName_ = "";
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

#if _NATIVE_WCHAR_T_DEFINED
ULog &ULog::operator<<(wchar_t t)
{
    wchar_t temp[2] = L"";
    temp[0] = t;
    temp[1] = L'\0';
    message_->stm_<<ws2s(temp,loc_);
    return mayHasDelim();
}
#endif

ULog &ULog::operator<<(const std::wstring &t) 
{
    message_->stm_<<ws2s(t,loc_);
    return mayHasDelim();
}

#if _NATIVE_WCHAR_T_DEFINED
ULog &ULog::operator<<(const wchar_t *t)
{
    int ptr = reinterpret_cast<int>(t);
    ptr;
    assert(ptr >= 0x10000 && ptr < 0x80000000 || !"向ULog输入的宽字符指针为空。");
    message_->stm_<<ws2s(t,loc_);
    return mayHasDelim();
}
#endif

std::string ULog::message()
{
    return message_->stm_.str();
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

void ULogDumpMemory( ULog &log,const char *address,int len )
{
    log<<"dumpmem("<<(void *)address<<","<<len<<")\r\n";
    char *beginAddress = const_cast<char *>(address);//reinterpret_cast<char *>(address);
    stringstream dumpInfo;
    stringstream dumpMessage;
    for(int i = 0; i < len; i++)
    {
        char tempBuf[255] = "";
        _itoa_s(i,tempBuf,255,10);
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
        dumpInfo<<" "<<hex<<noshowbase<<uppercase
            <<setw(2)<<setfill('0')<<charValue;

        char word = GetAt<char>(beginAddress,i);
        if(word == '\0')
        {
            //dumpMessage<<"0"<<setw(2)<<"\\0";
            dumpMessage<<setw(3)<<setfill(' ')<<"\\0"<<setw(0);
        }
        else if(word == '\r')
        {
            dumpMessage<<setw(3)<<setfill(' ')<<"\\r"<<setw(0);
        }
        else if(word == '\n')
        {
            dumpMessage<<setw(3)<<setfill(' ')<<"\\n"<<setw(0);
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

void ULogHexDisp(ULog &log,int number)
{
    log.disableDelim()<<"0x"<<(void *)number;
    log.enableDelim();
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

ULog::LoggerAppender::LoggerAppender()
{
    //打开当前的日志文件,没有则创建.
    string logFileName = "ulog_";
    char currentTime[255] = "";
    time_t t;
    tm timeStruct;
    time(&t);
    localtime_s(&timeStruct,&t);
    strftime(currentTime,255,"%Y_%m_%d",&timeStruct);
    logFileName += currentTime;
    logFileName += ".log";
    logFile_.open(logFileName.c_str(),ios_base::out|ios_base::app);
    if(!logFile_.is_open())
    {
        DebugMessage("UniCore ULog::LoggerAppender::LoggerAppender() 无法打开日志文件.");
    }
}

ULog::LoggerAppender::~LoggerAppender()
{
    logFile_.close();
}

void ULog::LoggerAppender::append( Message *message )
{
    if(logFile_)
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
        time_t t;
        tm timeStruct;
        time(&t);
        localtime_s(&timeStruct,&t);
        char currentTime[255] = "";
        if(asctime_s(currentTime,&timeStruct) != 0)
        {
            DebugMessage("UniCore ULog::LoggerAppender::append( Message *message ) asctime_s失败.");
        }
        logFile_<<currentTime<<" "
            <<"{"<<message->name_<<"}"
            <<"["<<type<<"]"
            <<"["<<message->func_<<"]"
            <<" "<<message->stm_.str()<<" "
            <<"<"<<message->line_<<">"<<endl;
    }
}

ULog::FileAppender::FileAppender(const wchar_t *fileName)
{
    file_.open(fileName,ios_base::out|ios_base::app);
    if(!file_.is_open())
    {
        assert(false && "UniCore ULog::FileAppender::FileAppender(const wchar_t *fileName) Open file failed.");
        DebugMessage("UniCore ULog::FileAppender::FileAppender(const wchar_t *fileName) 无法打开文件.");
    }
}

ULog::FileAppender::~FileAppender()
{
    file_.close();
}

void ULog::FileAppender::append( Message *message )
{
    if(file_)
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
        time_t t;
        tm timeStruct;
        time(&t);
        localtime_s(&timeStruct,&t);
        char currentTime[255] = "";
        if(asctime_s(currentTime,&timeStruct) != 0)
        {
            DebugMessage("UniCore ULog::LoggerAppender::append( Message *message ) asctime_s失败.");
        }
        file_<<currentTime<<" "
            <<"{"<<message->name_<<"}"
            <<"["<<type<<"]"
            <<"["<<message->func_<<"]"
            <<" "<<message->stm_.str()<<" "
            <<"<"<<message->line_<<">"<<endl;
    }
}


void ULog::ConsoleAppender::append( Message *message )
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

    string messageString = message->stm_.str();
    printf("{%s}[%s][%s]%s<%d>\n",
        message->name_.c_str(),type.c_str(),
        message->func_.c_str(),messageString.c_str(),
        message->line_);

}

}//namespace uni