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

inline Point Lerp(Point a,Point b,float t)
{
	float x = a.x + (b.x-a.x)*t;
	float y = a.y + (b.y-a.y)*t;

	return Point(x,y);
}


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

	//! 使用fat line切割line,丢弃fat line之外的部分.
	std::vector<CubicBezierLine> clipByFatLine(CubicBezierLine lineToClip)
	{
		std::vector<CubicBezierLine> result;

		//L为过P0和P3的线段.
		StraightLine L(points[0],points[3]);
		//找到过控制点且和L平行的线.

		float a = L.a();
		float b = L.b();
		float c = L.c();
		float c1 = -a*points[1].x-b*points[1].y;
		float c2 = -a*points[2].x-b*points[2].y;
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
		float minL[3] = {-a,-b,-minC};
		float maxL[3] = {a,b,maxC};


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
			E[i].y = minL[0]*lineToClip.points[i].x+minL[1]*lineToClip.points[i].y+minL[2];
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
			E[i].y = maxL[0]*lineToClip.points[i].x+maxL[1]*lineToClip.points[i].y+maxL[2];
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
				result.push_back(lineToClip);
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
					//
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
			result.push_back(lineToClip);
			return result;
		}
		
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

		//假如a,b足够小,则认为找到了交点.

		if(a.originTRange() < 0.000001)
		{
			float t = (a.originEndT+a.originBeginT)/2.0;
			result.push_back(Point(a.getX(t),a.getY(t)));
		}
		else if(b.originTRange() < 0.000001)
		{
			float t = (b.originEndT+b.originBeginT)/2.0;
			result.push_back(Point(b.getX(t),b.getY(t)));
		}
		else
		{
			counter++;
			if(counter%2 == 0)
			{
				std::vector<CubicBezierLine> sub = a.clipByFatLine(b);
				if(sub.size() == 1)
				{
					sub = b.split(0.5);
				}

				for(int i = 0; i < sub.size(); i++)
				{
					groupA.push_back(a);
					groupB.push_back(sub[i]);
				}
			}
			else
			{
				std::vector<CubicBezierLine> sub = b.clipByFatLine(a);
				if(sub.size() == 1)
				{
					sub = a.split(0.5);
				}
				for(int i = 0; i < sub.size(); i++)
				{
					groupA.push_back(sub[i]);
					groupB.push_back(b);
				}
			}

			if(counter > 10000)
			{
				assert(false);
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