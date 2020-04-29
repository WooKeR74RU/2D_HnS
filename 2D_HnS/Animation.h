#pragma once

#include <SFML/Graphics.hpp>

class Animation : public sf::Drawable, public sf::Transformable
{
public:
	Animation(std::string texturePath, int frameWidth, int frameHeight, sf::Time delay, int framesCount);

	void update(sf::Time elapsed);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::Texture _texture;
	int _frameWidth;
	int _frameHeight;
	sf::Time _delay;
	int _framesCount;
	std::vector<sf::IntRect> _frames;
	int _curFrame;
	sf::Time _stopwatch;
	sf::Sprite _sprite;
};
