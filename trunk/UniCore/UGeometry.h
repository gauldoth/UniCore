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

namespace uni
{


class UPoint
{
public:
	UPoint() {}
	UPoint(float x, float y)
		:x_(x),y_(y)
	{
	}
	float x() const {return x_;}
	float y() const {return y_;}
private:
	float x_;
	float y_;
};

class UGeometryLine
{
public:
	enum Type
	{
		Straight,
		CubicBezier,
	};
	UGeometryLine() {}
	virtual Type type() const = 0;
	virtual int pointCount() const = 0;
	virtual UPoint point(int index) const = 0;


};

class UStraightLine : public UGeometryLine
{
public:
	UStraightLine(float x1, float y1, float x2, float y2)
	{
		points_.push_back(UPoint(x1,y1));
		points_.push_back(UPoint(x2,y2));
	}
	UStraightLine(UPoint a1, UPoint a2)
	{
		points_.push_back(a1);
		points_.push_back(a2);
	}
	UPoint getProjectionPoint(UPoint p)
	{
		return p;
	}
	virtual Type type() const {return Straight;}
	virtual int pointCount() const {return points_.size();}
	virtual UPoint point(int index) const 
	{
		assert(index >= 0);
		assert(index < points_.size());
		return points_[index];
	}
private:
	std::vector<UPoint> points_;
};

static float binomials[][5] = 
{
	{1,0,0,0,0},
	{1,1,0,0,0},
	{1,2,1,0,0},
	{1,3,3,1,0},
};

class UCubicBezierLine : public UGeometryLine
{
public:
	UCubicBezierLine(float x1, float y1, float x2, float y2, 
		float x3, float y3, float x4, float y4)
	{
		points_.push_back(UPoint(x1,y1));
		points_.push_back(UPoint(x2,y2));
		points_.push_back(UPoint(x3,y3));
		points_.push_back(UPoint(x4,y4));

	}
	UCubicBezierLine(UPoint a1, UPoint a2, UPoint a3, UPoint a4)
	{
		points_.push_back(a1);
		points_.push_back(a2);
		points_.push_back(a3);
		points_.push_back(a4);
	}
	UCubicBezierLine(const std::vector<UPoint> &points)
	{
		points_ = points;
	}

	UPoint getProjectionPoint(UPoint p)
	{
		UPoint result;
		return result;
	}

	UCubicBezierLine align(UPoint start, UPoint end)
	{
		float angle = atan2(end.y()-start.y(),end.x()-start.x());
		float ca = cos(-angle);
		float sa = sin(-angle);
		float ox = start.x();
		float oy = start.y();
		
		std::vector<UPoint> alignedPoints;

		for(int i = 0; i < 4; i++)
		{
			float x = points_[i].x();
			float y = points_[i].y();
			float alignedX = 
				ca * (x-ox) - sa * (y-oy);
			float alignedY = 
				sa * (x-ox) + ca * (y-oy);

			alignedPoints.push_back(UPoint(alignedX,alignedY));
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
		for(int i = 0; i < points_.size(); i++)
		{
			v.push_back(points_[i].y());
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
			if(points_[k].x() == 0)
			{
				continue;
			}
			float polyterm = pow((1-t),n-k) * pow(t,k);
			result += binomials[n][k] * polyterm * points_[k].x();
		}

		return result;
	}
	float getY(float t)
	{
		const int n = 3;
		float result = 0;
		for(int k = 0; k <= n; k++)
		{
			if(points_[k].y() == 0)
			{
				continue;
			}
			float polyterm = pow((1-t),n-k) * pow(t,k);
			result += binomials[n][k] * polyterm * points_[k].y();
		}

		return result;
	}
	virtual Type type() const {return CubicBezier;}
	virtual int pointCount() const {return points_.size();}
	virtual UPoint point(int index) const 
	{
		assert(index >= 0);
		assert(index < points_.size());
		return points_[index];
	}
private:
	std::vector<UPoint> points_;
};

inline std::vector<UPoint> IntersectStraightAndBezierLine(const UGeometryLine &straight, const UGeometryLine &bezier)
{
	std::vector<UPoint> result;

	UCubicBezierLine origin = ((UCubicBezierLine &)bezier);

	UCubicBezierLine aligned = 
		origin.align(straight.point(0),straight.point(1));

	std::vector<float> roots = aligned.findAllRoots();

	for(int i = 0; i < roots.size(); i++)
	{
		result.push_back(UPoint(origin.getX(roots[i]),origin.getY(roots[i])));
	}

	return result;
}

inline std::vector<UPoint> IntersectStraightLine(const UGeometryLine &lineA, const UGeometryLine &lineB)
{
	std::vector<UPoint> result;

	float lineA_x1 = lineA.point(0).x();
	float lineA_y1 = lineA.point(0).y();
	float lineA_x2 = lineA.point(1).x();
	float lineA_y2 = lineA.point(1).y();


	float A1 = lineA.point(1).y() - lineA.point(0).y();
	float B1 = lineA.point(0).x() - lineA.point(1).x();
	float C1 = A1*lineA.point(0).x()+B1*lineA.point(0).y();

	float A2 = lineB.point(1).y() - lineB.point(0).y();
	float B2 = lineB.point(0).x() - lineB.point(1).x();
	float C2 = A2*lineB.point(0).x()+B2*lineB.point(0).y();

	float det = A1*B2 - A2*B1;
	if(det == 0)
	{

	}
	else
	{
		float x = (B2*C1 - B1*C2)/det;
		float y = (A1*C2 - A2*C1)/det;

		//判断交点是否在两条线段上.


		result.push_back(UPoint(x,y));
	}

	return result;
}

inline std::vector<UPoint> Intersect(const UGeometryLine &lineA, const UGeometryLine &lineB)
{
	std::vector<UPoint> result;
	if(lineA.type() == UGeometryLine::Straight
		&& lineB.type() == UGeometryLine::Straight)
	{
		result = IntersectStraightLine(lineA,lineB);
	}
	else if(lineA.type() == UGeometryLine::Straight && lineB.type() == UGeometryLine::CubicBezier)
	{
		result = IntersectStraightAndBezierLine(lineA,lineB);
	}
	else if(lineA.type() == UGeometryLine::CubicBezier && lineB.type() == UGeometryLine::Straight)
	{
		result = IntersectStraightAndBezierLine(lineB,lineA);
	}
	return result;
}



}//namespace uni