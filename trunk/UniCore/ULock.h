/*! \file ULock.h
    \brief Windows下简单的锁.

    \author unigauldoth@gmail.com
    \date       2013-6-9
*/
#ifndef UNICORE_ULOCK_H
#define UNICORE_ULOCK_H

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace uni
{
//! 简单的锁.
class ULock
{
public:
    ULock():atomic_(0) {}
    ~ULock() {}
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
    ULock(const ULock&);
    ULock& operator=(const ULock&);
    volatile long atomic_;
};

//! 区域锁.
/*!
    声明的时候加锁,变量销毁时(退出作用域)解锁.
*/
class UScopedLock
{
public:
    explicit UScopedLock(ULock &ulock)
        :lock_(ulock)
    {
        lock_.lock();
    }
    ~UScopedLock()
    {
        lock_.unlock();
    }
private:
    UScopedLock(const UScopedLock&);
    UScopedLock& operator=(const UScopedLock&);
    ULock &lock_;
};

}//namespace uni

#endif//UNICORE_ULOCK_H