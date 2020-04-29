#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "EquilateralPolygon.h"
#include "Tentacle.h"
#include "Vector.h"

class PlayerUnit : public sf::Drawable, public sf::Transformable
{
public:
	PlayerUnit();

	void update(sf::Time elapsed);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Vector _controlVelocity;
	bool _isInDash;
	Vector _dashVelocity;
	sf::Time _dashTimer;

	double _size;
	static const int MIN_FRAGMENTS_COUNT = 3;
	static const int MAX_FRAGMENTS_COUNT = 20;
	int _fragmentsCount;
	EquilateralPolygon _equilateralPolygon;
	std::array<Tentacle, MAX_FRAGMENTS_COUNT> _tentacles;
	sf::CircleShape _center;
};
