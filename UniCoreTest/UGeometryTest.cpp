#include "stdafx.h"

#include "../UniCore/UGeometry.h"

using namespace uni;

TEST(UGeometryTest,LineLineIntersection_Works)
{
	UStraightLine lineA(15,10,54,29);
	UStraightLine lineB(41,4,32,32);
	
	auto result = Intersect(lineA,lineB);

	//result(36.0,20.1)
	ASSERT_EQ(1,result.size());
	ASSERT_FLOAT_EQ(35.812351,result[0].x());
	ASSERT_FLOAT_EQ(20.139351,result[0].y());
}

TEST(UGeometryTest,LineBezierIntersection_Works)
{
}