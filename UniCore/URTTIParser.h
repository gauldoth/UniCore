#ifndef UNICORE_RTTIPARSER_H
#define UNICORE_RTTIPARSER_H
#include <cstdint>
#include <memory>
#include "UProcessMemory.h"
#include "URTTIInfo.h"

namespace uni
{

class URTTIParser
{
public:
	static std::shared_ptr<URTTIParser> create(std::shared_ptr<UProcessMemory> pm);
	URTTIParser(std::shared_ptr<UProcessMemory> pm);
	virtual void parse();
	virtual void asyncParse(){}
	virtual void parsePointer(int64_t p);
	virtual std::shared_ptr<URTTIInfo> info();
private:
	std::shared_ptr<UProcessMemory> pm_;

};


}//namespace uni


#endif//UNICORE_RTTIPARSER_H