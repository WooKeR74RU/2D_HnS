#include "PlayerUnit.h"

#include "Resources.h"
#include "Utilities.h"

PlayerUnit::PlayerUnit(int size, int sidesCount)
{
	_size = size;
	_sidesCount = sidesCount;
	_isInDash = false;

	_equilateralPolygon.setRadius(_size / 2);
	_equilateralPolygon.setSidesCount(_sidesCount);
}

void PlayerUnit::update(sf::Time elapsed)
{
	Vector moveVector;

	static const double MOVE_SPEED = 300;
	static const sf::Time SPEED_UP_TIME = sf::seconds(0.1);
	static const sf::Time SPEED_DOWN_TIME = sf::seconds(0.3);

	static const double MOVE_FORCE = MOVE_SPEED / SPEED_UP_TIME.asSeconds();
	static const double MOVE_FRICTION = MOVE_SPEED / SPEED_DOWN_TIME.asSeconds();

	static const int CONTROLS_COUNT = 4;
	static const sf::Keyboard::Key CONTROLS[CONTROLS_COUNT] = { sf::Keyboard::W, sf::Keyboard::D,
		sf::Keyboard::S, sf::Keyboard::A };
	static const Vector CONTROL_FORCES[CONTROLS_COUNT] = { Vector(0, -MOVE_FORCE), Vector(MOVE_FORCE, 0),
		Vector(0, MOVE_FORCE), Vector(-MOVE_FORCE, 0) };

	Vector controlForce;
	for (int i = 0; i < CONTROLS_COUNT; i++)
	{
		if (sf::Keyboard::isKeyPressed(CONTROLS[i]))
			controlForce += CONTROL_FORCES[i];
	}
	_controlVelocity += controlForce * elapsed.asSeconds();

	_controlVelocity.decrease(MOVE_FRICTION * elapsed.asSeconds());
	if (MOVE_SPEED < _controlVelocity.getLength())
		_controlVelocity.resize(MOVE_SPEED);

	moveVector += _controlVelocity * elapsed.asSeconds();

	static const double DASH_DISTANCE = 150;
	static const sf::Time DASH_TIME = sf::seconds(0.2);
	static const double DASH_BRAKING_DISTANCE = 50;
	static const sf::Time DASH_BRAKING_TIME = sf::seconds(0.3);

	static const sf::Time DASH_TOTAL_TIME = DASH_TIME + DASH_BRAKING_TIME;
	static const double DASH_SPEED = 2 * (DASH_DISTANCE / DASH_TIME.asSeconds() - DASH_BRAKING_DISTANCE / DASH_BRAKING_TIME.asSeconds());
	static const double DASH_FRICTION = 2 * (DASH_SPEED / DASH_TIME.asSeconds() - DASH_DISTANCE / sqr(DASH_TIME.asSeconds()));
	static const double DASH_BRAKING_SPEED = 2 * DASH_BRAKING_DISTANCE / DASH_BRAKING_TIME.asSeconds();
	static const double DASH_BRAKING_FRICTION = 2 * (DASH_BRAKING_SPEED / DASH_BRAKING_TIME.asSeconds() - DASH_BRAKING_DISTANCE / sqr(DASH_BRAKING_TIME.asSeconds()));

	if (_isInDash)
	{
		_dashTimer += elapsed;
		if (_dashTimer < DASH_TOTAL_TIME)
		{
			if (_dashTimer < DASH_TIME)
				_dashVelocity.resize(DASH_SPEED - DASH_FRICTION * _dashTimer.asSeconds());
			else
				_dashVelocity.resize(DASH_BRAKING_SPEED - DASH_BRAKING_FRICTION * (_dashTimer - DASH_TIME).asSeconds());
		}
		else
		{
			_dashVelocity = Vector::Zero;
			_isInDash = false;
		}
	}
	else
	{
		if (RS().eventInput.isKeyPressed(sf::Keyboard::Space))
		{
			_isInDash = true;
			_dashTimer = sf::Time::Zero;
			_dashVelocity = _controlVelocity;
			_dashVelocity.resize(DASH_SPEED);
		}
	}

	moveVector += _dashVelocity * elapsed.asSeconds();

	move(moveVector.x, moveVector.y);

	sf::View view(RS().window.getView());
	view.setCenter(getPosition());
	RS().window.setView(view);
}

void PlayerUnit::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(_equilateralPolygon, states);
}
