#include "Utilities.h"

#include <cmath>
#include <SFML/Graphics.hpp>
#include "Resources.h"

const double EPS = 1e-6;
bool doubleEqual(double a, double b)
{
	return std::abs(a - b) < EPS;
}
bool doubleLess(double a, double b)
{
	return !doubleEqual(a, b) && a < b;
}

void drawDot(double x, double y)
{
	static const int DOT_RADIUS = 5;
	sf::CircleShape dot(DOT_RADIUS);
	dot.setFillColor(sf::Color::Red);
	dot.setPosition(x - DOT_RADIUS, y - DOT_RADIUS);
	RS().window.draw(dot);
}
