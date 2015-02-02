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

inline bool FloatEqual(float x,float y)
{
	return (fabs(x-y) <= FLT_EPSILON * std::max(1.0f, std::max(fabs(x), fabs(y))) );
}

class Rect
{
public:
	Rect(float l1,float t1,float r1,float b1)
		:l(l1),t(t1),r(r1),b(b1)
	{

	}
	float l;
	float t;
	float r;
	float b;
};

class Point
{
public:
	Point(float x1, float y1)
		:x(x1),y(y1)
	{
	}
public:
	float x;
	float y;
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
	virtual Point getNearestPoint(Point p) = 0;
	std::vector<Point> points;
};

//! 直线段
class StraightLine : public Line
{
public:
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

	virtual Type type() const {return Straight;}
	virtual int pointCount() const {return points.size();}
	virtual Point point(int index) const
	{
		assert(index >= 0);
		assert(index < points.size());
		return points[index];
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
class UCubicBezierLine : public Line
{
public:
	UCubicBezierLine(float x1, float y1, float x2, float y2, 
		float x3, float y3, float x4, float y4)
	{
		points.push_back(Point(x1,y1));
		points.push_back(Point(x2,y2));
		points.push_back(Point(x3,y3));
		points.push_back(Point(x4,y4));

	}
	UCubicBezierLine(Point a1, Point a2, Point a3, Point a4)
	{
		points.push_back(a1);
		points.push_back(a2);
		points.push_back(a3);
		points.push_back(a4);
	}
	UCubicBezierLine(const std::vector<Point> &points1)
	{
		points = points1;
	}

	std::vector<float> getInflections()
	{
		std::vector<float> result;

		std::vector<float> tValues;
		tValues.push_back(0);
		tValues.push_back(1);

		std::vector<float> roots;

		//findAllRoots()

		return result;
	}

	Rect boundingBox()
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

	UCubicBezierLine align(Point start, Point end)
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

		return UCubicBezierLine(alignedPoints);
	}
	std::vector<float> findAllRoots()
	{
		std::vector<float> result;

		for(float t = 0.0; t <= 1.0; t += 0.01)
		{
			
			float root = findRoot(t,0);
			if(root < 0.0 || root > 1.0)
			{
				continue;
			}
			if(abs(root-t) <= 0.000001) 
			{
				continue;
			}

			bool found = false;
			for(int i = 0; i < result.size(); i++)
			{
				if(abs(result[i]-root) <= 0.00001)
				{
					found = true;
					break;
				}
			}
			if(found)
			{
				continue;
			}

			result.push_back(root);
		}

		return result;
	}
	float findRoot(float t,int depth)
	{
		std::vector<float> v;
		for(int i = 0; i < points.size(); i++)
		{
			v.push_back(points[i].y);
		}
		float f = getDerivative(0,t,v);
		float df = getDerivative(1,t,v);
		float t2 = t-(f/df);

		if(df == 0)
		{
			t2 = t - f;
		}
		
		if(depth > 12)
		{
			if(abs(t-t2) < 0.000001)
			{
				return t2;
			}

			//assert(false);
		}

		if(abs(t-t2) > 0.000001)
		{
			return findRoot(t2,depth+1);
		}

		return t2;
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
	float getX(float t)
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
	float getY(float t)
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
};

inline std::vector<Point> IntersectStraightAndBezierLine(const Line &straight, const Line &bezier)
{
	std::vector<Point> result;

	UCubicBezierLine origin = ((UCubicBezierLine &)bezier);

	UCubicBezierLine aligned = 
		origin.align(straight.point(0),straight.point(1));

	std::vector<float> roots = aligned.findAllRoots();

	for(int i = 0; i < roots.size(); i++)
	{
		result.push_back(Point(origin.getX(roots[i]),origin.getY(roots[i])));
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
	if(det == 0)
	{

	}
	else
	{
		float x = (B2*C1 - B1*C2)/det;
		float y = (A1*C2 - A2*C1)/det;

		//判断交点是否在两条线段上.


		result.push_back(Point(x,y));
	}

	return result;
}

inline std::vector<Point> IntersectBezierAndBezierLine(
	const Line &lineA, const Line &lineB)
{
	std::vector<Point> result;
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