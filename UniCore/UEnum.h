/*! \file UEnum.h
    \brief 提供枚举值到字符串的映射。

    \author     uni
    \date       2013-10-25
*/
#ifndef UNICORE_UENUM_H
#define UNICORE_UENUM_H

#include <map>

static std::map<std::string,std::map<int,std::wstring> > g_enum_string_datas;

template<typename T>
inline void populate_enum_strings()
{

}

#define UENUM_BEGIN(enum_name) \
    template<>\
    inline void populate_enum_strings<enum_name>() \
    { \
    std::string name = "enum "; \
    name += #enum_name; \
    if(g_enum_string_datas.count(name)) \
    { \
        return; \
    } 

#define UENUM_ENTRY(entry) \
    g_enum_string_datas[name][entry] = L#entry; 

#define UENUM_ENTRY_VALUE(entry,value) \
    g_enum_string_datas[name][entry] = L#value;

#define UENUM_END \
    }

template<typename enum_name>
inline const std::wstring e2s(enum_name e)
{
    populate_enum_strings<enum_name>();
    return g_enum_string_datas[typeid(enum_name).name()][e].c_str();
}

#endif//UNICORE_UENUM_H