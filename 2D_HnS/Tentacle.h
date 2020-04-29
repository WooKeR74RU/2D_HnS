#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "Point.h"
#include "FloatingValue.h"

class Tentacle : public sf::Drawable, public sf::Transformable
{
public:
	Tentacle();

	void setLength(double length);
	void setBodyThickness(double bodyThickness);
	void setColor(sf::Color color);

	void update(sf::Time elapsed);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	void updateBehFuncM();
	
	double thicknessFunc(double curLen);
	double behaviorFunc(double curX);

	double _length;
	double _bodyThickness;
	sf::Color _color;

	static const int BEHAVIOR_COMPONENTS = 3;
	std::array<FloatingValue, BEHAVIOR_COMPONENTS> _behFuncM1;
	std::array<FloatingValue, BEHAVIOR_COMPONENTS> _behFuncM2;

	std::vector<Point> _leftBound;
	std::vector<Point> _rightBound;
	sf::VertexArray _triangleStrip;
	sf::CircleShape _body;
};
