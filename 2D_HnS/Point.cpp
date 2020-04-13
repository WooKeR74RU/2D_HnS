#include "Point.h"

#include "Vector.h"
#include "Utilities.h"

Point::Point(double x, double y) : x(x), y(y)
{ }
Point::Point(const Vector& vector) : x(vector.x), y(vector.y)
{ }

bool Point::operator==(const Point& other) const
{
	return doubleEqual(x, other.x) && doubleEqual(y, other.y);
}
bool Point::operator!=(const Point& other) const
{
	return !operator==(other);
}

bool PointCmpY::operator()(Point* a, Point* b) const
{
	if (a->y != b->y)
		return a->y < b->y;
	return a->x < b->x;
}
bool PointCmpX::operator()(Point* a, Point* b) const
{
	if (a->x != b->x)
		return a->x < b->x;
	return a->y < b->y;
}
