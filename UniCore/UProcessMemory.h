/*! \file UProcessMemory.h
    \brief 用于读取,写入指定进程的内存.

    \author uni
    \date 2016-1-2
*/
#ifndef UNICORE_PROCESSMEMORY_H
#define UNICORE_PROCESSMEMORY_H

#include <cassert>
#include <vector>
#include <memory>
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "UProcess.h"
#include "ULog.h"
#include <cstdint>


namespace uni
{


class UProcessMemoryRegion
{
public:
	int64_t base;
	int64_t size;

};


//! 用于读取,写入指定进程的内存.
class UProcessMemory
{
public:
	//! Create UProcessMemory which attached to a process.
	UProcessMemory(int pid)
	{
		EnableDebugPrivilege();
		hProcess_ = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
		if(hProcess_ == NULL)
		{
			throw std::exception("OpenProcess failed.");
		}

		SYSTEM_INFO systemInfo = {0};
		GetNativeSystemInfo(&systemInfo);
		pageSize_ = systemInfo.dwPageSize;

		//Check the process is 64bit or 32bit.
		if(GetProcessBitness(pid) == 32)
		{
			is64Bit = false;
			//Scan the memory.
			MEMORY_BASIC_INFORMATION pbi;
			char *address = 0;
			while(true)
			{
				SIZE_T numOfBytes = VirtualQueryEx(hProcess_,address,&pbi,sizeof(pbi));
				if(!numOfBytes)
				{
					break;
				}
				assert(pbi.RegionSize);
				address += pbi.RegionSize;
				if(pbi.Protect == PAGE_NOACCESS)
				{
					continue;
				}
				UINFO<<delim<<"current address:"<<hexdisp((int)pbi.BaseAddress)
					<<"size:"<<pbi.RegionSize<<"protect:"<<pbi.Protect;

				UProcessMemoryRegion region;
				region.base = (int64_t)pbi.BaseAddress;
				region.size = pbi.RegionSize;
				regions_.push_back(region);
			}
		}

	}
	virtual ~UProcessMemory()
	{
		CloseHandle(hProcess_);
	}
	UProcessMemoryRegion region(int i) {return regions_[i];}
	int regionCount() {return regions_.size();}

	template<typename BaseType>
	void getAt(BaseType base,int64_t offset,char *buf,int64_t size,bool &ok)
	{
		ok = false;
		int64_t basePtr = (int64_t)base;
		//假如basePtr+offset可读,则读取数据并返回.
		//获得basePtr+offset所在区块.
		int64_t beginAddress = basePtr+offset;
		int64_t endAddress = beginAddress+size;  //读到endAddress前一个字节.
		int64_t blockStart = beginAddress/pageSize_;  //粒度为PageSize.
		int64_t blockEnd = endAddress/pageSize_;  //需要读取的内存在哪个内存块结束.

		for(int64_t blockIndex = blockStart; blockIndex <= blockEnd; blockIndex++)
		{
			if(blocks_.count(blockIndex) == 0)
			{
				//该块内存未缓存.
				//检查区块是否可读,可读则初始化,不可读则设为0.
				bool found = false;
				for(int i = 0; i < regions_.size(); i++)
				{
					if(blockIndex*pageSize_ >= regions_[i].base && (blockIndex+1)*pageSize_ < regions_[i].base+regions_[i].size)
					{
						found = true;
						break;
					}
				}

				if(found)
				{
					blocks_[blockIndex] = new char[pageSize_];
					BOOL ok = 
						ReadProcessMemory(hProcess_,(LPVOID)(blockIndex*pageSize_),
						blocks_[blockIndex],pageSize_,NULL);
					if(!ok)
					{
						throw std::exception("ReadProcessMemory failed.");
					}
				}
				else
				{
					blocks_[blockIndex] = 0;
					return;
				}
			}

			//读取内存.
			int64_t readBeginAddress = blockIndex*pageSize_;
			int64_t readEndAddress = (blockIndex+1)*pageSize_;
			if(beginAddress > readBeginAddress)
			{
				readBeginAddress = beginAddress;
			}
			if(endAddress < readEndAddress)
			{
				readEndAddress = endAddress;
			}
			int64_t readBytes = readEndAddress - readBeginAddress;

			//从缓存中读取数据.

			memcpy(buf,blocks_[blockIndex]+readBeginAddress%pageSize_,readBytes);
			buf += readBytes;
		}

		ok = true;
	}

	template<typename RetType,typename BaseType>
	RetType getAt(BaseType base, int64_t offset, bool &ok)
	{
		RetType retType;
		char *resultBuf = (char *)&retType;

		getAt(base,offset,resultBuf,sizeof(RetType),ok);

		return retType;
	}

	bool is64Bit;  //指定进程是否为64位.
private:
	std::vector<UProcessMemoryRegion> regions_;
	std::map<int64_t,char *> blocks_;
	HANDLE hProcess_;
	int64_t pageSize_;
};

}//namespace uni

#endif//UNICORE_PROCESSMEMORY_H