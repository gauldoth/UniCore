#include "UGeometry.h"


namespace uni
{

float RawDistance( Point a, Point b )
{
	float deltaX = b.x-a.x;
	float deltaY = b.y-a.y;

	return deltaX*deltaX+deltaY*deltaY;
}


}//namespace uni