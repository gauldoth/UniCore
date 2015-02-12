#include "UGeometry.h"


namespace uni
{

float RawDistance( Point a, Point b )
{
	float deltaX = b.x-a.x;
	float deltaY = b.y-a.y;

	return deltaX*deltaX+deltaY*deltaY;
}

bool FloatEqual( float x,float y )
{
	float delta = fabs(x-y);
	return (delta <= FLT_EPSILON * std::max(1.0f, std::max(fabs(x), fabs(y))) );
}


}//namespace uni