#include "stdafx.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../UniCore/URTTIParser.h"
#include "../UniCore/UProcessMemory.h"

using namespace uni;

class URTTITestBaseClass
{
public:
	virtual ~URTTITestBaseClass() {}
};

class URTTITestDerivedClass : public URTTITestBaseClass
{
public:
	int forTestingOnly;
};

//UTRRIParser parsePointer works.
TEST(URTTITest, URTTIParser_ParsePointer_Works)
{
	auto pm = std::make_shared<UProcessMemory>(GetCurrentProcessId());
	auto parser = URTTIParser::create(pm);
	URTTITestBaseClass *a = new URTTITestDerivedClass;
	parser->parsePointer((int64_t)a);
	delete a;
}