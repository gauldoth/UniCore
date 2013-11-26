/*! \file UCommon.h
    \brief 包含了一些常用的函数。

    \date       2011-8-5
*/
#ifndef UNICORE_UCOMMON_H
#define UNICORE_UCOMMON_H

#include <string>
#include <vector>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

/*! \def DISALLOW_COPY_AND_ASSIGN
    该宏用于禁止类拷贝复制。
    \code
    class Server
    {
    public:
        Server();
        ~Server();
    private:
        SOCKET socket;
        DISALLOW_COPY_AND_ASSIGN(Server);  //在private处加入该宏以禁止类Server复制，Server无法被放入标准库容器。
    };
    \endcode
*/
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    TypeName& operator=(const TypeName&);

/*! \def USE_DEFAULT_COPY
    类Class使用编译器自动生成的拷贝构造函数和赋值符号。
    \code
    class Server
    {
    public:
        Server();
        ~Server();
    private:
        SOCKET socket;
        USE_DEFAULT_COPY(Server);  //在private处加入该宏以禁止类Server复制，Server无法被放入标准库容器。
    };
*/
#define USE_DEFAULT_COPY(Class);

namespace uni
{
//! 获得一个随机的角色名字。
/*! 
    \param minSize 名字允许的最小长度，假如生成的名字长度小于该值，则重新生成。
    假如为0（默认值）则表示不限制最小长度。
    \param maxSize 名字允许的最大长度，假如生成的名字长度大于该值，则重新生成。
    假如为0（默认值）则表示不限制最大长度。
    \param wordCount 名字所使用的单词个数.
    \param randomSuffixCount 名字末尾添加的随机数字的位数.
    例如randomSuffixCount = 3,则生成Apple251,Xore546之类的名字,后面附加3个随机数字.
    \return 随机的角色名。
*/
std::string GetRandomRoleName(int minSize = 0,int maxSize = 0,int wordCount = 1,int randomSuffixCount = 0);

//! 分割字符串.
/*!
    \param s 源字符串,被分割的字符串.
    \param delim 分隔符.这里的分隔符可能不止一个字符,而可以是字符串.
	\return 分割后的子字符串组成的数组.

    分割字符串s.每遇到一次分隔符就生成一个子字符串并保存到结果数组中,
	空的子字符串也会被保存.

    \code
    std::string s = "Acc Psw Credit";
    std::vector<std::string> results = split(s);
	//以空格分割字符串s,results将包含"Acc","Psw","Credit"三个字符串.
    \endcode
*/
std::vector<std::string> split(const std::string &s,const std::string &delim = " ");

//! 分割字符串.
/*!
    \param s 源字符串,被分割的字符串.
    \param delim 分隔符.这里的分隔符可能不止一个字符,而可以是字符串.
	\return 分割后的子字符串组成的数组.

    分割字符串s.每遇到一次分隔符就生成一个子字符串并保存到结果数组中,
	空的子字符串也会被保存.

    \code
    std::wstring s = L"Acc Psw Credit";
    std::vector<std::wstring> results = split(s);
	//以空格分割字符串s,results将包含L"Acc",L"Psw",L"Credit"三个字符串.
    \endcode
*/
std::vector<std::wstring> split(const std::wstring &s,const std::wstring &delim = L" ");

//! 拼接字符串.
/*!
    \param stringsToJoin 要拼接的字符串数组.
    \param delim 拼接使用的分隔符.
    \return 拼接好的字符串.
*/
std::string join(const std::vector<std::string> &stringsToJoin,const std::string &delim);

//! 拼接字符串.
/*!
    \param stringsToJoin 要拼接的字符串数组.
    \param delim 拼接使用的分隔符.
    \return 拼接好的字符串.
*/
std::wstring join(const std::vector<std::wstring> &stringsToJoin,const std::wstring &delim);

//! 从字符串的首尾剔除指定字符.
/*!
    \param s 处理的字符串.
    \paran trim 要剔除的字符集合,默认为空格.
    该函数从s的首尾剔除字符,任何出现在字符串两端的trimChars中包含的字符都会被剔除.
    s为空字符串时也将返回空字符串.
    \code
    std::string s = "    this is a string with space.    ";
    std::string trimed = trim(s);  //trimed: "this is a string with space."
    trimed = trim(s,". ");  //trimed: "this is a string with space"  这里空格和'.'都被剔除了.
    \endcode
*/
std::string trim(const std::string &s,const std::string &trimChars = " ");

//! 从字符串的首尾剔除指定字符.
/*!
    \param s 处理的字符串.
    \paran trim 要剔除的字符集合,默认为空格.
    该函数从s的首尾剔除字符,任何出现在字符串两端的trimChars中包含的字符都会被剔除.
    s为空字符串时也将返回空字符串.
    \code
    std::wstring s = L"    this is a string with space.    ";
    std::wstring trimed = trim(s);  //trimed: L"this is a string with space."
    trimed = trim(s,L". ");  //trimed: L"this is a string with space"  这里空格和'.'都被剔除了.
    \endcode
*/
std::wstring trim(const std::wstring &s,const std::wstring &trimChars = L" ");

enum CaseSensitivity
{
    CaseInsensitive = 0,
    CaseSensitive = 1,
};

//! 查看字符串中是否包含pattern.
/*!
    \param src 源字符串.
    \param pattern 要查找的字符串.
    \param case 是否大小写敏感.
    \return 是否包含pattern.
*/
bool contains(const std::wstring &src, const std::wstring &pattern, CaseSensitivity caseSensitivity = CaseSensitive);

//! 查看字符串中是否包含pattern.
/*!
    \param src 源字符串.
    \param pattern 要查找的字符串.
    \param case 是否大小写敏感.
    \return 是否包含pattern.
*/
bool contains(const std::string &src, const std::string &pattern, CaseSensitivity caseSensitivity = CaseSensitive);

//! 字符串是否以pattern开始.
/*!
    \param src 源字符串.
    \param pattern 是否以pattern开始.
    \param caseSensitivity 是否大小写敏感, 默认大小写敏感.
    \return 是否以字符串pattern开始.

    \code
    std::wstring src = L"hello world!";
    if(starts_with(src,L"hello"))
    {
        printf("\"hello world!\"  starts with \"hello\"");
    }
    \endcode
*/
bool starts_with(const std::wstring &src, const std::wstring &pattern, CaseSensitivity caseSensitivity = CaseSensitive);

//! 字符串是否以pattern开始.
/*!
    \param src 源字符串.
    \param pattern 是否以pattern开始.
    \param caseSensitivity 是否大小写敏感, 默认大小写敏感.
    \return 是否以字符串pattern开始.

    \code
    std::string src = "hello world!";
    if(starts_with(src,"hello"))
    {
        printf("\"hello world!\"  starts with \"hello\"");
    }
    \endcode
*/
bool starts_with(const std::string &src, const std::string &pattern, CaseSensitivity caseSensitivity = CaseSensitive);


}//namespace uni

#endif//UNICORE_UCOMMON_H