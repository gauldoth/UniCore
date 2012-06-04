#include "UBuffer.h"

#include "assert.h"
#include <boost/lexical_cast.hpp>

#include "UCast.h"
#include "ULog.h"

using namespace boost;

namespace uni
{

void UBuffer::appendHexPattern( const std::string &pattern )
{
    std::string word;  //保存从pattern分离出来的单个16进制数据，如"00"，"A3"之类。 
    for(int i = 0; i < pattern.size(); i++)
    {
        if(isalnum(pattern[i]))
        {
            assert(pattern[i] != ' ' && pattern[i] != '|');
            assert(word.size() < 2);
            word.push_back(pattern[i]);
            
            //向word加入字符后，word已经包含2个字符，将word代表的16进制数据存入缓冲区中。
            if(word.size() == 2)
            {
                short data = strtol(word.c_str(),0,16);
                appendShort(data);
                //准备下个数据的输入。
                word.clear();
            }
        }
        else
        {
            //不是字母或数字。
            //假如word中已经有字符，那么将word代表的16进制数据存入缓冲区中。
            assert(word.size() < 2);
            if(!word.empty())
            {
                short data = strtol(word.c_str(),0,16);
                appendShort(data);
                //准备下个数据的输入。
                word.clear();
            }
        }

    }
}

}//namespace uni