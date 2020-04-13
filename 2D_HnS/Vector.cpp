#include "Vector.h"

#include "Point.h"
#include "Geometry.h"

Vector::Vector(double x, double y) : x(x), y(y)
{ }
Vector::Vector(const Point& a, const Point& b) : x(b.x - a.x), y(b.y - a.y)
{ }
Vector::Vector(const Point& point) : x(point.x), y(point.y)
{ }
bool Vector::operator==(const Vector& other) const
{
	return Point(*this) == Point(other);
}
bool Vector::operator!=(const Vector& other) const
{
	return !operator==(other);
}
Vector Vector::operator+(const Vector& other) const
{
	return Vector(x + other.x, y + other.y);
}
Vector Vector::operator-(const Vector& other) const
{
	return Vector(x - other.x, y - other.y);
}
Vector Vector::operator-() const
{
	return Vector(-x, -y);
}
Vector Vector::operator*(double k) const
{
	return Vector(x * k, y * k);
}
Vector Vector::operator/(double k) const
{
	return Vector(x / k, y / k);
}
double Vector::getLength() const
{
	return distance(Point(0, 0), Point(x, y));
}
void Vector::resize(double length)
{
	*this = *this / getLength() * length;
}
void Vector::rotate90()
{
	*this = Vector(y, -x);
}
