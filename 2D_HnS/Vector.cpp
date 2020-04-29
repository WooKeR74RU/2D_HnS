#include "Vector.h"

#include "Point.h"
#include "Geometry.h"
#include "Utilities.h"

const Vector Vector::Zero(0, 0);

Vector::Vector() : x(0), y(0)
{ }
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
Vector Vector::operator-() const
{
	return Vector(-x, -y);
}
Vector Vector::operator+(const Vector& other) const
{
	return Vector(x + other.x, y + other.y);
}
void Vector::operator+=(const Vector& other)
{
	*this = *this + other;
}
Vector Vector::operator-(const Vector& other) const
{
	return Vector(x - other.x, y - other.y);
}
void Vector::operator-=(const Vector& other)
{
	*this = *this - other;
}
Vector Vector::operator*(double k) const
{
	return Vector(x * k, y * k);
}
void Vector::operator*=(double k)
{
	*this = *this * k;
}
Vector Vector::operator/(double k) const
{
	return Vector(x / k, y / k);
}
void Vector::operator/=(double k)
{
	*this = *this / k;
}
double Vector::getLength() const
{
	return distance(Point(0, 0), Point(x, y));
}
void Vector::resize(double length)
{
	if (doubleEqual(length, 0) || doubleEqual(getLength(), 0))
		*this = Zero;
	else
		*this = *this / getLength() * length;
}
void Vector::increase(double value)
{
	resize(getLength() + value);
}
void Vector::decrease(double value)
{
	if (value < getLength())
		resize(getLength() - value);
	else
		*this = Zero;
}
void Vector::rotate90()
{
	*this = Vector(y, -x);
}
