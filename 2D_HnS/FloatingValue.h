#pragma once

#include <random>
#include <SFML/Graphics.hpp>

class FloatingValue
{
public:
	FloatingValue();
	FloatingValue(double min, double max, double floatingSpeed, double minTransitionPart = 0.25, bool isSmooth = true);
	
	void setLimits(double min, double max);
	void setFloatingSpeed(double floatingSpeed);

	void update(sf::Time elapsed);
	double getValue() const;

private:
	void resetAimValue();
	double getSmoothCoefficent() const;

	double _min;
	double _max;
	double _floatingSpeed;
	double _minTransitionPart;
	bool _isSmooth;

	std::uniform_real_distribution<double> _dis;
	double _beginValue;
	double _endValue;
	double _curValue;

	static std::mt19937 _gen;
};
