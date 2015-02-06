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

namespace uni
{

bool FloatEqual(float x,float y);

template <typename T> inline int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

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

//! 求两点间的插值.
/*!
*/
inline Point Lerp(Point a,Point b,float t)
{
	float x = a.x + (b.x-a.x)*t;
	float y = a.y + (b.y-a.y)*t;

	return Point(x,y);
}



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
	bool contains(Point p)
	{
		if(p.x < l)
		{
			return false;
		}
		if(p.x > r)
		{
			return false;
		}
		if(p.y < t)
		{
			return false;
		}
		if(p.y > b)
		{
			return false;
		}
		return true;
	}
	float l;
	float t;
	float r;
	float b;
};

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

//! 计算点a和点b间的,用于比较的距离.
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
		Straight,
		CubicBezier,
	};
	Line() {}
	virtual Type type() const = 0;
	virtual int pointCount() const = 0;
	virtual Point point(int index) const = 0;
	virtual Point midpoint() const = 0;
	virtual Point getNearestPoint(Point p) = 0;
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

		if(FloatEqual(deltaY,0))
		{
			double x1 = p.x;
			double y1 = (pA.y + pB.y)/2.0;
			return Point(x1,y1);
		}

		if(FloatEqual(deltaX,0))
		{
			double x1 = (pA.x+pB.x)/2.0;
			double y1 = p.y;
			return Point(x1,y1);
		}

		double k = deltaY/deltaX;
		double k1 = -1/k;

		double x1 = (pA.y-k*pA.x-p.y+k1*p.x)/(k1-k);
		double y1 = (pA.y*k1+pA.x-k*p.y-p.x)/(k1-k);
		
		return Point(x1,y1);
	}

	//! 获得该线段上的,离点p最近的点.
	virtual Point getNearestPoint(Point p)
	{
		Point projectionPoint = getProjectionPoint(p);
		//如果projectionPoint在线段外,则返回线段的端点.

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

	virtual Point midpoint() const
	{
		return Point((points[0].x+points[1].x)/2.0,(points[0].y+points[1].y)/2.0);
	}

	virtual Type type() const {return Straight;}
	virtual int pointCount() const {return points.size();}
	virtual Point point(int index) const
	{
		assert(index >= 0);
		assert(index < points.size());
		return points[index];
	}

	virtual Rect boundingBox() const
	{
		Rect result;
		result.l = points[0].x;
		result.r = points[1].x;
		result.t = points[0].y;
		result.b = points[1].y;
		if(result.l > result.r)
		{
			std::swap(result.l,result.r);
		}
		if(result.t > result.b)
		{
			std::swap(result.t,result.b);
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
		:originBeginT(0)
		,originEndT(1)
	{
		points.push_back(Point(x1,y1));
		points.push_back(Point(x2,y2));
		points.push_back(Point(x3,y3));
		points.push_back(Point(x4,y4));

	}
	CubicBezierLine(Point a1, Point a2, Point a3, Point a4)
		:originBeginT(0)
		,originEndT(1)
	{
		points.push_back(a1);
		points.push_back(a2);
		points.push_back(a3);
		points.push_back(a4);
	}
	CubicBezierLine(const std::vector<Point> &points1)
		:originBeginT(0)
		,originEndT(1)
	{
		points = points1;
	}

	CubicBezierLine(Point points1[4])
		:originBeginT(0)
		,originEndT(1)
	{
		points.assign(&points1[0],&points1[4]);
	}

	//! 使用de Casteljau's algorithm分割3次贝塞尔曲线.
	/*!
		t太小会分割失败.
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
				Point p3 = Lerp(p1,p2,t);
				span[next++] = p3;
			}
			rightSplit[r--] = span[next-c-1];
		}
		leftSplit[l] = span[next-1];
		rightSplit[0] = span[next-1];

		CubicBezierLine leftBezier(leftSplit);
		leftBezier.originBeginT = originBeginT;
		leftBezier.originEndT = originBeginT+t*(originEndT-originBeginT);

		assert(leftBezier.originEndT >= leftBezier.originBeginT);

		CubicBezierLine rightBezier(rightSplit);
		assert(leftBezier.originEndT <= originEndT);
		rightBezier.originBeginT = leftBezier.originEndT;
		rightBezier.originEndT = originEndT;

		result.push_back(leftBezier);
		result.push_back(rightBezier);

		return result;
	}

	//! 获得originT的范围.
	float originTRange() 
	{
		assert(originEndT >= originBeginT);
		return originEndT-originBeginT;
	}

	std::vector<CubicBezierLine> clipByParallelLine(float a1,float b1,float c1,float a2,float b2,float c2)
	{
		std::vector<CubicBezierLine> result;
// 		assert(FloatEqual(a1,a2));
// 		assert(FloatEqual(b1,b2));

		float minL[3] = {};
		float maxL[3] = {};

		if(c1 > c2)
		{
			minL[0] = -a2;
			minL[1] = -b2;
			minL[2] = -c2;			
			maxL[0] = a1;
			maxL[1] = b1;
			maxL[2] = c1;
		}
		else
		{
			minL[0] = -a1;
			minL[1] = -b1;
			minL[2] = -c1;
			maxL[0] = a2;
			maxL[1] = b2;
			maxL[2] = c2;
		}



		Point E[4];
		float minT = 1.1;
		float maxT = -0.1;
		StraightLine LE[3];
		float vx[3] = {};

		//使用minL切割.
		bool inMinLine = false;
		for(int i = 0; i < 4; i++)
		{
			E[i].x = i/3.0;
			E[i].y = minL[0]*points[i].x+minL[1]*points[i].y+minL[2];
			if(E[i].y > 0.0)
			{
				inMinLine = true;
			}
		}

		if(!inMinLine)
		{
			return result;
		}

		if(E[0].y < 0.0)
		{
			//得到E0到其他端点连成的线,例如E0-E1,E0-E2,E0-E3...
			for(int i = 0; i < 3; i++)
			{
				LE[i].points[0] = E[0];
				LE[i].points[1] = E[i];
				vx[i] = -LE[i].c()/LE[i].a();
				if(vx[i] > 0.0 && vx[i] < 1.0 && minT > vx[i])
				{
					minT = vx[i];
				}
			}
		}

		if(E[3].y < 0.0)
		{
			//得到E3到其他端点连成的线,例如E3-E2,E3-E1,E3-E0...
			for(int i = 2; i >= 0; i--)
			{
				LE[i].points[0] = E[3];
				LE[i].points[1] = E[i];
				vx[i] = -LE[i].c()/LE[i].a();
				if(vx[i] > 0.0 && vx[i] < 1.0 && maxT < vx[i])
				{
					maxT = vx[i];
				}
			}
		}

		//>> 使用maxL切割.
		bool inMaxLine = false;
		for(int i = 0; i < 4; i++)
		{
			E[i].x = i/3.0;
			E[i].y = maxL[0]*points[i].x+maxL[1]*points[i].y+maxL[2];
			if(E[i].y > 0.0)
			{
				inMaxLine = true;
			}
		}

		if(!inMaxLine)
		{
			return result;
		}

		if(E[0].y < 0.0)
		{
			//得到E0到其他端点连成的线,例如E0-E1,E0-E2,E0-E3...
			for(int i = 0; i < 3; i++)
			{
				LE[i].points[0] = E[0];
				LE[i].points[1] = E[i];
				vx[i] = -LE[i].c()/LE[i].a();
				if(vx[i] > 0.0 && vx[i] < 1.0 && minT > vx[i])
				{
					minT = vx[i];
				}
			}
		}

		if(E[3].y < 0.0)
		{
			//得到E3到其他端点连成的线,例如E3-E2,E3-E1,E3-E0...
			for(int i = 2; i >= 0; i--)
			{
				LE[i].points[0] = E[3];
				LE[i].points[1] = E[i];
				vx[i] = -LE[i].c()/LE[i].a();
				if(vx[i] > 0.0 && vx[i] < 1.0 && maxT < vx[i])
				{
					maxT = vx[i];
				}
			}
		}

		if(minT > 0.0 && minT < 1.0 && maxT > 0.0 && maxT < 1.0)
		{
			//assert(minT < maxT);
			if(minT > maxT)
			{
				result.push_back(*this);
			}
			else
			{
				std::vector<CubicBezierLine> subCurve = split(minT,maxT);
				if(subCurve.size() == 3)
				{
					result.push_back(subCurve[1]);
				}
				else
				{
					assert(false);
				}
			}
			return result;
		}
		else if(minT > 0.0 && minT < 1.0)
		{
			std::vector<CubicBezierLine> subCurve = split(minT);
			assert(subCurve.size() == 2);
			result.push_back(subCurve[1]);
			return result;
		}
		else if(maxT > 0.0 && maxT < 1.0)
		{
			std::vector<CubicBezierLine> subCurve = split(maxT);
			assert(subCurve.size() == 2);
			result.push_back(subCurve[0]);
			return result;
		}
		else
		{
			result.push_back(*this);
			return result;
		}

	}

	//! 使用两条平行线切割当前曲线,丢弃掉平行线外的部分.
	/*!
		返回被切割后的曲线.
	*/
	std::vector<CubicBezierLine> clipByParallelLine(StraightLine lineA,StraightLine lineB)
	{
		std::vector<CubicBezierLine> result;

		float a1 = lineA.a();
		float b1 = lineA.b();
		float c1 = lineA.c();
		float a2 = lineB.a();
		float b2 = lineB.b();
		float c2 = lineB.c();

		return clipByParallelLine(a1,b1,c1,a2,b2,c2);
	}


	//! 使用fat line切割line,丢弃fat line之外的部分.
	std::vector<CubicBezierLine> clipByFatLine(CubicBezierLine clipLine)
	{
		//L为过P0和P3的线段.
		StraightLine L(clipLine.points[0],clipLine.points[3]);
		//找到过控制点且和L平行的线.

		float a = L.a();
		float b = L.b();
		float c = L.c();
		float c1 = -a*clipLine.points[1].x-b*clipLine.points[1].y;
		float c2 = -a*clipLine.points[2].x-b*clipLine.points[2].y;
		//找到最大和最小的c.
		float minC = c;
		float maxC = c;
		if(minC > c1)
		{
			minC = c1;
		}
		if(minC > c2)
		{
			minC = c2;
		}
		if(maxC < c1)
		{
			maxC = c1;
		}
		if(maxC < c2)
		{
			maxC = c2;
		}

		return clipByParallelLine(a,b,minC,a,b,maxC);		
	}

	std::vector<CubicBezierLine> split(float t1,float t2)
	{
		assert(t1 <= t2);
		if(FloatEqual(t1,0.0))
		{
			return split(t2);
		}
		else if(FloatEqual(t2,1.0))
		{
			return split(t1);
		}
		else
		{
			std::vector<CubicBezierLine> result;
			std::vector<CubicBezierLine> subCurve1 = split(t1);
			assert(subCurve1.size() == 2);
			result.push_back(subCurve1[0]);
			t2 = (t2-t1)/(1.0-t1);
			std::vector<CubicBezierLine> subCurve2 = 
				subCurve1[1].split(t2);
			assert(subCurve2.size() == 2);
			result.push_back(subCurve2[0]);
			result.push_back(subCurve2[1]);
			return result;
		}
	}

	std::vector<float> getInflections() const
	{
		std::vector<float> result;

		result.push_back(0);
		result.push_back(1);

		std::vector<float> roots;

		//求1阶和2阶导数的根.
		float p0 = points[0].x;
		float p1 = points[1].x;
		float p2 = points[2].x;
		float p3 = points[3].x;
		
		roots = root(p0,p1,p2,p3,1);
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

	virtual Rect boundingBox() const
	{
		std::vector<float> inflections = getInflections();
		bool inited = false;
		float minX = 0;
		float minY = 0;
		float maxX = 0;
		float maxY = 0;
		for(int i=0; i<inflections.size(); i++) {
			float t = inflections[i];
			float x = getX(t);
			float y = getY(t);
			if(!inited)
			{
				minX = maxX = getX(t);
				minY = maxY = getY(t);
				inited = true;
			}
			if(x < minX) { minX = x; }
			if(x > maxX) { maxX = x; }
			if(y < minY) { minY = y; }
			if(y > maxY) { maxY = y; }
		}
		
		return Rect(minX,minY,maxX,maxY);
	}

	XRect tightBoundingBox()
	{
		const float PI = 3.1415926;
		float ox = points[0].x;
		float oy = points[0].y;
		float angle = atan2(points[3].y - points[0].y, points[3].x - points[0].x) + PI;
		float ca = cos(angle);
		float sa = sin(angle);
		float nx,ny;
		Rect boundingBox = align(points[0],points[3]).boundingBox();
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

	virtual Point midpoint() const
	{
		return Point(getX(0.5),getY(0.5));
	}

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

	CubicBezierLine align(Point start, Point end)
	{
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
		if(FloatEqual(a,0))
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
		if(FloatEqual(a,0))
		{
			if(FloatEqual(b,0))
			{
				return result;
			}
			float root = -c/b;
			if(root >= 0.0 && root <= 1.0)
			{
				result.push_back(root);
			}
			return result;
		}
		else
		{
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
	}

	//! 求一元三次方程.参考wikipedia中求根公式法和三角函数解.
	std::vector<float> cubicRoot(float a,float b,float c,float d) const
	{
		const float PI = 3.1415926;
		float A = b/a;
		float B = c/a;
		float C = d/a;

		float Q, R, D, S, T, Im;

		Q = (3*B - pow(A, 2))/9.0;
		R = (9*A*B - 27*C - 2*pow(A, 3))/54.0;
		D = pow(Q, 3) + pow(R, 2);    // polynomial discriminant

		float t[3] = {};

		if (D >= 0)                                 // complex or duplicate roots
		{
			S = sgn(R + sqrt(D))*pow(abs(R + sqrt(D)),(1/3));
			T = sgn(R - sqrt(D))*pow(abs(R - sqrt(D)),(1/3));

			t[0] = -A/3 + (S + T);                    // real root
			t[1] = -A/3 - (S + T)/2;                  // real part of complex root
			t[2] = -A/3 - (S + T)/2;                  // real part of complex root
			Im = abs(sqrt(3.0)*(S - T)/2);    // complex part of root pair   

			/*discard complex roots*/
			if (Im!=0)
			{
				t[1]=-1;
				t[2]=-1;
			}

		}
		else                                          // distinct real roots
		{
			float th = acos(R/sqrt(-pow(Q, 3)));

			t[0] = 2*sqrt(-Q)*cos(th/3) - A/3;
			t[1] = 2*sqrt(-Q)*cos((th + 2*PI)/3) - A/3;
			t[2] = 2*sqrt(-Q)*cos((th + 4*PI)/3) - A/3;
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
	virtual int pointCount() const {return points.size();}
	virtual Point point(int index) const 
	{
		assert(index >= 0);
		assert(index < points.size());
		return points[index];
	}

	//originBeginT和originEndT记录了该条曲线的t的范围.
	//假如不是0,1,则说明该曲线是被分割出来的一条子曲线.
	float originBeginT;
	float originEndT;
};

inline std::vector<Point> IntersectStraightAndBezierLine(const Line &straight, const Line &bezier)
{
	std::vector<Point> result;

	CubicBezierLine origin = ((CubicBezierLine &)bezier);

	CubicBezierLine aligned = 
		origin.align(straight.point(0),straight.point(1));

	float p0 = aligned.points[0].y;
	float p1 = aligned.points[1].y;
	float p2 = aligned.points[2].y;
	float p3 = aligned.points[3].y;

	std::vector<float> roots = aligned.root(p0,p1,p2,p3,0);

	for(int i = 0; i < roots.size(); i++)
	{
		Point p(origin.getX(roots[i]),origin.getY(roots[i]));
		if(straight.boundingBox().contains(p))
		{
			result.push_back(p);
		}
	}

	return result;
}

inline std::vector<Point> IntersectStraightLine(const Line &lineA, const Line &lineB)
{
	std::vector<Point> result;

	float lineA_x1 = lineA.point(0).x;
	float lineA_y1 = lineA.point(0).y;
	float lineA_x2 = lineA.point(1).x;
	float lineA_y2 = lineA.point(1).y;


	float A1 = lineA.point(1).y - lineA.point(0).y;
	float B1 = lineA.point(0).x - lineA.point(1).x;
	float C1 = A1*lineA.point(0).x+B1*lineA.point(0).y;

	float A2 = lineB.point(1).y - lineB.point(0).y;
	float B2 = lineB.point(0).x - lineB.point(1).x;
	float C2 = A2*lineB.point(0).x+B2*lineB.point(0).y;

	float det = A1*B2 - A2*B1;
	if(FloatEqual(det,0))
	{

	}
	else
	{
		float x = (B2*C1 - B1*C2)/det;
		float y = (A1*C2 - A2*C1)/det;
		Point p(x,y);

		//判断交点是否在两条线段上.
		if(lineA.boundingBox().contains(p)
			&& lineB.boundingBox().contains(p))
		{
			result.push_back(Point(x,y));
		}
	}

	return result;
}

class TestPainter
{
public:
	virtual void newFrame() {};
	virtual void addBezier(CubicBezierLine line) {};
	virtual void addLine(StraightLine line) {};
	virtual void waitNextFrame() {};
};

inline std::vector<Point> IntersectBezierAndBezierLine(
	const Line &lineA, const Line &lineB,TestPainter *painter = new TestPainter)
{
	std::vector<Point> result;

	CubicBezierLine &lA = (CubicBezierLine &)lineA;
	CubicBezierLine &lB = (CubicBezierLine &)lineB;

	std::vector<CubicBezierLine> groupA;
	std::vector<CubicBezierLine> groupB;
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

		//假如a,b足够小,则认为找到了交点.

		if(a.originTRange() < 0.001)
		{
			float t = (a.originEndT+a.originBeginT)/2.0;
			result.push_back(Point(a.getX(t),a.getY(t)));
		}
		else if(b.originTRange() < 0.001)
		{
			float t = (b.originEndT+b.originBeginT)/2.0;
			result.push_back(Point(b.getX(t),b.getY(t)));
		}
		else
		{
			counter++;
			if(counter%2 == 0)
			{
				XRect bb = b.tightBoundingBox();

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
					&& !FloatEqual(a.originTRange(),0)
					&& sub[0].originTRange()/a.originTRange() > 0.55)
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
					&& !FloatEqual(a.originTRange(),0)
					&& sub[0].originTRange()/a.originTRange() > 0.55)
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
					&& !FloatEqual(a.originTRange(),0)
					&& sub[0].originTRange()/a.originTRange() > 0.25)
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
				XRect bb = a.tightBoundingBox();

				std::vector<CubicBezierLine> sub = b.clipByFatLine(a);

				painter->newFrame();
				painter->addBezier(a);
				for(int i = 0; i < sub.size(); i++)
				{
					painter->addBezier(sub[i]);
				}
				painter->waitNextFrame();

				if(sub.size() == 1
					&& !FloatEqual(b.originTRange(),0)
					&& sub[0].originTRange()/b.originTRange() > 0.55)
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
					&& !FloatEqual(b.originTRange(),0)
					&& sub[0].originTRange()/b.originTRange() > 0.55)
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
					&& !FloatEqual(b.originTRange(),0)
					&& sub[0].originTRange()/b.originTRange() > 0.25)
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

			if(counter > 10000)
			{
				assert(!"Intersect Bezier does not converge.");
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