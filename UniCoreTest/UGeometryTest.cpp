#include "stdafx.h"

#include "../UniCore/UGeometry.h"
#include <cmath>

using namespace uni;

//求直线交点可用.
TEST(UGeometryTest,LineLineIntersection_Works)
{
	StraightLine lineA(15,10,54,29);
	StraightLine lineB(41,4,32,32);
	
	auto result = Intersect(lineA,lineB);

	//result(36.0,20.1)
	ASSERT_EQ(1,result.size());
	ASSERT_FLOAT_EQ(35.812351,result[0].x);
	ASSERT_FLOAT_EQ(20.139351,result[0].y);
}

//平行的直线没有交点.
TEST(UGeometryTest,IntersectParallelLine_NoIntersection)
{
	StraightLine lineA(15,10,54,29);
	StraightLine lineB(30,20,69,39);

	auto result = Intersect(lineA,lineB);

	//result(36.0,20.1)
	ASSERT_EQ(0,result.size());
}

int round(float a)
{
	return (int)(a+0.5);
}

TEST(UGeometryTest,LineBezierIntersection_Works)
{
	StraightLine lineA(53,329,401,371);
	CubicBezierLine lineB(168,272,22,501,368,204,352,416);

	auto result = Intersect(lineA,lineB);

	ASSERT_EQ(3,result.size());
	ASSERT_EQ(346,round(result[0].x));
	ASSERT_EQ(364,round(result[0].y));
	ASSERT_EQ(134,round(result[1].x));
	ASSERT_EQ(339,round(result[1].y));
	ASSERT_EQ(217,round(result[2].x));
	ASSERT_EQ(349,round(result[2].y));

	//result t(0.109,0.484,0.896).
}


TEST(UGeometryTest,StraightLineProjection_Works)
{
	StraightLine line(2,2,5,5);
	Point p(0,2);
	Point point = line.getProjectionPoint(p);
	ASSERT_EQ(1,point.x);
	ASSERT_EQ(1,point.y);
}

//投射到水平线段.
TEST(UGeometryTest,StraightLineProjection_Works1)
{
	StraightLine line(2,2,8,2);
	Point p(0,2);
	Point point = line.getNearestPoint(p);
	EXPECT_EQ(2,point.x);
	EXPECT_EQ(2,point.y);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//投射到垂直线段.
TEST(UGeometryTest,StraightLineProjection_Works2)
{
	StraightLine line(2,2,2,-8);
	Point p(0,0);
	Point point = line.getProjectionPoint(p);
	EXPECT_EQ(2,point.x);
	EXPECT_EQ(0,point.y);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

TEST(UGeometryTest,StraightLineNearestPoint_Works)
{
	StraightLine line(2,2,5,5);
	Point p(0,2);
	Point point = line.getNearestPoint(p);
	ASSERT_EQ(2,point.x);
	ASSERT_EQ(2,point.y);
}

TEST(UGeometryTest,StraightLineProjection_Works3)
{
	StraightLine line(71.68,738.22,141.05,738.55);
	Point p(92.46,739.40);
	Point point = line.getProjectionPoint(p);
	ASSERT_GT(point.x,71.68);
	ASSERT_LT(point.x,141.05);
}

TEST(UGeometryTest,StraightLineProjection_Works4)
{
	StraightLine line(-4.78,-2.10,-7.34,1.68);
	Point p(-5.98,-2.43);
	Point point = line.getProjectionPoint(p);
	ASSERT_NEAR(-5.00,point.x,0.01);
	ASSERT_NEAR(-1.77,point.y,0.01);
}

TEST(UGeometryTest,CubicBezierLine_split_Works)
{
	CubicBezierLine line(120,160,35,200,220,260,220,40);
	auto result = line.split(0.1);
	EXPECT_EQ(2,result.size());
	EXPECT_NEAR(0.0f,result[0].originBeginT,0.001);
	EXPECT_NEAR(0.1f,result[0].originEndT,0.001);
	EXPECT_NEAR(0.1f,result[1].originBeginT,0.001);
	EXPECT_NEAR(1.0f,result[1].originEndT,0.001);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//直线可以写成Ax + By + C = 0.
//(y1 – y2)x + (x2 – x1)y + (x1y2 – x2y1) = 0
//A = y1 - y2; B = x2 - x1; C = x1y2 – x2y1;
TEST(UGeometryTest,StraightLine_GetABC_Works)
{
	StraightLine l(2,1,5,7);
	EXPECT_NEAR(-6,l.a(),0.001);
	EXPECT_NEAR(3,l.b(),0.001);
	EXPECT_NEAR(9,l.c(),0.001);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//贝塞尔曲线相交.
TEST(UGeometryTest,Bezier_Intersection_Works)
{
	CubicBezierLine a(10,100,90,30,40,140,220,240);
	CubicBezierLine b(5,150,180,20,80,280,210,190);
	std::vector<Point> result = IntersectBezierAndBezierLine(a,b);
	EXPECT_EQ(3,result.size());
	float x1 = a.getX(0.895);
	float y1 = a.getY(0.895);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}


//tightBoundingRect可用.
TEST(UGeometryTest,tightBoundingBox_Works)
{
	CubicBezierLine a(10,100,90,30,40,140,220,240);
	XRect rect = a.tightBoundingBox();

	EXPECT_GT(rect.points[0].x,0);
	EXPECT_GT(rect.points[0].y,0);
	EXPECT_GT(rect.points[1].x,0);
	EXPECT_GT(rect.points[1].y,0);
	EXPECT_GT(rect.points[2].x,0);
	EXPECT_GT(rect.points[2].y,0);
	EXPECT_GT(rect.points[3].x,0);
	EXPECT_GT(rect.points[3].y,0);

	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//直线的Bounding Box.
TEST(UGeometryTest,StraightLine_BoundingBox_Ok)
{
	StraightLine line(2,-2,-5,0);
	EXPECT_EQ(-5,line.boundingBox().l);
	EXPECT_EQ(-2,line.boundingBox().t);
	EXPECT_EQ(2,line.boundingBox().r);
	EXPECT_EQ(0,line.boundingBox().b);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}