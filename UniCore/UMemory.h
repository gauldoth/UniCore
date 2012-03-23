/*! \file UMemory.h
    \brief 内存操作相关。

    \date 2011-9-5
*/
#ifndef UNICORE_UMEMORY_H
#define UNICORE_UMEMORY_H

#include <assert.h>

namespace uni
{

//! 内存取值。
template<typename RetType,typename BaseType>
inline RetType &GetAt(BaseType base,int offset)
{
    char *basePtr = (char *)base;
    basePtr += offset;
    return *reinterpret_cast<RetType *>(basePtr);
}

//! 内存取值，取2次偏移。
template<typename RetType,typename BaseType>
inline RetType &GetAt(BaseType base,int offset1,int offset2)
{
    char *basePtr = (char *)base;
    basePtr += offset1;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset2;
    return *reinterpret_cast<RetType *>(basePtr);
}

//! 内存取值，取3次偏移。
template<typename RetType,typename BaseType>
inline RetType &GetAt(BaseType base,int offset1,int offset2,int offset3)
{
    char *basePtr = (char *)base;
    basePtr += offset1;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset2;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset3;
    return *reinterpret_cast<RetType *>(basePtr);
}

//! 内存取值，取4次偏移。
template<typename RetType,typename BaseType>
inline RetType &GetAt(BaseType base,int offset1,int offset2,int offset3,int offset4)
{
    char *basePtr = (char *)base;
    basePtr += offset1;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset2;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset3;
    basePtr = *reinterpret_cast<char **>(basePtr);
    basePtr += offset4;
    return *reinterpret_cast<RetType *>(basePtr);
}

//! 内存取值，取任意次。
/*!
    \param base 基地址。
    \param offsets 该数组保存了所有的偏移。
    \param count offsets所指定的int数组中保存了多少个偏移。
*/
template<typename RetType,typename BaseType>
inline RetType &GetAt(BaseType base,int offsets[],int count)
{
    assert(offsets);
    int result = 0;
    char *basePtr = (char *)base;
    for(int i = 0; i < count; i++)
    {
        basePtr += offsets[i];
        if(i < count - 1)
        {
            basePtr = *reinterpret_cast<char **>(basePtr);
        }
    }

    return *reinterpret_cast<RetType *>(basePtr);
}

}//namespace uni

#endif//UNICORE_UMEMORY_H