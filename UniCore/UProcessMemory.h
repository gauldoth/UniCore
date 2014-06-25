/*! \file UProcessMemory.h
    \brief 用于读取和修改进程内存.

    \author unigauldoth@gmail.com
    \date   2014-4-24
*/
#ifndef UNICORE_UPROCESS_MEMORY_H
#define UNICORE_UPROCESS_MEMORY_H



class UProcessMemory
{
public:
    class Block
    {
    public:

    };
    //构造的时候就把Block初始化好.
    //但只记录范围,并未读入内存.
    explicit UProcessMemory(int pid);
    virtual ~UProcessMemory();
    template<typename RetType, typename BaseType>
    RetType getAt(BaseType base, int offset)
    {
        //查看当前地址所在内存块是否可读.

        //可读的话,查看内存块是否加载,未加载则加载.

        //读取内存并返回.
    }
    template<typename SetType, typename BaseType>
    void setAt(BaseType base, int offset, SetType value);
private:
    //UProcessMemoryBlock block;
};

#endif//UNICORE_UPROCESS_MEMORY_H