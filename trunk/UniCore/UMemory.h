/*! \file UMemory.h
    \brief 内存操作相关。

    \date 2011-9-5
*/
#ifndef UNICORE_UMEMORY_H
#define UNICORE_UMEMORY_H

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

}//namespace uni

#endif//UNICORE_UMEMORY_H