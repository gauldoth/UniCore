#include "URTTIParser.h"
#include "UProcessMemory.h"
#include <cstdint>
#include "UMemory.h"

namespace uni
{


std::shared_ptr<URTTIParser> URTTIParser::create(std::shared_ptr<UProcessMemory> pm)
{
	return std::make_shared<URTTIParser>(pm);
}

URTTIParser::URTTIParser(std::shared_ptr<UProcessMemory> pm)
	:pm_(pm)
{

}

void URTTIParser::parse()
{
	for(int i = 0; i < pm_->regionCount(); i++)
	{
		auto region = pm_->region(i);
		for(int64_t j = region.base; j < region.size; j++)
		{
			parsePointer(j);
		}
	}
}

void URTTIParser::parsePointer(int64_t p)
{
	//Get VFTablePtr.
	int64_t vfTablePtr = 0;
	bool ok = false;
	if(pm_->is64Bit)
	{
		vfTablePtr = pm_->getAt<int64_t>(p,0,ok);
	}
	else
	{
		vfTablePtr = pm_->getAt<int>(p,0,ok);
	}

	if(!ok || !vfTablePtr)
	{
		return;
	}

	//Get pCompleteObjectLocator
	int64_t pCompleteObjectLocator = 0;
	if(pm_->is64Bit)
	{
		pCompleteObjectLocator = pm_->getAt<int64_t>(vfTablePtr,-4,ok);
	}
	else
	{
		pCompleteObjectLocator = pm_->getAt<int>(vfTablePtr,-4,ok);
	}
	
	if(!ok || !pCompleteObjectLocator)
	{
		return;
	}

	//Get pTypeDescriptor(type_info *)
	std::type_info *ti = 0;
	if(pm_->is64Bit)
	{
		ti = pm_->getAt<std::type_info *>(pCompleteObjectLocator,12,ok);
	}
	else
	{
		ti = pm_->getAt<std::type_info *>(pCompleteObjectLocator,12,ok);
	}
	if(!ok || !ti)
	{
		return;
	}

	//32位下,4字节虚函数表指针,4字节void *_M_data;
	//之后是一个char数组.
	std::string rttiBuf;
	char buf[8] = "";  

	pm_->getAt(ti,0,buf,8,ok);
	if(!ok || GetAt<int>(buf,0) == 0)  //type_info member should not be zero.
	{
		return;
	}

	rttiBuf.append(buf,8);

	//读取后续的字符串.
	char c = 0;
	int i = 0;
	while(true)
	{
		c = pm_->getAt<char>(ti,8+i,ok);
		if(!ok)
		{
			return;
		}
		rttiBuf.push_back(c);
		if(c == 0)
		{
			break;
		}

		i++;
	}

	if(rttiBuf.find(".?AV") == std::string::npos)
	{
		return;
	}

	//call name method to get the name.
	std::type_info *tiNew = (std::type_info *)rttiBuf.c_str();
	try
	{
		const char *className = tiNew->name();
		if(className == 0)
		{
			
		}
		
	}
	catch (...)
	{

	}

}

std::shared_ptr<URTTIInfo> URTTIParser::info()
{
	return std::make_shared<URTTIInfo>();
}

}//namespace uni