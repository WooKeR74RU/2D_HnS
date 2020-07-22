#define _USE_MATH_DEFINES
#include "FloatingValue.h"

#include <cmath>
#include <chrono>
#include <algorithm>
#include "Utilities.h"

FloatingValue::FloatingValue() : _min(0), _max(1), _floatingSpeed(0.1), _minTransitionPart(0.25), _isSmooth(true)
{
	_dis = std::uniform_real_distribution<double>(_min, _max);
	_curValue = _dis(_gen);
	resetAimValue();
}

FloatingValue::FloatingValue(double min, double max, double floatingSpeed, double minTransitionPart, bool isSmooth) :
	_min(min), _max(max), _floatingSpeed(floatingSpeed), _minTransitionPart(minTransitionPart), _isSmooth(isSmooth)
{
	_dis = std::uniform_real_distribution<double>(_min, _max);
	_curValue = _dis(_gen);
	resetAimValue();
}

void FloatingValue::setLimits(double min, double max)
{
	_min = min;
	_max = max;
	_beginValue = std::max(_beginValue, _min);
	_beginValue = std::min(_beginValue, _max);
	_endValue = std::max(_endValue, _min);
	_endValue = std::min(_endValue, _max);
	_curValue = std::max(_curValue, _min);
	_curValue = std::min(_curValue, _max);
	_dis = std::uniform_real_distribution<double>(_min, _max);
	if (doubleEqual(_curValue, _endValue))
		resetAimValue();
}

void FloatingValue::setFloatingSpeed(double floatingSpeed)
{
	_floatingSpeed = floatingSpeed;
}

void FloatingValue::update(sf::Time elapsed)
{
	if (doubleLess(_curValue, _endValue))
	{
		_curValue += _floatingSpeed * getSmoothCoefficent() * elapsed.asSeconds();
		if (!doubleLess(_curValue, _endValue))
			resetAimValue();
	}
	else
	{
		_curValue -= _floatingSpeed * getSmoothCoefficent() * elapsed.asSeconds();
		if (doubleLess(_curValue, _endValue))
			resetAimValue();
	}
}

double FloatingValue::getValue() const
{
	return _curValue;
}

void FloatingValue::resetAimValue()
{
	_beginValue = _curValue;
	_endValue = _dis(_gen);
	while (std::abs(_endValue - _beginValue) < _minTransitionPart)
		_endValue = _dis(_gen);
}

double FloatingValue::getSmoothCoefficent() const
{
	if (!_isSmooth)
		return 1;
	static const double MIN_VAL = 0.1;
	double progress = (_curValue - _beginValue) / (_endValue - _beginValue);
	return MIN_VAL + std::sin(progress * M_PI);
}

//std::mt19937 FloatingValue::_gen(0);
std::mt19937 FloatingValue::_gen(std::chrono::system_clock::now().time_since_epoch().count());
