#include "stdafx.h"

#include "../UniCore/UGeometry.h"
#include <cmath>

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

int round(float a)
{
	return (int)(a+0.5);
}

TEST(UGeometryTest,LineBezierIntersection_Works)
{
	UStraightLine lineA(53,329,401,371);
	UCubicBezierLine lineB(168,272,22,501,368,204,352,416);

	auto result = Intersect(lineA,lineB);

	ASSERT_EQ(3,result.size());
	ASSERT_EQ(134,round(result[0].x()));
	ASSERT_EQ(339,round(result[0].y()));
	ASSERT_EQ(346,round(result[1].x()));
	ASSERT_EQ(364,round(result[1].y()));
	ASSERT_EQ(217,round(result[2].x()));
	ASSERT_EQ(349,round(result[2].y()));

	//result t(0.109,0.484,0.896).
}