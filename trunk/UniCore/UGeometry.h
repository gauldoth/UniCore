/*!	\file UGeometry.h
    \brief 包含了线,曲线,几何图形等的处理逻辑.
	
	\author uni
	\date 2014-10-29
*/

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

#include <vector>
#include <cassert>

namespace uni
{


class UPoint
{
public:
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

class UCubicBezierLine : public UGeometryLine
{
public:
	UCubicBezierLine(float x1, float y1, float x2, float y2, 
		float x3, float y3, float x4, float y4)
	{

	}
	UCubicBezierLine(UPoint a1, UPoint a2, UPoint a3, UPoint a4)
	{

	}
private:
	std::vector<UPoint> points_;
};

inline std::vector<UPoint> IntersectStraightAndBezierLine(const UGeometryLine &straight, const UGeometryLine &bezier)
{
	std::vector<UPoint> result;

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