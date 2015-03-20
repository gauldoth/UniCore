#include "stdafx.h"

#include "../UniCore/UGeometry.h"
#include <cmath>

using namespace uni;

//��ֱ�߽������.
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

//ֱ�߽���.
//lineA = {points=[2]({x=211.08000 y=591.17896 },{x=299.03000 y=642.29706 }) }
//lineB = {points=[2]({x=191.83101 y=629.68298 },{x=300.69000 y=573.91803 }) }
TEST(UGeometryTest,LineLineIntersection_Works2)
{
	StraightLine lineA(211.08000,591.17896,299.03000,642.29706);
	StraightLine lineB(191.83101,629.68298,300.69000,573.91803);

	auto result = Intersect(lineA,lineB);

	//result(36.0,20.1)
	ASSERT_EQ(1,result.size());
// 	ASSERT_FLOAT_EQ(35.812351,result[0].x);
// 	ASSERT_FLOAT_EQ(20.139351,result[0].y);
}

//ƽ�е�ֱ��û�н���.
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

//ֱ�ߺͱ����������ཻ.
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

//ֱ�ߺͱ����������ཻ2.
//straight = {points=[2]({x=407.88898 y=779.05499 },{x=407.88898 y=709.05499 }) }
//bezier = {points=[4]({x=371.71402 y=739.22205 },{x=397.60101 y=746.52502 },{x=429.13000 y=747.18896 },{x=469.28799 y=737.89404 }) }
TEST(UGeometryTest,LineBezierIntersection_Works2)
{
	StraightLine lineA(407.88898,779.05499,407.88898,709.05499);
	CubicBezierLine lineB(371.71402,739.22205,397.60101,746.52502,429.13000,747.18896,469.28799,737.89404);

	auto result = Intersect(lineA,lineB);

	ASSERT_EQ(1,result.size());
}

//ֱ�ߺͱ����������ཻ3.
//straight = {points=[2]({x=413.50000 y=645.75000 },{x=413.50000 y=571.75000 }) }
//bezier = {points=[4]({x=388.25000 y=638.00000 },{x=430.50000 y=625.75000 },{x=433.00000 y=586.50000 },{x=390.00000 y=574.25000 }) }
TEST(UGeometryTest,LineBezierIntersection_Works3)
{
	StraightLine lineA(413.50000,645.75000,413.50000,571.75000);
	CubicBezierLine lineB(388.25000,638.00000,430.50000,625.75000,433.00000,586.50000,390.00000,574.25000);

	auto result = Intersect(lineA,lineB);

	ASSERT_EQ(2,result.size());
}


TEST(UGeometryTest,StraightLineProjection_Works)
{
	StraightLine line(2,2,5,5);
	Point p(0,2);
	Point point = line.getProjectionPoint(p);
	ASSERT_EQ(1,point.x);
	ASSERT_EQ(1,point.y);
}

//Ͷ�䵽ˮƽ�߶�.
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

//Ͷ�䵽��ֱ�߶�.
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

//ֱ�߿���д��Ax + By + C = 0.
//(y1 �C y2)x + (x2 �C x1)y + (x1y2 �C x2y1) = 0
//A = y1 - y2; B = x2 - x1; C = x1y2 �C x2y1;
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

//�����������ཻ.
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

//lineA = {points=[4]({x=29.869799 y=749.17999 },{x=82.259300 y=769.42804 },{x=126.58900 y=732.07898 },{x=96.247299 y=685.11597 }) }
//lineB = {points=[4]({x=51.442501 y=774.73901 },{x=46.796101 y=729.59601 },{x=67.041199 y=693.41504 },{x=136.40599 y=706.02899 }) }//�����������ཻ.
TEST(UGeometryTest,Bezier_Intersection_Works1)
{
	CubicBezierLine a(29.869799,749.17999,82.259300,769.42804,126.58900,732.07898,96.247299,685.11597);
	CubicBezierLine b(51.442501,774.73901,46.796101,729.59601,67.041199,693.41504,136.40599,706.02899);
	std::vector<Point> result = IntersectBezierAndBezierLine(a,b);
	EXPECT_EQ(2,result.size());
	float x1 = a.getX(0.895);
	float y1 = a.getY(0.895);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//tightBoundingRect����.
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

//ֱ�ߵ�Bounding Box.
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



//�����������ཻ,���ܲ���.
TEST(UGeometryPerfTest,Bezier_Intersection_Perf)
{
	CubicBezierLine a(10,100,90,30,40,140,220,240);
	CubicBezierLine b(5,150,180,20,80,280,210,190);
	std::vector<Point> result = IntersectBezierAndBezierLine(a,b);
// 	for(int i = 0; i < 100; i++)
// 	{
// 		result = IntersectBezierAndBezierLine(a,b);
// 	}
	EXPECT_EQ(3,result.size());
	float x1 = a.getX(0.895);
	float y1 = a.getY(0.895);
	if(HasNonfatalFailure())
	{
		FAIL();
	}
}

//ֱ�ߺ�ֱ���ཻ,���ܲ���.
TEST(UGeometryPerfTest,LineLineIntersection_Perf)
{
	StraightLine lineA(15,10,54,29);
	StraightLine lineB(41,4,32,32);

	auto result = Intersect(lineA,lineB);
// 	for(int i = 0; i < 6000000; i++)
// 	{
// 		result = Intersect(lineA,lineB);
// 		if(result.size() >= 5900000)
// 		{
// 			break;
// 		}
// 	}

	//result(36.0,20.1)
// 	ASSERT_EQ(1,result.size());
// 	ASSERT_FLOAT_EQ(35.812351,result[0].x);
// 	ASSERT_FLOAT_EQ(20.139351,result[0].y);
}

//�ָ��������.
TEST(UGeometryTest,Bezier_split_test1)
{
	CubicBezierLine cubic(442.81189,599.61261,533.04486,576.13934,
		623.64673,636.77960,636.44458,729.12103);
	std::vector<CubicBezierLine> subCurve = cubic.split(1.0359697e-007,0.99999994);
	ASSERT_EQ(3,subCurve.size());
}

//>>�������Ƚ�.

//+0 == -0
TEST(UFloatTests,ZeroEqualsMinusZero)
{
	ASSERT_TRUE(AlmostEqualAbs(0.0,-0.0,1));
}