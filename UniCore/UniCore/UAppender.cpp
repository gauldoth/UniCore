#include "UAppender.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

using namespace std;

namespace uni
{

void UConsoleAppender::append(const string &msg)
{
    printf("%s\n",msg.c_str());
}

void UDebuggerAppender::append(const string &msg)
{
    OutputDebugStringA(msg.c_str());
}

UFileAppender::UFileAppender(const string &file)
{
    ftm_.open(file.c_str(),ios_base::out|ios_base::app);
}

UFileAppender::~UFileAppender()
{
    ftm_.close();
}

void UFileAppender::append(const std::string &msg)
{
    ftm_<<msg<<"\r\n";
}

}//namespace uni