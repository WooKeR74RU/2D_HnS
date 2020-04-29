#define _USE_MATH_DEFINES
#include "Tentacle.h"

#include <cmath>
#include "Vector.h"
#include "Geometry.h"
#include "Utilities.h"

Tentacle::Tentacle() : _length(450), _bodyThickness(40), _color(sf::Color::White)
{
	_body.setRadius(_bodyThickness / 2);
	_body.setPosition(-_bodyThickness / 2, -_bodyThickness / 2);
	_body.setFillColor(_color);

	_triangleStrip.setPrimitiveType(sf::TriangleStrip);

	updateBehFuncM();
	
	update(sf::Time::Zero);
}

void Tentacle::setLength(double length)
{
	_length = length;
	updateBehFuncM();
}
void Tentacle::setBodyThickness(double bodyThickness)
{
	_bodyThickness = bodyThickness;
	_body.setRadius(_bodyThickness / 2);
	_body.setPosition(-_bodyThickness / 2, -_bodyThickness / 2);
	updateBehFuncM();
}
void Tentacle::setColor(sf::Color color)
{
	_color = color;
	_body.setFillColor(_color);
}

void Tentacle::update(sf::Time elapsed)
{
	static const int SEG_COUNT = 30;
	static const int SEG_STEP_COUNT = 3;
	static const double MIN_SEG_LEN_LIMIT = 4;
	static const double MAX_SEG_LEN_LIMIT = 25;
	static const double MIN_SEG_LEN = 1;
	static const double DERIVATIVE_EPS = 1e-4;

	for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
	{
		_behFuncM1[i].update(elapsed);
		_behFuncM2[i].update(elapsed);
	}

	double curX = 0;
	double lastSegX = 0;
	double curLen = 0;
	double curSegLen = 0;

	double segLenLimit = _length / SEG_COUNT;
	segLenLimit = std::max(MIN_SEG_LEN_LIMIT, segLenLimit);
	segLenLimit = std::min(MAX_SEG_LEN_LIMIT, segLenLimit);
	
	double stepX = segLenLimit / SEG_STEP_COUNT;

	auto addBound = [&]() -> void
	{
		Point p1(curX, behaviorFunc(curX));
		Point p2(curX + DERIVATIVE_EPS, behaviorFunc(curX + DERIVATIVE_EPS));
		Vector edge(p1, p2);
		edge.resize(thicknessFunc(curLen) / 2);
		edge.rotate90();
		_leftBound.push_back(Point(Vector(p1) - edge));
		_rightBound.push_back(Point(Vector(p2) + edge));
	};

	_leftBound.clear();
	_rightBound.clear();

	addBound();

	bool isLastSeg = false;
	while (!isLastSeg)
	{
		Point p1(curX, behaviorFunc(curX));
		Point p2(curX + stepX, behaviorFunc(curX + stepX));

		if (segLenLimit < curSegLen + distance(p1, p2))
		{
			if (_length < curLen + curSegLen)
			{
				isLastSeg = true;
				curSegLen = _length - curLen;
				if (MIN_SEG_LEN < curSegLen)
				{
					double l = lastSegX;
					double r = curX;
					p1 = Point(lastSegX, behaviorFunc(lastSegX));
					while (!doubleEqual(l, r))
					{
						double mid = l + (r - l) / 2;
						p2 = Point(mid, behaviorFunc(mid));
						if (curLen + distance(p1, p2) < _length)
							l = mid;
						else
							r = mid;
					}
					curX = l;
				}
			}

			curLen += curSegLen;
			if (MIN_SEG_LEN < curSegLen)
				addBound();

			curSegLen = 0;
			lastSegX = curX;
		}

		curSegLen += distance(p1, p2);
		curX += stepX;
	}

	int boundLen = _leftBound.size();
	int verticesCount = boundLen * 2 - 1;
	_triangleStrip.resize(verticesCount);
	for (int i = 0; i < boundLen; i++)
	{
		_triangleStrip[2 * i].position = sf::Vector2f(_leftBound[i].x, _leftBound[i].y);
		_triangleStrip[2 * i].color = _color;
		if (i != boundLen - 1)
		{
			_triangleStrip[2 * i + 1].position = sf::Vector2f(_rightBound[i].x, _rightBound[i].y);
			_triangleStrip[2 * i + 1].color = _color;
		}
	}
}

void Tentacle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(_triangleStrip, states);
	target.draw(_body, states);
}

void Tentacle::updateBehFuncM()
{
	static const double BEHAVIOUR_SPEED = 1;

	static const double EXT_MIN_FACTOR = -0.3;
	static const double EXT_MAX_FACTOR = 0.3;
	static const double EXT_SPEED_FACTOR = 0.5 * BEHAVIOUR_SPEED;
	static const double INT_MIN_FACTOR = 0.12;
	static const double INT_MAX_FACTOR = 0.18;
	static const double INT_SPEED_FACTOR = 0.5 * BEHAVIOUR_SPEED;

	for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
	{
		double extFactorMinValue = _bodyThickness * EXT_MIN_FACTOR;
		double extFactorMaxValue = _bodyThickness * EXT_MAX_FACTOR;
		double extFactorFloatingSpeed = (extFactorMaxValue - extFactorMinValue) * EXT_SPEED_FACTOR;
		_behFuncM1[i].setLimits(extFactorMinValue, extFactorMaxValue);
		_behFuncM1[i].setFloatingSpeed(extFactorFloatingSpeed);
		double intFactorMinValue = _length * INT_MIN_FACTOR;
		double intFactorMaxValue = _length * INT_MAX_FACTOR;
		double intFactorFloatingSpeed = (intFactorMaxValue - intFactorMinValue) * INT_SPEED_FACTOR;
		_behFuncM2[i].setLimits(intFactorMinValue, intFactorMaxValue);
		_behFuncM2[i].setFloatingSpeed(intFactorFloatingSpeed);
	}
}

double Tentacle::thicknessFunc(double curLen)
{
	const double coefficient = _bodyThickness / _length;
	return _bodyThickness - coefficient * curLen;
}
double Tentacle::behaviorFunc(double curX)
{
	// Example: f(x) = 15 * (sin(3 * curX / 100) + sin(2 * curX / 100) + sin(curX / 100) - sin(2.5 * curX / 100))
	double result = 0;
	for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
		result += _behFuncM1[i].getValue() * sin(curX * M_PI_2 / _behFuncM2[i].getValue());
	return result;
}
