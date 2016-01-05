#ifndef UNICORE_RTTIINFO_H
#define UNICORE_RTTIINFO_H

#include <map>
#include <vector>
#include <memory>

namespace uni
{

//! Class info.
class URTTIClassDesc
{
public:
	std::string name;
	std::vector<std::weak_ptr<URTTIClassDesc> > baseClasses;
};

//! Record all available rtti info.
class URTTIInfo
{
public:
	//! Pointer to class info mapping.
	std::map<void *,URTTIClassDesc *> objects;
	//! Record all classes info.
	std::vector<std::shared_ptr<URTTIClassDesc> > classes;
};


}//namespace uni

#endif//UNICORE_RTTIINFO_H