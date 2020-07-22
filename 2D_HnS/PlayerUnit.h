#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "EquilateralPolygon.h"
#include "Tentacle.h"
#include "Vector.h"

class PlayerUnit : public sf::Drawable, public sf::Transformable
{
public:
	PlayerUnit(int size, int sidesCount);

	void update(sf::Time elapsed);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	double _size;
	int _sidesCount;
	EquilateralPolygon _equilateralPolygon;

	Vector _controlVelocity;
	bool _isInDash;
	Vector _dashVelocity;
	sf::Time _dashTimer;
};
