﻿/*! \file UMiniLog.h
    \brief 日志相关。

    使用MINILOG<<"asd",这种类似cout的方式记录日志.
    假如定义了DISABLE_MINILOG,则不会有日志产生.
	假如使用UMiniLog::setLogPath设置了日志路径,则日志将被写到该路径指定的文件中.

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


class UMiniLog
{
public:
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
	
	//! 设置日志路径.
	/*!
		返回值为设置前使用的路径.
		若path为0,则不会修改当前路径.
	*/
	static std::wstring setLogPath(const wchar_t *path)
	{
		static std::wstring s_path;

		if(!path)
		{
			return s_path;
		}

		std::wstring oldPath = s_path;
		s_path = path;
		return oldPath;
	}
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

				std::string message = "[";
				message += message_->func_;
				message += "]";

				message += " ";
				message += message_->stm_.str();
				message += " ";

				char buf[255] = "";
				_itoa_s(message_->line_,buf,10);
				message += "<"; message += buf; message += ">";

				std::wstring path = setLogPath(0);
				if(path.empty())
				{
					OutputDebugStringA(message.c_str());
				}
				else
				{
					//输出到指定路径.
					static std::ofstream file;
					if(!file.is_open())
					{
						file.open(path,std::ios_base::out|std::ios_base::trunc);
					}
					file<<message<<std::endl;
				}

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