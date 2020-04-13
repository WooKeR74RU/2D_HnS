#include "Geometry.h"

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include "Utilities.h"

double toDeg(double rad)
{
	return rad / M_PI * 180.0;
}
double toRad(double deg)
{
	return deg / 180.0 * M_PI;
}

double distance(const Point& a, const Point& b)
{
	return std::sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

double scalarProduct(const Vector& a, const Vector& b)
{
	return a.x * b.x + a.y * b.y;
}
double vectorProduct(const Vector& a, const Vector& b)
{
	return a.x * b.y - b.x * a.y;
}

double getSinBetween(const Vector& a, const Vector& b)
{
	return vectorProduct(a, b) / a.getLength() / b.getLength();
}
double getCosBetween(const Vector& a, const Vector& b)
{
	return scalarProduct(a, b) / a.getLength() / b.getLength();
}

double getAngle(const Vector& a, const Vector& b)
{
	double cos = scalarProduct(a, b) / a.getLength() / b.getLength();
	cos = std::min(cos, 1.0);
	cos = std::max(cos, -1.0);
	return std::acos(cos);
}

bool isPointOnLine(const Point& p1, const Point& p2, const Point& point)
{
	Vector edge(p1, p2);
	Vector v(p1, point);
	return doubleEqual(vectorProduct(edge, v), 0);
}
