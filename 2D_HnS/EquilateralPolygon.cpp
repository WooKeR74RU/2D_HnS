#define _USE_MATH_DEFINES
#include "EquilateralPolygon.h"

#include <cmath>

EquilateralPolygon::EquilateralPolygon(double radius, int sidesCount) : _radius(radius), _sidesCount(sidesCount)
{
	update();
}

void EquilateralPolygon::setRadius(double radius)
{
	_radius = radius;
	update();
}
void EquilateralPolygon::setSidesCount(int sidesCount)
{
	_sidesCount = sidesCount;
	update();
}

std::size_t EquilateralPolygon::getPointCount() const
{
	return _sidesCount;
}
sf::Vector2f EquilateralPolygon::getPoint(std::size_t index) const
{
	double angle = index * 2 * M_PI / _sidesCount - M_PI / 2;
	double x = std::cos(angle) * _radius;
	double y = std::sin(angle) * _radius;
	return sf::Vector2f(x, y);
}
