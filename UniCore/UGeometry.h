/*!	\file UGeometry.h
    \brief 包含了线,曲线,几何图形等的处理逻辑.
	
	\author uni
	\date 2014-10-29
*/

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#include <vector>
#include <cassert>
#include <algorithm>
#include <cfloat>
#include <utility>
#include <limits>
#include <xutility>
#include "UMiniLog.h"

namespace uni
{

//! 判断浮点数a和浮点数b是否相等,maxUlpsDiff为最多允许的ulp差值(units in the last place).
inline bool AlmostEqualUlps(float a, float b, int maxUlpsDiff)
{
	int iA = *(int *)&a;
	int iB = *(int *)&b;

	// Different signs means they do not match.
	//\todo 这里的比较比较冒险,可能会是未定义行为.
	if ((((unsigned int)iA)>>31) != (((unsigned int)iB)>>31))
	{
		// Check for equality to make sure +0==-0
		if (a == b)
			return true;
		return false;
	}

	// Find the difference in ULPs.
	int ulpsDiff = abs(iA-iB);
	if (ulpsDiff <= maxUlpsDiff)
		return true;

	return false;
}

//! 判断浮点数a和浮点数b是否相等,maxDiff为允许的绝对差异,maxUlpsDiff为允许的ulp差值(units in the last place).
inline bool AlmostEqualUlpsAndAbs(float a, float b,
	float maxDiff, int maxUlpsDiff)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	float absDiff = fabs(a - b);
	if (absDiff <= maxDiff)
		return true;

	int &iA = *(int *)&a;
	int &iB = *(int *)&b;

	// Different signs means they do not match.
	if ((iA>>31) != (iB>>31))
	{
		// Check for equality to make sure +0==-0
		if (a == b)
			return true;
		return false;
	}

	// Find the difference in ULPs.
	int ulpsDiff = abs(iA-iB);
	if (ulpsDiff <= maxUlpsDiff)
		return true;

	return false;
}

//! 判断浮点数a和浮点数b是否相等,maxDiff为允许的绝对差异.
inline bool AlmostEqualAbs(float a, float b,float maxDiff)
{
	float absDiff = fabs(a - b);
	if (absDiff <= maxDiff)
		return true;
	return false;
}

//! 计算val的符号.若为正则返回1,为负则返回-1.
template <typename T> inline int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

//! 点.
class Point
{
public:
	Point()
		:x(0),y(0)
	{
	}
	Point(float x1, float y1)
		:x(x1),y(y1)
	{
	}
public:
	float x;
	float y;
};


//! 矩形,边和x或y轴平行.
class Rect
{
public:
	Rect()
		:l(0),t(0),r(0),b(0)
	{

	}
	Rect(float l1,float t1,float r1,float b1)
		:l(l1),t(t1),r(r1),b(b1)
	{

	}

	//! 点是否在矩形范围内.
	bool contains(Point p)
	{
		if((p.x > l || AlmostEqualUlps(p.x,l,1)) 
			&& (p.x < r || AlmostEqualUlps(p.x,r,1))
			&& (p.y > t || AlmostEqualUlps(p.y,t,1))
			&& (p.y < b || AlmostEqualUlps(p.y,b,1))
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	float l;
	float t;
	float r;
	float b;
};

//! 矩形,和Rect不同的地方是,边可以是倾斜的.
class XRect
{
public:
	XRect(Rect rect)
	{
		points.push_back(Point(rect.l,rect.t));
		points.push_back(Point(rect.r,rect.t));
		points.push_back(Point(rect.r,rect.b));
		points.push_back(Point(rect.l,rect.b));
	}
	std::vector<Point> points;
};

//! 计算点a和点b间的,可以用于比较的距离.
/*!
	不是实际距离,不会开平方根.
*/
float RawDistance(Point a, Point b);

//! 线条
class Line
{
public:
	enum Type
	{
		Straight,  //!< 直线段.
		CubicBezier,  //!< 三次贝塞尔曲线.
	};
	Line() {}
	Line(const Line &line)
	{
		points = line.points;
	}
	//! 获得线条的类型.
	virtual Type type() const = 0;
	//! 获得中点.
	virtual Point midpoint() const = 0;
	//! 获得最近点.
	virtual Point getNearestPoint(Point p) = 0;
	//! 获得包围矩形.
	virtual Rect boundingBox() const = 0;
	std::vector<Point> points;
};

//! 直线段
class StraightLine : public Line
{
public:
	StraightLine()
	{
		points.push_back(Point(0,0));
		points.push_back(Point(0,0));
	}
	StraightLine(float x1, float y1, float x2, float y2)
	{
		points.push_back(Point(x1,y1));
		points.push_back(Point(x2,y2));
	}
	StraightLine(Point a1, Point a2)
	{
		points.push_back(a1);
		points.push_back(a2);
	}
	//! 获得点p在该线段所属直线上的投影点.
	/*!
		投影点可能在线段外.
	*/
	Point getProjectionPoint(Point p)
	{
		Point pA = points[0];
		Point pB = points[1];
		double deltaX = pB.x-pA.x;
		double deltaY = pB.y-pA.y;

		if(AlmostEqualAbs(deltaY,0,fabs(pB.y)*FLT_EPSILON))
		{
			double x1 = p.x;
			double y1 = (pA.y + pB.y)/2.0;
			return Point(x1,y1);
		}

		if(AlmostEqualAbs(deltaX,0,fabs(pB.x)*FLT_EPSILON))
		{
			double x1 = (pA.x+pB.x)/2.0;
			double y1 = p.y;
			return Point(x1,y1);
		}

		double k = deltaY/deltaX;
		double k1 = -1/k;

		assert(k1 != k);

		double x1 = (pA.y-k*pA.x-p.y+k1*p.x)/(k1-k);
		double y1 = (pA.y*k1+pA.x-k*p.y-p.x)/(k1-k);
		
		return Point(x1,y1);
	}

	//! 获得该线段上的,离点p最近的点.
	virtual Point getNearestPoint(Point p)
	{
		Point projectionPoint = getProjectionPoint(p);
		//如果投射点在线段外,则返回距离p最近的线段的端点.
		float minX = points[0].x;
		float maxX = points[0].x;
		float minY = points[0].y;
		float maxY = points[0].y;
		for(int i = 0; i < points.size(); i++)
		{
			if(points[i].x > maxX)
			{
				maxX = points[i].x;
			}
			if(points[i].x < minX)
			{
				minX = points[i].x;
			}
			if(points[i].y > maxY)
			{
				maxY = points[i].y;
			}
			if(points[i].y < minY)
			{
				minY = points[i].y;
			}
		}

		if(projectionPoint.x < minX || projectionPoint.x > maxX
			|| projectionPoint.y < minY || projectionPoint.y > maxY)
		{
			//投射点在线段外,使用线段端点作为最近点.
			float distanceA = RawDistance(projectionPoint,points[0]);
			float distanceB = RawDistance(projectionPoint,points[1]);

			if(distanceA < distanceB)
			{
				return points[0];
			}
			else
			{
				return points[1];
			}
		}

		return projectionPoint;
	}

	//! 中点.
	virtual Point midpoint() const
	{
		return Point((points[0].x+points[1].x)/2.0,(points[0].y+points[1].y)/2.0);
	}

	virtual Type type() const {return Straight;}

	//! 获得直线段的Bounding Box.
	virtual Rect boundingBox() const
	{
		Rect result;
		result.l = points[0].x;
		result.r = points[1].x;
		result.t = points[0].y;
		result.b = points[1].y;
		if(result.l > result.r)
		{
			float temp = result.r;
			result.r = result.l;
			result.l = temp;
		}
		if(result.t > result.b)
		{
			float temp = result.t;
			result.t = result.b;
			result.b = temp;
		}
		return result;
	}

	//假如使用Ax + By + C = 0表示该直线,得到对应的a,b,c值.
	float a()
	{
		return points[0].y - points[1].y;
	}
	float b()
	{
		return points[1].x - points[0].x;
	}
	float c()
	{
		return points[0].x*points[1].y-points[1].x*points[0].y;
	}
};

//二项式系数(Binomial Coefficient)
//用于贝塞尔曲线相关计算.
static float binomials[][5] = 
{
	{1,0,0,0,0},
	{1,1,0,0,0},
	{1,2,1,0,0},
	{1,3,3,1,0},
};

//! 三次贝塞尔曲线
class CubicBezierLine : public Line
{
public:
	CubicBezierLine(float x1, float y1, float x2, float y2, 
		float x3, float y3, float x4, float y4)
		:originBeginT(0.0)
		,originEndT(1.0)
	{
		points.push_back(Point(x1,y1));
		points.push_back(Point(x2,y2));
		points.push_back(Point(x3,y3));
		points.push_back(Point(x4,y4));

	}
	CubicBezierLine(Point a1, Point a2, Point a3, Point a4)
		:originBeginT(0.0)
		,originEndT(1.0)
	{
		points.push_back(a1);
		points.push_back(a2);
		points.push_back(a3);
		points.push_back(a4);
	}
	CubicBezierLine(const std::vector<Point> &points1)
		:originBeginT(0.0)
		,originEndT(1.0)
	{
		points = points1;
	}

	CubicBezierLine(Point points1[4])
		:originBeginT(0.0)
		,originEndT(1.0)
	{
		for(int i = 0; i < 4; i++)
		{
			points.push_back(points1[i]);
		}
	}

	//! 提取出beginT和endT之间的贝塞尔曲线.
	CubicBezierLine sub(float beginT,float endT)
	{
		assert(beginT <= endT);

		std::vector<CubicBezierLine> subCurve = split(beginT,endT);
		assert(subCurve.size() == 3);

		return subCurve[1];
	}

	//! 使用de Casteljau's algorithm分割3次贝塞尔曲线.
	/*!
		
	*/
	std::vector<CubicBezierLine> split(float t)
	{
		std::vector<CubicBezierLine> result;

		Point leftSplit[4] = {};
		Point rightSplit[4] = {};
		Point span[10] = {};
		assert(points.size() == 4);

		span[0] = points[0];
		span[1] = points[1];
		span[2] = points[2];
		span[3] = points[3];

		int l = 0;
		int r = 3;
		int next = 4;
		for(int c = 3; c > 0; c--)
		{
			leftSplit[l++] = span[next - c - 1];
			for(int i = 0; i < c; i++)
			{
				Point p1 = span[next-c-1];
				Point p2 = span[next-c];
				Point p3;  //p3为p1和p2之间的点,t为比例.

				p3.x = p1.x + (p2.x-p1.x)*t;
				p3.y = p1.y + (p2.y-p1.y)*t;

				span[next++] = p3;
			}
			rightSplit[r--] = span[next-c-1];
		}
		leftSplit[l] = span[next-1];
		rightSplit[0] = span[next-1];

		//左边的子曲线.
		CubicBezierLine leftBezier(leftSplit);
		leftBezier.originBeginT = originBeginT;
		leftBezier.originEndT = originBeginT+t*(originEndT-originBeginT);

		assert(leftBezier.originEndT >= leftBezier.originBeginT);

		//右边的子曲线.
		CubicBezierLine rightBezier(rightSplit);
		assert(leftBezier.originEndT <= originEndT);
		rightBezier.originBeginT = leftBezier.originEndT;
		rightBezier.originEndT = originEndT;

		result.push_back(leftBezier);
		result.push_back(rightBezier);

		return result;
	}

	//! 获得当前曲线在分割前,对应原曲线的t的范围.
	/*!
		例如曲线a (t [0,1]),以t=0.3分割为曲线a1 (t [0,0.3]), a2 (t [0.3,1.0]).
		这里a1的originTRange为0.3, a2的originTRange为1.0-0.3 = 0.7 .
	*/
	float originTRange() 
	{
		assert(originEndT >= originBeginT);
		return originEndT-originBeginT;
	}

	//! 被平行线切割,返回位于平行线间的曲线部分.
	/*!
		两条平行线分别为,
		a1*x+b1*y+c1 = 0.
		a2*x+b2*y+c2 = 0.
	*/
	std::vector<CubicBezierLine> clipByParallelLine(double a1,double b1,double c1,double a2,double b2,double c2)
	{
		std::vector<CubicBezierLine> result;

		//使用minL切割. 计算出有效的t区间.
		//这里E[4]代表贝塞尔曲线到直线间距离的explicit bezier的4个点.
		Point EA[4];
		for(int i = 0; i < 4; i++)
		{
			EA[i].x = i/3.0;
			EA[i].y = a1*points[i].x+b1*points[i].y+c1;
		}

		//计算根.
		std::vector<float> rootsA = root(EA[0].y,EA[1].y,EA[2].y,EA[3].y,0);
		std::sort(rootsA.begin(),rootsA.end());
		std::vector<float> derivativesA;
		std::vector<float> vA;
		for(int i = 0; i < 4; i++)
		{
			vA.push_back(EA[i].y);
		}
		//计算根上的一阶导数.
		for(int i = 0; i < rootsA.size(); i++)
		{
			float derivative = getDerivative(1,rootsA[i],vA);
			derivativesA.push_back(derivative);
		}

		if(AlmostEqualUlpsAndAbs(fabs(c1),fabs(c2),fabs((std::max)(c1,c2))*FLT_EPSILON,1))
		{
			//平行线靠得太近,接近于一条直线.
			//随意找一条直线,算出和曲线到这条直线距离为0的点.
			//之后对于每个点,返回t范围为0的子曲线.
			for(int i = 0; i < rootsA.size(); i++)
			{
				result.push_back(sub(rootsA[i],rootsA[i]));
			}
			return result;
		}

		//>> 使用maxL切割. 计算出有效的t区间.
		Point EB[4];
		for(int i = 0; i < 4; i++)
		{
			EB[i].x = i/3.0;
			EB[i].y = a2*points[i].x+b2*points[i].y+c2;
		}

		//计算根.
		std::vector<float> rootsB = root(EB[0].y,EB[1].y,EB[2].y,EB[3].y,0);
		std::sort(rootsB.begin(),rootsB.end());
		std::vector<float> derivativesB;
		std::vector<float> vB;
		for(int i = 0; i < 4; i++)
		{
			vB.push_back(EB[i].y);
		}
		//计算根上的一阶导数.
		for(int i = 0; i < rootsB.size(); i++)
		{
			float derivative = getDerivative(1,rootsB[i],vB);
			derivativesB.push_back(derivative);
		}

		//计算出要保留的t的范围.
		int currentSignA = 0;  //A组当前的符号.
		if(AlmostEqualAbs(EA[0].y,0,FLT_EPSILON))
		{
			currentSignA = 0;
		}
		else if(EA[0].y > 0)
		{
			currentSignA = 1;
		}
		else
		{
			currentSignA = -1;
		}
		int currentSignB = 0;  //B组当前的符号.
		if(AlmostEqualAbs(EB[0].y,0,FLT_EPSILON))
		{
			currentSignB = 0;
		}
		else if(EB[0].y > 0)
		{
			currentSignB = 1;
		}
		else
		{
			currentSignB = -1;
		}

		int indexA = 0;
		int indexB = 0;
		float currentBeginT = 0.0;
		//如果都没有根,则根据当前的符号判断是要截取整条曲线还是要丢弃整条曲线.

		while(true)
		{
			float rootA = 1.0;
			if(indexA < rootsA.size())
			{
				rootA = rootsA[indexA];
			}
			float rootB = 1.0;
			if(indexB < rootsB.size())
			{
				rootB = rootsB[indexB];
			}

			float currentEndT = currentBeginT;

			bool updateSignA = false;
			bool updateSignB = false;
			if(rootA == rootB)
			{
				updateSignA = true;
				updateSignB = true;
				currentEndT = rootA;
			}
			else if(rootA < rootB)
			{
				updateSignA = true;
				currentEndT = rootA;
			}
			else //(rootB < rootA)
			{
				updateSignB = true;
				currentEndT = rootB;
			}

			if(currentSignA == 1 && currentSignB == 1)
			{
				//提取当前分段作为结果.
				CubicBezierLine subCurve = sub(currentBeginT,currentEndT);
				result.push_back(subCurve);
				currentBeginT = currentEndT;
			}
			else if(currentSignA == -1 || currentSignB == -1)
			{
				currentBeginT = currentEndT;
			}
			else
			{
				//有0存在的情形,不改变currentBeginT.
			}

			if(indexA >= rootsA.size() && indexB >= rootsB.size())
			{
				break;
			}

			//更新A组的符号.
			if(updateSignA)
			{
				if(indexA >= rootsA.size())
				{
					if(AlmostEqualAbs(EA[3].y,0.0,FLT_EPSILON))
					{
						//currentSignA不变.
					}
					else if(EA[3].y > 0)
					{
						if(currentSignA == 1)
						{

						}
						else if(currentSignA == -1)
						{
							assert(!"currentSignA should not be -1.");
						}
						else
						{
							currentSignA = 1;
						}
					}
				}
				else
				{
					if(derivativesA[indexA] == 0)
					{
						//currentSignA不变.
					}
					else if(derivativesA[indexA] > 0)
					{
						if(currentSignA == -1)
						{
							currentSignA = 1;
						}
						else if(currentSignA == 1)
						{
							assert(!"currentSignA shold not be 1.");
						}
						else
						{

						}
					}
					else //derivativesA[indexA] < 0
					{
						if(currentSignA == -1)
						{
							assert(!"currentSignA shold not be -1.");
						}
						else if(currentSignA == 1)
						{
							currentSignA = -1;
						}
						else
						{

						}
					}
				}
				
				indexA++;  //A组的这个数据已经使用,处理A组的下一个.
			}

			//更新B组的符号.
			if(updateSignB)
			{
				if(indexB >= rootsB.size())
				{
					if(AlmostEqualAbs(EB[3].y,0.0,FLT_EPSILON))
					{
						//currentSignB不变.
					}
					else if(EB[3].y > 0)
					{
						if(currentSignB == 1)
						{

						}
						else if(currentSignB == -1)
						{
							assert(!"currentSignB should not be -1.");
						}
						else
						{
							currentSignB = 1;
						}
					}
				}
				else
				{
					if(derivativesB[indexB] == 0)
					{
						//currentSignB不变.
					}
					else if(derivativesB[indexB] > 0)
					{
						if(currentSignB == -1)
						{
							currentSignB = 1;
						}
						else if(currentSignB == 1)
						{
							assert(!"currentSignB shold not be 1.");
						}
						else
						{

						}
					}
					else //derivativesB[indexB] < 0
					{
						if(currentSignB == -1)
						{
							assert(!"currentSignB shold not be -1.");
						}
						else if(currentSignB == 1)
						{
							currentSignB = -1;
						}
						else
						{

						}
					}

				}

				indexB++;  //B组的这个数据已经使用,处理B组的下一个.
			}

		}

		return result;
	}

	//! 使用两条平行线切割当前曲线,丢弃掉平行线外的部分.
	/*!
		返回被切割后的曲线.
	*/
	std::vector<CubicBezierLine> clipByParallelLine(StraightLine lineA,StraightLine lineB)
	{
		std::vector<CubicBezierLine> result;

		double a1 = lineA.a();
		double b1 = lineA.b();
		double c1 = lineA.c();
		double a2 = lineB.a();
		double b2 = lineB.b();
		double c2 = lineB.c();

		//>> 以下代码保证了两条平行线间的点到这两条平行线的距离均为正的.

		//计算lineA上的点到lineB的距离.
		Point pA = lineA.points[0];
		double enumerator = (a2*pA.x+b2*pA.y+c2);
		double denominator = sqrt(a2*a2+b2*b2);
		double distance = 0.0;
		if(denominator != 0.0)
		{
			distance = enumerator/denominator;
		}

		if(distance < 0)
		{
			a2 = -a2;
			b2 = -b2;
			c2 = -c2;
		}

		//计算lineB上的点到lineA的距离.
		Point pB = lineB.points[0];
		enumerator = (a1*pB.x+b1*pB.y+c1);
		denominator = sqrt(a1*a1+b1*b1);
		distance = 0.0;
		if(denominator != 0)
		{
			distance = enumerator/denominator;
		}
		if(distance < 0)
		{
			a1 = -a1;
			b1 = -b1;
			c1 = -c1;
		}

		return clipByParallelLine(a1,b1,c1,a2,b2,c2);
	}


	//! 使用clipLine这条曲线的fat line切割line,丢弃fat line之外的部分.
	std::vector<CubicBezierLine> clipByFatLine(CubicBezierLine clipLine)
	{
		//L为过P0和P3的线段.
		StraightLine L(clipLine.points[0],clipLine.points[3]);
		//找到过控制点且和L平行的线.

		double a0 = L.a();
		double b0 = L.b();
		double c0 = L.c();
		Point p0 = L.points[0];

		Point p1 = clipLine.points[1];
		double a1 = a0;
		double b1 = b0;
		double c1 = -a0*p1.x-b0*p1.y;

		Point p2 = clipLine.points[2];
		double a2 = a0;
		double b2 = b0;
		double c2 = -a0*p2.x-b0*p2.y;

		//找到最大和最小的c.
		if(c1 < c2)
		{
			if(c0 < c1)
			{
				c1 = c0;
				p1 = p0;
			}
			
			if(c0 > c2)
			{
				c2 = c0;
				p2 = p0;
			}
		}
		else if(c1 > c2)
		{
			if(c0 < c2)
			{
				c2 = c0;
				p2 = p0;
			}

			if(c0 > c1)
			{
				c1 = c0;
				p1 = p0;
			}
		}
		else//c1 == c2
		{
			if(c0 < c1)
			{
				c1 = c0;
				p1 = p0;
			}

			if(c0 > c2)
			{
				c2 = c0;
				p2 = p0;
			}
		}

		//>> 以下代码保证了两条平行线间的点到这两条平行线的距离均为正的.
 		double numerator = (a1*p2.x+b1*p2.y+c1);
 		double denominator = sqrt(a1*a1+b1*b1);
		double distance = 0.0;
		if(denominator != 0.0)
		{
			distance = numerator/denominator;
		}

		if(distance < 0.0)
		{
			a1 = -a1;
			b1 = -b1;
			c1 = -c1;

		}

		distance = 0.0;
		numerator = (a2*p1.x+b2*p1.y+c2);
		denominator = sqrt(a2*a2+b2*b2);
		if(denominator != 0)
		{
			distance = numerator/denominator;
		}

		if(distance < 0.0)
		{
			a2 = -a2;
			b2 = -b2;
			c2 = -c2;
		}

		return clipByParallelLine(a1,b1,c1,a2,b2,c2);
	}

	//分割贝塞尔曲线.
	std::vector<CubicBezierLine> split(float t1,float t2)
	{
		assert(t1 <= t2);
		assert(t1 >= 0.0);
		assert(t1 <= 1.0);
		assert(t2 >= 0.0);
		assert(t2 <= 1.0);

		std::vector<CubicBezierLine> result;
		std::vector<CubicBezierLine> subCurve1 = split(t1);
		assert(subCurve1.size() == 2);
		result.push_back(subCurve1[0]);

		if(t2 != 1.0)
		{
			assert(t1 != 1.0);
			t2 = (t2-t1)/(1.0-t1);
		}

		std::vector<CubicBezierLine> subCurve2 = 
			subCurve1[1].split(t2);
		assert(subCurve2.size() == 2);
		result.push_back(subCurve2[0]);
		result.push_back(subCurve2[1]);
		return result;
	}

	//! 计算inflection(拐点).
	/*!
		用于计算贝塞尔曲线的包围矩形(tight bounding box).
	*/
	std::vector<float> getInflections() const
	{
		std::vector<float> result;

		result.reserve(20);

		result.push_back(0);
		result.push_back(1);

		//求1阶和2阶导数的根.
		float p0 = points[0].x;
		float p1 = points[1].x;
		float p2 = points[2].x;
		float p3 = points[3].x;
		
		std::vector<float> &&roots = root(p0,p1,p2,p3,1);
		result.insert(result.end(),roots.begin(),roots.end());

		roots = root(p0,p1,p2,p3,2);
		result.insert(result.end(),roots.begin(),roots.end());

		p0 = points[0].y;
		p1 = points[1].y;
		p2 = points[2].y;
		p3 = points[3].y;

		roots = root(p0,p1,p2,p3,1);
		result.insert(result.end(),roots.begin(),roots.end());

		roots = root(p0,p1,p2,p3,2);
		result.insert(result.end(),roots.begin(),roots.end());
		
		return result;
	}

	//! 计算贝塞尔曲线的紧包围矩形.
	/*!
		得到的并不是包围该贝塞尔曲线的最小包围矩形,但是相比boundingBox函数
		得到的矩形要更小.
	*/
	virtual Rect tightBoundingBox() const
	{
		std::vector<float> inflections = getInflections();

		if(inflections.empty())
		{
			assert(!"inflections should not be empty!");
			return Rect(0,0,0,0);
		}

		float minX = points[0].x;
		float minY = points[0].y;
		float maxX = points[0].x;
		float maxY = points[0].y;

		for(int i=0; i<inflections.size(); i++) {
			float t = inflections[i];
			float x = getX(t);
			float y = getY(t);

			if(x < minX) { minX = x; }
			if(x > maxX) { maxX = x; }
			if(y < minY) { minY = y; }
			if(y > maxY) { maxY = y; }
		}
		
		return Rect(minX,minY,maxX,maxY);
	}

	//! 得到包围矩形.
	/*!
		为包围4个控制点的矩形.
	*/
	virtual Rect boundingBox() const
	{
		float minX = points[0].x;
		float minY = points[0].y;
		float maxX = points[0].x;
		float maxY = points[0].y;
		for(int i=0; i< points.size(); i++) {
			float x = points[i].x;
			float y = points[i].y;

			if(x < minX) { minX = x; }
			if(x > maxX) { maxX = x; }
			if(y < minY) { minY = y; }
			if(y > maxY) { maxY = y; }
		}

		return Rect(minX,minY,maxX,maxY);
	}

	//! 获得非常紧的包围矩形.
	XRect veryTightBoundingBox()
	{
		//将曲线旋转成和x轴平行,计算bounding box,之后再旋转回原角度.
		//这样计算出的bounding box比未旋转直接计算要来得更小.
		const double PI = 3.14159265358979323846264338327950288419716939937510582097494459;
		float ox = points[0].x;
		float oy = points[0].y;
		float angle = atan2(points[3].y - points[0].y, points[3].x - points[0].x);
		float ca = cos(angle);
		float sa = sin(angle);

		Rect boundingBox = align(points[0],points[3]).tightBoundingBox();
		XRect xBoundingBox(boundingBox);

		for(int i = 0; i < xBoundingBox.points.size(); i++)
		{
			Point &p = xBoundingBox.points[i];
			float nx = (p.x * ca - p.y * sa) + ox;
			float ny = (p.x * sa + p.y * ca) + oy;
			p.x = nx;
			p.y = ny;
		}

		return xBoundingBox;
	}

	//! 获得曲线上离点p最近的点.
	/*!
		采用数值解法计算当前点到贝塞尔曲线上的最近点.
	*/
	virtual Point getNearestPoint(Point p)
	{
		float minT = 0;
		float dist = 0;
		float minDist = RawDistance(p,Point(getX(0),getY(0)));
		const int lutResolusion = 300;

		for(int i = 0; i < lutResolusion; i++)
		{
			float t = (float)i/(float)lutResolusion;
			dist = RawDistance(p,Point(getX(t),getY(t)));
			if(dist < minDist)
			{
				minDist = dist;
				minT = t;
			}
		}

		minT = refineNearest(p,minT,minDist,1.0/1.01*lutResolusion);

		return Point(getX(minT),getY(minT));
	}

	//! 获得中点.
	/*!
		得到的是t=0.5处的点,并非距离为一半的点.
	*/
	virtual Point midpoint() const
	{
		return Point(getX(0.5),getY(0.5));
	}

	//! 用于查找最近点.
	float refineNearest(Point p,float t,float dist,float precision)
	{
		if(precision < 0.0001)
		{
			return t;
		}
		float prevT = t-precision;
		float nextT = t+precision;
		Point prevP(getX(prevT),getY(prevT));
		Point nextP(getX(nextT),getY(nextT));
		float prevDist = RawDistance(p,prevP);
		float nextDist = RawDistance(p,nextP);

		if(prevT >= 0 && prevDist < dist)
		{
			return refineNearest(p,prevT,prevDist,precision);
		}

		if(nextT <= 1 && nextDist < dist)
		{
			return refineNearest(p,nextT,nextDist,precision);
		}

		return refineNearest(p,t,dist,precision/2.0);
	}

	//! 将该曲线绕start,end连成的线段旋转,使线段和x轴平行.
	CubicBezierLine align(Point start, Point end)
	{
		//! 旋转角度为-angle.
		float angle = atan2(end.y-start.y,end.x-start.x);
		float ca = cos(-angle);
		float sa = sin(-angle);
		float ox = start.x;
		float oy = start.y;
		
		std::vector<Point> alignedPoints;

		for(int i = 0; i < 4; i++)
		{
			float x = points[i].x;
			float y = points[i].y;
			float alignedX = 
				ca * (x-ox) - sa * (y-oy);
			float alignedY = 
				sa * (x-ox) + ca * (y-oy);

			alignedPoints.push_back(Point(alignedX,alignedY));
		}

		return CubicBezierLine(alignedPoints);
	}

	//! 求根.
	/*!
		\param derivative 0为求根,1为求1阶导数的根,2为求2阶导数的根...
	*/
	std::vector<float> root(float p0,float p1,float p2,float p3,int derivative) const
	{
		std::vector<float> roots;
		std::vector<float> result;
		if(derivative == 0)
		{

			float a = -p0 + 3*p1 + -3*p2 + p3;
			float b = 3*p0 - 6*p1 + 3*p2;
			float c = -3*p0 + 3*p1;
			float d = p0;

			roots = cubicRoot(a,b,c,d);
		}
		else if(derivative == 1)
		{
			//3⋅(B−A), 3⋅(C−B), 3⋅(D−C)
			float pp0 = 3*(p1-p0);
			float pp1 = 3*(p2-p1);
			float pp2 = 3*(p3-p2);
			float a = pp0-2*pp1+pp2;
			float b = 2*pp1-2*pp0;
			float c = pp0;
			roots = quadraticRoot(a,b,c);
		}
		else if(derivative == 2)
		{
			float pp0 = 3*(p1-p0);
			float pp1 = 3*(p2-p1);
			float pp2 = 3*(p3-p2);
			//{2⋅(B′−A′), 2⋅(C′−B′)}
			float ppp0 = 2*(pp1-pp0);
			float ppp1 = 2*(pp2-pp1);

			float a = ppp1-ppp0;
			float b = ppp0;

			roots = linearRoot(a,b);
		}
		else
		{
			assert(false);
		}

		for(int i = 0; i < roots.size(); i++)
		{
			if(roots[i] >= 0 && roots[i] <= 1.0)
			{
				result.push_back(roots[i]);
			}
		}
		return result;
	}

	//! 求一元一次方程.
	std::vector<float> linearRoot(float a,float b) const
	{
		std::vector<float> result;
		if(AlmostEqualAbs(a,0,FLT_EPSILON))
		{
			return result;
		}
		result.push_back(-b/a);
		return result;
	}

	//! 求一元二次方程.
	std::vector<float> quadraticRoot(float a,float b,float c) const
	{
		std::vector<float> result;
		if(AlmostEqualAbs(a,0,FLT_EPSILON))
		{
			return linearRoot(b,c);
		}

		float delta = b*b-4*a*c;
		if(delta < 0)
		{
			return result;
		}
		float rootA = (-b+sqrt(delta))/(2*a);
		float rootB = (-b-sqrt(delta))/(2*a);
		result.push_back(rootA);
		result.push_back(rootB);
		return result;
	}

	//! 求一元三次方程.参考wikipedia中求根公式法和三角函数解法.
	std::vector<float> cubicRoot(float a,float b,float c,float d) const
	{
		const double PI = 3.14159265358979323846264338327950288419716939937510582097494459;
		if(AlmostEqualAbs(a,0,FLT_EPSILON))
		{
			return quadraticRoot(b,c,d);
		}

		double A = b/a;
		double B = c/a;
		double C = d/a;

		double Q, R, D, S, T, Im;

		Q = (3.0*B - pow(A, 2))/9.0;
		R = (9.0*A*B - 27.0*C - 2*pow(A, 3))/54.0;
		D = pow(Q, 3) + pow(R, 2);    // polynomial discriminant

		double t[3] = {};

		if (D >= 0)                                 // complex or duplicate roots
		{
			//求根公式法.
			S = sgn(R + sqrt(D))*pow(abs(R + sqrt(D)),(1.0/3.0));
			T = sgn(R - sqrt(D))*pow(abs(R - sqrt(D)),(1.0/3.0));

			t[0] = -A/3.0 + (S + T);                    // real root
			t[1] = -A/3.0 - (S + T)/2.0;                  // real part of complex root
			t[2] = -A/3.0 - (S + T)/2.0;                  // real part of complex root
			Im = abs(sqrt(3.0)*(S - T)/2.0);    // complex part of root pair   

			/*discard complex roots*/
			//虚部不为0,则为复根.
			if (Im!=0)
			{
				t[1]=-1;
				t[2]=-1;
			}

		}
		else                                          // distinct real roots
		{
			//三角函数法.
			//R > 0, D < 0, 因此 Q < 0.
			//以下sqrt(-Q)及sqrt(-pow(Q, 3))不会返回nan.
			//因为R != 0 && R^2 + Q^3 < 0.
			//因此abs(Q^3) > abs(R^2).
			//因此R^2/Q^3在[-1,1]之间.
			//因此R/Q^(3/2)在[-1,1]之间.
			//下面acos中的R/sqrt(-pow(Q, 3))必定会在[-1,1]之间.
			double th = acos(R/sqrt(-pow(Q, 3)));

			t[0] = 2.0*sqrt(-Q)*cos(th/3.0) - A/3.0;
			t[1] = 2.0*sqrt(-Q)*cos((th + 2*PI)/3.0) - A/3.0;
			t[2] = 2.0*sqrt(-Q)*cos((th + 4*PI)/3.0) - A/3.0;
			Im = 0.0;
		}

		/*discard out of spec roots*/
		std::vector<float> result;
		for (int i=0;i<3;i++) 
		{
			result.push_back(t[i]);
		}

		return result;
	}

	//! 求导数.
	/*!
		\param derivative 几阶导数.
		\param v 传入points中的所有x组成的数组,或者所有y组成的数组.
	*/
	float getDerivative(int derivative, float t, const std::vector<float> v)
	{
		int n = v.size()-1;
		if(n == 0)
		{
			return 0;
		}

		//零阶导数,就是自身.
		if(derivative == 0)
		{
			float value = 0;
			for(int k = 0; k <= n; k++)
			{
				value += 
					binomials[n][k] * pow(1-t,n-k) * pow(t,k) *v[k];
			}
			return value;
		}
		else
		{
			std::vector<float> newV;
			
			for(int k = 0; k < n; k++)
			{
				newV.push_back(n*(v[k+1]-v[k]));
			}
			return getDerivative(derivative - 1, t, newV);
		}
	}

	//! 获得t位置处的x值.
	float getX(float t) const
	{
		const int n = 3;
		float result = 0;
		for(int k = 0; k <= n; k++)
		{
			if(points[k].x == 0)
			{
				continue;
			}
			float polyterm = pow((1-t),n-k) * pow(t,k);
			result += binomials[n][k] * polyterm * points[k].x;
		}

		return result;
	}

	//! 获得t位置处的y值.
	float getY(float t) const
	{
		const int n = 3;
		float result = 0;
		for(int k = 0; k <= n; k++)
		{
			if(points[k].y == 0)
			{
				continue;
			}
			float polyterm = pow((1-t),n-k) * pow(t,k);
			result += binomials[n][k] * polyterm * points[k].y;
		}

		return result;
	}
	virtual Type type() const {return CubicBezier;}

	//originBeginT和originEndT记录了该条曲线的t的范围.
	//假如不是0,1,则说明该曲线是被分割出来的一条子曲线.
	float originBeginT;
	float originEndT;
};

//! 计算直线和贝塞尔曲线的交点.
inline std::vector<Point> IntersectStraightAndBezierLine(const Line &straight, const Line &bezier)
{
	std::vector<Point> result;

	CubicBezierLine origin = ((CubicBezierLine &)bezier);

	Rect bezierBoundingBox = origin.boundingBox();
	Rect straightBoundingBox = straight.boundingBox();

	if(bezierBoundingBox.l > straightBoundingBox.r
		|| bezierBoundingBox.r < straightBoundingBox.l
		|| bezierBoundingBox.t > straightBoundingBox.b
		|| bezierBoundingBox.b < straightBoundingBox.t)
	{
		return result;
	}

	CubicBezierLine aligned = 
		origin.align(straight.points[0],straight.points[1]);

	float p0 = aligned.points[0].y;
	float p1 = aligned.points[1].y;
	float p2 = aligned.points[2].y;
	float p3 = aligned.points[3].y;

	std::vector<float> roots = aligned.root(p0,p1,p2,p3,0);

	for(int i = 0; i < roots.size(); i++)
	{
		Point p(origin.getX(roots[i]),origin.getY(roots[i]));
		if(straightBoundingBox.contains(p))
		{
			result.push_back(p);
		}
	}

	return result;
}

//计算直线和直线的交点.
inline std::vector<Point> IntersectStraightLine(const Line &lineA, const Line &lineB)
{
	std::vector<Point> result;

	float lineA_x1 = lineA.points[0].x;
	float lineA_y1 = lineA.points[0].y;
	float lineA_x2 = lineA.points[1].x;
	float lineA_y2 = lineA.points[1].y;
	float lineB_x1 = lineB.points[0].x;
	float lineB_y1 = lineB.points[0].y;
	float lineB_x2 = lineB.points[1].x;
	float lineB_y2 = lineB.points[1].y;

	Rect lineABoundingBox = lineA.boundingBox();
	Rect lineBBoundingBox = lineB.boundingBox();

	if(lineABoundingBox.l > lineBBoundingBox.r
		|| lineABoundingBox.r < lineBBoundingBox.l
		|| lineABoundingBox.t > lineBBoundingBox.b
		|| lineABoundingBox.b < lineBBoundingBox.t)
	{
		return result;
	}

	float A1 = lineA_y2 - lineA_y1;
	float B1 = lineA_x1 - lineA_x2;
	float C1 = A1*lineA_x1+B1*lineA_y1;

	float A2 = lineB_y2 - lineB_y1;
	float B2 = lineB_x1 - lineB_x2;
	float C2 = A2*lineB_x1+B2*lineB_y1;

	float det = A1*B2 - A2*B1;
	if(AlmostEqualUlps(A1*B2,A2*B1,1))
	{
		//两直线平行.
	}
	else
	{
		assert(det != 0.0);
		float x = (B2*C1 - B1*C2)/det;
		float y = (A1*C2 - A2*C1)/det;
		Point p(x,y);

		//判断交点是否在两条线段上.
		if(lineABoundingBox.contains(p)
			&& lineBBoundingBox.contains(p))
		{
			result.push_back(p);
		}
	}

	return result;
}


//! 用于相交算法的测试.
class IntersectionTestPainter
{
public:
	virtual void newFrame() {};
	virtual void addBezier(CubicBezierLine line) {};
	virtual void addLine(StraightLine line) {};
	virtual void addPoint(Point p) {};
	virtual void waitNextFrame() {};
};

//! 计算贝塞尔曲线和贝塞尔曲线的交点.
inline std::vector<Point> IntersectBezierAndBezierLine(
	const Line &lineA, const Line &lineB,IntersectionTestPainter *painter = new IntersectionTestPainter)
{
	std::vector<Point> result;

	CubicBezierLine &lA = (CubicBezierLine &)lineA;
	CubicBezierLine &lB = (CubicBezierLine &)lineB;

	std::vector<CubicBezierLine> groupA;
	std::vector<CubicBezierLine> groupB;
	groupA.reserve(5);
	groupB.reserve(5);
	groupA.push_back(lA);
	groupB.push_back(lB);
	int counter = 0;
	while(!groupA.empty() && !groupB.empty())
	{
		CubicBezierLine a = groupA.back();
		groupA.pop_back();
		CubicBezierLine b = groupB.back();
		groupB.pop_back();

		painter->newFrame();
		painter->addBezier(a);
		painter->addBezier(b);
		painter->waitNextFrame();

		//假如a,b都足够小,则认为找到了交点.

		if(a.originTRange() < 0.001 && b.originTRange() < 0.001)
		{
			float t = (a.originEndT+a.originBeginT)/2.0;
			result.push_back(Point(a.getX(t),a.getY(t)));

			painter->newFrame();
			painter->addPoint(Point(a.getX(t),a.getY(t)));
			painter->waitNextFrame();
		}
		else
		{
			counter++;
			if(counter%2 == 0)
			{
				XRect bb = b.veryTightBoundingBox();

				std::vector<CubicBezierLine> sub = a.clipByFatLine(b);

 				painter->newFrame();
 				for(int i = 0; i < sub.size(); i++)
 				{
 					painter->addBezier(sub[i]);
 				}
 				painter->addBezier(b);
 				painter->waitNextFrame();

				//假如切割得不够大,继续用boundingRect切割.
				if(sub.size() == 1
					&& sub[0].originTRange()/a.originTRange() > 0.80)
				{
					//继续切割.
					a = sub[0];
					StraightLine clip1(bb.points[0],bb.points[1]);
					StraightLine clip2(bb.points[3],bb.points[2]);

					painter->newFrame();
					painter->addBezier(a);
					painter->addBezier(b);
					painter->addLine(clip1);
					painter->addLine(clip2);
					painter->waitNextFrame();

					sub = a.clipByParallelLine(clip1,clip2);

				}

				painter->newFrame();
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->addBezier(b);
				painter->waitNextFrame();

				if(sub.size() == 1 
					&& sub[0].originTRange()/a.originTRange() > 0.80)
				{
					a = sub[0];
					StraightLine clip1(bb.points[1],bb.points[2]);
					StraightLine clip2(bb.points[0],bb.points[3]);

					painter->newFrame();
					painter->addBezier(a);
					painter->addBezier(b);
					painter->addLine(clip1);
					painter->addLine(clip2);
					painter->waitNextFrame();

					sub = a.clipByParallelLine(clip1,clip2);
				}

				painter->newFrame();
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->addBezier(b);
				painter->waitNextFrame();

				if(sub.size() == 1
					&& a.originTRange() > 0.05
					&& sub[0].originTRange()/a.originTRange() > 0.50)
				{
					a = sub[0];
					sub = a.split(0.5);
				}

				painter->newFrame();
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->addBezier(b);
				painter->waitNextFrame();

				for(int i = 0; i < sub.size(); i++)
				{
					groupA.push_back(sub[i]);
					groupB.push_back(b);
				}
			}
			else
			{
				XRect bb = a.veryTightBoundingBox();

				std::vector<CubicBezierLine> sub = b.clipByFatLine(a);

				painter->newFrame();
				painter->addBezier(a);
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->waitNextFrame();

				if(sub.size() == 1
					&& sub[0].originTRange()/b.originTRange() > 0.80)
				{
					//继续切割.
					b = sub[0];
					StraightLine clip1(bb.points[0],bb.points[1]);
					StraightLine clip2(bb.points[3],bb.points[2]);

					painter->newFrame();
					painter->addBezier(a);
					painter->addBezier(b);
					painter->addLine(clip1);
					painter->addLine(clip2);
					painter->waitNextFrame();

					sub = b.clipByParallelLine(clip1,clip2);

				}

				painter->newFrame();
				painter->addBezier(a);
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->waitNextFrame();

				if(sub.size() == 1
					&& sub[0].originTRange()/b.originTRange() > 0.80)
				{
					b = sub[0];
					StraightLine clip1(bb.points[1],bb.points[2]);
					StraightLine clip2(bb.points[0],bb.points[3]);

					painter->newFrame();
					painter->addBezier(a);
					painter->addBezier(b);
					painter->addLine(clip1);
					painter->addLine(clip2);
					painter->waitNextFrame();

					sub = b.clipByParallelLine(clip1,clip2);
				}

				painter->newFrame();
				painter->addBezier(a);
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->waitNextFrame();

				if(sub.size() == 1
					&& b.originTRange() > 0.05
					&& sub[0].originTRange()/b.originTRange() > 0.50)
				{
					b = sub[0];
					sub = b.split(0.5);
				}

				painter->newFrame();
				painter->addBezier(a);
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->waitNextFrame();

				for(int i = 0; i < sub.size(); i++)
				{
					groupA.push_back(a);
					groupB.push_back(sub[i]);
				}
			}

			if(counter > 100)
			{
				//assert(!"Intersect Bezier does not converge.");
				break;
			}
		}
	}
	return result;
}

//! 计算交点.
inline std::vector<Point> Intersect(const Line &lineA, const Line &lineB)
{
	std::vector<Point> result;
	if(lineA.type() == Line::Straight
		&& lineB.type() == Line::Straight)
	{
		result = IntersectStraightLine(lineA,lineB);
	}
	else if(lineA.type() == Line::Straight && lineB.type() == Line::CubicBezier)
	{
		result = IntersectStraightAndBezierLine(lineA,lineB);
	}
	else if(lineA.type() == Line::CubicBezier && lineB.type() == Line::Straight)
	{
		result = IntersectStraightAndBezierLine(lineB,lineA);
	}
	else if(lineA.type() == Line::CubicBezier && lineB.type() == Line::CubicBezier)
	{
		result = IntersectBezierAndBezierLine(lineA,lineB);
	}
	else
	{
		assert(!"Don't know how to calculate the intersection point.");
	}
	return result;
}


}//namespace uni