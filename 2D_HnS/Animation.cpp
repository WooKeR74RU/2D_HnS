#include "Animation.h"

Animation::Animation(std::string texturePath, int frameWidth, int frameHeight, sf::Time delay, int framesCount) :
	_frameWidth(frameWidth), _frameHeight(frameHeight), _delay(delay), _framesCount(framesCount), _curFrame(0)
{
	if (!_texture.loadFromFile(texturePath))
		std::exit(EXIT_FAILURE);

	int w = _texture.getSize().x / _frameWidth;
	int h = _texture.getSize().y / _frameHeight;
	for (int i = 0; i < h && _frames.size() < _framesCount; i++)
	{
		for (int j = 0; j < w && _frames.size() < _framesCount; j++)
			_frames.push_back(sf::IntRect(j * _frameWidth, i * _frameHeight, _frameWidth, _frameHeight));
	}

	_sprite.setTexture(_texture);
	_sprite.setTextureRect(_frames.front());
}

void Animation::update(sf::Time elapsed)
{
	_stopwatch += elapsed;
	while (_stopwatch >= _delay)
	{
		_stopwatch -= _delay;
		_curFrame++;
		if (_curFrame == _framesCount)
			_curFrame = 0;
		_sprite.setTextureRect(_frames[_curFrame]);
	}
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(_sprite, states);
}
