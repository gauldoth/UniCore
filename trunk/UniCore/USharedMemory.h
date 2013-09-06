/*! \file USharedMemory.h
    \brief 简单的共享内存类.
    \date   2013-8-29
    \author uni(unigauldoth@gmail.com)
*/
#ifndef UNICORE_USHAREDMEMORY_H
#define UNICORE_USHAREDMEMORY_H

#include <string>
#include <cassert>
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

namespace uni
{

struct Commu
{
    long __int32 lock;
    __int32 eventOffset;
    __int32 pairOffset;
};

class UMemoryManager
{
public:
    virtual ~UMemoryManager() {}
    virtual __int64 allocMemory(int size) = 0;
    virtual bool freeMemory(__int64 address) = 0;
    virtual __int64 fixedMemory() = 0;
    virtual __int64 fixedMemorySize() = 0;
    virtual bool isValid() {return true;}
};

struct Block
{
    __int32 size;  // available buf size, not including the "size" and "free" field.
    __int32 free;
    char buf[1];
};

class USharedMemoryManager : public UMemoryManager
{
public:
    enum {SharedMemorySize = 0x1000};
    USharedMemoryManager()
    {
        hFileMapping_ = 
            CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE|SEC_COMMIT,0,SharedMemorySize,NULL);
        assert(hFileMapping_ != NULL);
        baseAddress_ = MapViewOfFile(hFileMapping_,FILE_MAP_WRITE,0,0,0);
        assert(baseAddress_ != NULL);
        
        Block *firstBlock = reinterpret_cast<Block *>((__int64)baseAddress_ + sizeof(Commu));
        firstBlock->size = SharedMemorySize - sizeof(Commu);
        firstBlock->free = true;
    }
    USharedMemoryManager(HANDLE hFileMapping)
        :hFileMapping_(hFileMapping)
    {
        assert(hFileMapping_ != NULL);
        baseAddress_ = MapViewOfFile(hFileMapping,FILE_MAP_WRITE,0,0,0);
        assert(baseAddress_ != NULL);
    }
    virtual __int64 allocMemory(int size)
    {
        if(size <= 0)
        {
            return 0;
        }
        Block *block = getFreeBlock(size);
        if(block)
        {
            Block *allocBlock = splitBlock(block,size);
            if(allocBlock)
            {
                return (__int64)&allocBlock->buf[0];
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    virtual bool freeMemory(__int64 address)
    {
        Block *blockToFree = (Block *)(address-sizeof(__int32)*2);
        Block *block = getFirstBlock();
        Block *prevBlock = 0;
        Block *nextBlock = 0;
        while(block)
        {
            if(block == blockToFree)
            {
                nextBlock = getNextBlock(block);
                break;
            }
            prevBlock = block;
            block = getNextBlock(block);
        }
        if(!block)
        {
            return false;
        }
        if(prevBlock && prevBlock->free)
        {
            prevBlock->size += block->size + sizeof(__int32)*2;
            block = prevBlock;
        }

        if(nextBlock && nextBlock->free)
        {
            block->size += nextBlock->size + sizeof(__int32)*2;
        }

        block->free = true;
        return true;
    }
    virtual __int64 fixedMemory()
    {
        return reinterpret_cast<__int64>(baseAddress_);
    }
    virtual __int64 fixedMemorySize()
    {
        return sizeof(Commu);
    }
    virtual bool isValid()
    {
        return hFileMapping_ != NULL && baseAddress_ != NULL;
    }
private:
    Block *getFreeBlock(int minimumSize)
    {
        Block *block = getFirstBlock();
        while(block && block->size < minimumSize+sizeof(__int32)*2)
        {
            block = getNextBlock(block);
        }
        if(block)
        {
            return block;
        }
        else
        {
            return 0;
        }
    }
    Block *getNextBlock(Block *block)
    {
        if(block)
        {
            __int64 nextBlockAddress = (__int64)&(block->buf[0]) + block->size;
            if(nextBlockAddress >= (__int64)baseAddress_+SharedMemorySize)
            {
                return 0;
            }
            else
            {
                return reinterpret_cast<Block *>(nextBlockAddress);
            }
        }
        return 0;
    }
    Block *getFirstBlock()
    {
        return reinterpret_cast<Block *>((__int64)baseAddress_ + sizeof(Commu));
    }
    Block *splitBlock(Block *block,__int64 size)
    {
        __int32 remainBlockSize = block->size - size - sizeof(__int32)*2;
        if(remainBlockSize >= 0x10)
        {
            block->size = size;
            Block *remainBlock = (Block *)((__int64)&(block->buf[0]) + block->size);
            remainBlock->size = remainBlockSize;
            remainBlock->free = true;
        }
        else
        {
            block->free = false;
        }
        return block;
    }
    HANDLE hFileMapping_;
    void *baseAddress_;
};



struct Pair
{
    __int32 keyOffset;
    __int32 dataOffset;
    __int32 nextOffset;
};

struct Event
{
    __int32 keyOffset;
    long __int32 event;
    __int32 nextOffset;
};

class USharedMemory
{
public:
    class ScopedLock
    {
    public:
        ScopedLock(volatile long __int32 *atomic)
            :atomic_(atomic)
        {
            do 
            {
                long prev = InterlockedCompareExchange(atomic_,1,0);
                if(*atomic_ == 1 && prev == 0)
                {
                    break;
                }
                if(!SwitchToThread())
                {
                    Sleep(1);
                }
            } while (true);
        }
        ~ScopedLock()
        {
            InterlockedCompareExchange(atomic_,0,1);
        }
        volatile long __int32 *atomic_;
    };
    USharedMemory(UMemoryManager &memoryManager)
        :memoryManager_(memoryManager)
    {
        commu_ = reinterpret_cast<Commu *>(memoryManager.fixedMemory());
    }
    void setData(const std::wstring &key, const std::wstring &data) 
    {
        ScopedLock lock(&commu_->lock);
        Pair * pair = findPair(key);
        if(pair)
        {
            setPair(pair,data);
        }
        else
        {
            addPair(key,data);
        }
    }
    void setIntData(const std::wstring &key, int data) 
    {
        wchar_t buf[255] = L"";
        _itow_s(data,buf,10);
        setData(key,buf);
    }
    std::wstring data(const std::wstring &key) 
    {
        ScopedLock lock(&commu_->lock);
        Pair *pair = findPair(key);
        if(pair)
        {
            return getStringByAddress(((__int64)pair)+pair->dataOffset);
        }
        return std::wstring();
    }
    int intData(const std::wstring &key) 
    {  
        return _wtoi(data(key).c_str());
    }
    void WaitEvent(const std::wstring &event) 
    {
        volatile long *eventAtom = getEvent(event);
        do 
        {
            long prev = InterlockedCompareExchange(eventAtom,0,1);
            if(*eventAtom == 0 && prev == 1)
            {
                break;
            }
            if(!SwitchToThread())
            {
                Sleep(1);
            }
        } while (true);
    }

    void NotifyEvent(const std::wstring &event) 
    {
        volatile long *eventAtom = getEvent(event);
        InterlockedCompareExchange(eventAtom,1,0);
    }
    bool WaitEvent(const std::wstring &event, int waitTime)
    {
        volatile long *eventAtom = getEvent(event);
        int startTime = GetTickCount();
        do
        {
            long prev = InterlockedCompareExchange(eventAtom,0,1);
            if(*eventAtom == 0 && prev == 1)
            {
                return true;
            }
            if(GetTickCount() - startTime >= waitTime)
            {
                return false;
            }
            if(!SwitchToThread())
            {
                Sleep(1);
            }
        } while (true);
        return false;
    }
    bool WaitEvent(const std::wstring &event, bool(*canExit)()) 
    {
        volatile long *eventAtom = getEvent(event);
        int startTime = GetTickCount();
        do 
        {
            long prev = InterlockedCompareExchange(eventAtom,0,1);
            if(*eventAtom == 0 && prev == 1)
            {
                return true;
            }
            if(canExit())
            {
                return false;
            }
            if(!SwitchToThread())
            {
                Sleep(1);
            }
        } while (true);
        return false;
    }

    volatile long *getEvent(const std::wstring &key)
    {
        ScopedLock lock(&commu_->lock);
        __int64 currentAddress = (__int64)&commu_->eventOffset;
        __int32 nextOffset = commu_->eventOffset;
        while(nextOffset)
        {
            currentAddress = currentAddress + nextOffset;
            if(getStringByAddress(currentAddress + ((Event *)currentAddress)->keyOffset)
                == key)
            {
                return &(((Event *)currentAddress)->event);
            }
            nextOffset = ((Event*)currentAddress)->nextOffset;
        }

        // Create a new event.
        __int64 address = memoryManager_.allocMemory(sizeof(Event));
        int keyAddress = newString(key);
        ((Event *)address)->keyOffset = keyAddress - address;
        ((Event *)address)->event = 0;
        ((Event *)address)->nextOffset = 0;
        if(currentAddress == (__int64)&commu_->eventOffset)
        {
            commu_->eventOffset = address - currentAddress;
        }
        else
        {
            ((Event *)currentAddress)->nextOffset = address - currentAddress;
        }
        return &(((Event *)address)->event);

    }

    __int64 newString(const std::wstring &key) 
    {
        int allocSize = key.length()*sizeof(wchar_t)+sizeof(__int32);
        __int64 address = memoryManager_.allocMemory(allocSize);
        *(__int32 *)address = key.length();
        memcpy_s(reinterpret_cast<void *>(address+sizeof(__int32)),allocSize-sizeof(__int32),key.c_str(),key.length()*sizeof(wchar_t));
        return address;
    }
    std::wstring getStringByAddress(__int64 address)
    {
        __int32 size = *(__int32 *)address;
        std::wstring result;
        result.assign((const wchar_t *)(address+sizeof(__int32)),size);
        return result;
    }
    Pair *findPair(const std::wstring &key)
    {
        __int64 currentAddress = (__int64)&commu_->pairOffset;
        __int32 nextOffset = commu_->pairOffset;
        while(nextOffset)
        {
            currentAddress = currentAddress + nextOffset;
            if(getStringByAddress(currentAddress + ((Pair *)currentAddress)->keyOffset)
                == key)
            {
                return (Pair *)currentAddress;
            }
            nextOffset = ((Pair*)currentAddress)->nextOffset;
        }

        return 0;
    }
    void addPair(const std::wstring &key, const std::wstring &data)
    {
        __int64 currentAddress = (__int64)&commu_->pairOffset;
        __int64 newPair = memoryManager_.allocMemory(sizeof(Pair));
        ((Pair *)newPair)->keyOffset = newString(key) - newPair;
        ((Pair *)newPair)->dataOffset = newString(data) - newPair;
        ((Pair *)newPair)->nextOffset = 0;
        if(commu_->pairOffset)
        {
            ((Pair *)newPair)->nextOffset = currentAddress +commu_->pairOffset-newPair;
        }
        commu_->pairOffset = newPair - currentAddress;
    }
    void setPair(Pair *pair,const std::wstring &data)
    {
        memoryManager_.freeMemory((__int64)pair+pair->dataOffset);
        pair->dataOffset = newString(data) - (__int64)pair;
    }
    Commu *commu_;
    UMemoryManager &memoryManager_;
private:
    USharedMemory(const USharedMemory &);
    USharedMemory &operator = (const USharedMemory &);
};

}//namespace uni

#endif//UNICORE_USHAREDMEMORY_H