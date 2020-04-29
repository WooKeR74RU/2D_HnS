#pragma once

#include <SFML/Graphics.hpp>

class EquilateralPolygon : public sf::Shape
{
public:
	EquilateralPolygon(double radius = 100, int sidesCount = 3);

	void setRadius(double radius);
	void setSidesCount(int sidesCount);

private:
	virtual std::size_t getPointCount() const;
	virtual sf::Vector2f getPoint(std::size_t index) const;

	double _radius;
	int _sidesCount;
};
