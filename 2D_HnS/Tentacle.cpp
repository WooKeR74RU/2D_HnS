#define _USE_MATH_DEFINES
#include "Tentacle.h"

#include <cmath>
#include "Vector.h"
#include "Geometry.h"
#include "Utilities.h"

Tentacle::Tentacle() : _length(450), _bodyThickness(40), _color(sf::Color::White), _behaviourSpeed(1)
{
	_triangleStrip.setPrimitiveType(sf::TriangleStrip);

	_body.setRadius(_bodyThickness / 2);
	_body.setPosition(-_bodyThickness / 2, -_bodyThickness / 2);
	_body.setFillColor(_color);

	//_outline.setPrimitiveType(sf::LineStrip);

	updateBehaviourExternalFactors();
	updateBehaviourInternalFactors();
	
	update(sf::Time::Zero);
}

void Tentacle::setLength(double length)
{
	_length = length;
	updateBehaviourInternalFactors();
}
void Tentacle::setBodyThickness(double bodyThickness)
{
	_bodyThickness = bodyThickness;
	_body.setRadius(_bodyThickness / 2);
	_body.setPosition(-_bodyThickness / 2, -_bodyThickness / 2);
	updateBehaviourExternalFactors();
}
void Tentacle::setColor(sf::Color color)
{
	_color = color;
	_body.setFillColor(_color);
}

void Tentacle::setBehaviourSpeed(double behaviourSpeed)
{
	_behaviourSpeed = behaviourSpeed;
	updateBehaviourExternalFactors();
	updateBehaviourInternalFactors();
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
		_behaviourExternalFactors[i].update(elapsed);
		_behaviourInternalFactors[i].update(elapsed);
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
		//int colorVal = 128 + 128 * (i + 1) / boundLen;
		//_triangleStrip[2 * i].color = sf::Color(colorVal, 0, 0);
		if (i != boundLen - 1)
		{
			_triangleStrip[2 * i + 1].position = sf::Vector2f(_rightBound[i].x, _rightBound[i].y);
			_triangleStrip[2 * i + 1].color = _color;
			//_triangleStrip[2 * i + 1].color = sf::Color(colorVal, 0, 0);
		}
	}

	//_outline.resize(verticesCount);
	//for (int i = 0; i < boundLen; i++)
	//	_outline[i].position = sf::Vector2f(_leftBound[i].x, _leftBound[i].y);
	//for (int i = 0; i < boundLen - 1; i++)
	//	_outline[boundLen + i].position = sf::Vector2f(_rightBound[boundLen - i - 1].x, _rightBound[boundLen - i - 1].y);
}

void Tentacle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(_triangleStrip, states);
	target.draw(_body, states);
}

//void Tentacle::drawOutline(sf::RenderTarget& target) const
//{
//	target.draw(_outline, getTransform());
//}

void Tentacle::updateBehaviourExternalFactors()
{
	static const double EXT_MIN_FACTOR = -0.3;
	static const double EXT_MAX_FACTOR = 0.3;
	double extSpeedFactor = 0.5 * _behaviourSpeed;
	for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
	{
		double extFactorMinValue = _bodyThickness * EXT_MIN_FACTOR;
		double extFactorMaxValue = _bodyThickness * EXT_MAX_FACTOR;
		double extFactorFloatingSpeed = (extFactorMaxValue - extFactorMinValue) * extSpeedFactor;
		_behaviourExternalFactors[i].setLimits(extFactorMinValue, extFactorMaxValue);
		_behaviourExternalFactors[i].setFloatingSpeed(extFactorFloatingSpeed);
	}
}

void Tentacle::updateBehaviourInternalFactors()
{
	static const double INT_MIN_FACTOR = 0.12;
	static const double INT_MAX_FACTOR = 0.18;
	double intSpeedFactor = 0.5 * _behaviourSpeed;
	for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
	{
		double intFactorMinValue = _length * INT_MIN_FACTOR;
		double intFactorMaxValue = _length * INT_MAX_FACTOR;
		double intFactorFloatingSpeed = (intFactorMaxValue - intFactorMinValue) * intSpeedFactor;
		_behaviourInternalFactors[i].setLimits(intFactorMinValue, intFactorMaxValue);
		_behaviourInternalFactors[i].setFloatingSpeed(intFactorFloatingSpeed);
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
		result += _behaviourExternalFactors[i].getValue() * sin(curX * M_PI_2 / _behaviourInternalFactors[i].getValue());
	return result;
}
