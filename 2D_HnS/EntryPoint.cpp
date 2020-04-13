#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include "Utilities.h"
#include "Geometry.h"
#include "Resources.h"

void drawDot(double x, double y)
{
	static const int DOT_RADIUS = 5;
	sf::CircleShape dot(DOT_RADIUS);
	dot.setFillColor(sf::Color::Red);
	dot.setPosition(x - DOT_RADIUS, y - DOT_RADIUS);
	RS().window.draw(dot);
}

class FloatingValue
{
public:
	FloatingValue(double min, double max, double floatingSpeed) : _min(min), _max(max), _floatingSpeed(floatingSpeed)
	{
		_dis = std::uniform_real_distribution<double>(min, max);
		_curValue = _dis(_gen);
		_curAimValue = _dis(_gen);
	}
	void update(sf::Time elapsed)
	{
		if (doubleLess(_curValue, _curAimValue))
		{
			_curValue += _floatingSpeed * elapsed.asSeconds();
			if (!doubleLess(_curValue, _curAimValue))
			{
				_curValue = _curAimValue;
				_curAimValue = _dis(_gen);
			}
		}
		else
		{
			_curValue -= _floatingSpeed * elapsed.asSeconds();
			if (doubleLess(_curValue, _curAimValue))
			{
				_curValue = _curAimValue;
				_curAimValue = _dis(_gen);
			}
		}
	}
	double getValue() const
	{
		return _curValue;
	}
private:
	double _min;
	double _max;
	double _floatingSpeed;

	double _curValue;
	double _curAimValue;
	std::uniform_real_distribution<double> _dis;

	static std::mt19937 _gen;
};
std::mt19937 FloatingValue::_gen(std::chrono::system_clock::now().time_since_epoch().count());

class Tentacle : public sf::Drawable, public sf::Transformable
{
public:
	Tentacle() : _length(450), _bodyThickness(40), _color(sf::Color::White), _isBorderDrawn(false)
	{
		for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
			_behFuncM1.push_back(FloatingValue(-20, 20, 30));
		for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
			_behFuncM2.push_back(FloatingValue(25, 35, 2));
		update(sf::Time::Zero);
	}

	void setLength(double length)
	{
		_length = length;
	}
	void setBodyThickness(double bodyThickness)
	{
		_bodyThickness = bodyThickness;
	}
	void setColor(sf::Color color)
	{
		_color = color;
	}
	void setIsBorderDrawn(bool isBorderDrawn)
	{
		_isBorderDrawn = isBorderDrawn;
	}

	void update(sf::Time elapsed)
	{
		static const int SEG_COUNT = 30;
		static const int SEG_STEP_COUNT = 3;
		static const double MIN_SEG_LEN_LIMIT = 5;
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

		const double segLenLimit = std::min(std::max(_length / SEG_COUNT, MIN_SEG_LEN_LIMIT), MAX_SEG_LEN_LIMIT);
		const double stepX = segLenLimit / SEG_STEP_COUNT;

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

		_segments.clear();
		_segments.resize(_leftBound.size() - 1);
		for (int i = 0; i < _leftBound.size() - 1; i++)
		{
			bool isTriangle = (i == _leftBound.size() - 2);
			_segments[i].setPointCount(isTriangle ? 3 : 4);
			_segments[i].setPoint(0, sf::Vector2f(_leftBound[i].x, _leftBound[i].y));
			_segments[i].setPoint(1, sf::Vector2f(_leftBound[i + 1].x, _leftBound[i + 1].y));
			if (!isTriangle)
				_segments[i].setPoint(2, sf::Vector2f(_rightBound[i + 1].x, _rightBound[i + 1].y));
			_segments[i].setPoint(isTriangle ? 2 : 3, sf::Vector2f(_rightBound[i].x, _rightBound[i].y));

			_segments[i].setFillColor(_color);
			if (_isBorderDrawn)
			{
				_segments[i].setOutlineColor(sf::Color::Red);
				_segments[i].setOutlineThickness(0.5);
			}
		}

		_body.setRadius(_bodyThickness / 2);
		_body.setPosition(-_bodyThickness / 2, -_bodyThickness / 2);
		//_body.setFillColor(_color);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		//target.draw(_body, states);
		for (int i = 0; i < _segments.size(); i++)
			target.draw(_segments[i], states);
		target.draw(_body, states);
	}

private:
	double thicknessFunc(double curLen)
	{
		const double coefficient = _bodyThickness / _length;
		return _bodyThickness - coefficient * curLen;
	}
	double behaviorFunc(double curX)
	{
		double result = 0;
		for (int i = 0; i < BEHAVIOR_COMPONENTS; i++)
			result += _behFuncM1[i].getValue() * sin(curX / _behFuncM2[i].getValue());
		return result;

		//curX /= 100;
		//return 15 * (sin(3 * curX) + sin(2 * curX) + sin(curX) - sin(2.5 * curX));
	}

	double _length;
	double _bodyThickness;
	sf::Color _color;
	bool _isBorderDrawn;

	static const int BEHAVIOR_COMPONENTS = 10;
	std::vector<FloatingValue> _behFuncM1;
	std::vector<FloatingValue> _behFuncM2;

	std::vector<Point> _leftBound;
	std::vector<Point> _rightBound;
	std::vector<sf::ConvexShape> _segments;
	sf::CircleShape _body;
};

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	static const int WINDOW_WIDTH = 750;
	static const int WINDOW_HEIGHT = 750;
	RS().window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D_HnS", sf::Style::Close, settings);
	//RS().window.setVerticalSyncEnabled(true);

	RS().output.precision(3);
	RS().output.setf(std::ios::fixed);

	sf::View view(RS().window.getView());
	view.move(-WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2);
	RS().window.setView(view);

	sf::Image icon128;
	if (!icon128.loadFromFile("Resources/icon128.png"))
		return 1;
	RS().window.setIcon(128, 128, icon128.getPixelsPtr());

	static const int MODE = 2;

	Tentacle tentacle;
	auto init1 = [&]() -> void
	{
		// ...
	};

	static const int N1 = 15;
	Tentacle tentacles1[N1];
	static const int N2 = 10;
	Tentacle tentacles2[N2];
	auto init2 = [&]() -> void
	{
		for (int i = 0; i < N1; i++)
		{
			for (int j = 0; j < i; j++)
				tentacles1[i].rotate(360 / N1);
		}

		for (int i = 0; i < N2; i++)
		{
			tentacles2[i].rotate(360 / N2 / 2);
			for (int j = 0; j < i; j++)
				tentacles2[i].rotate(360 / N2);
		}

		for (int i = 0; i < N1; i++)
		{
			//tentacles1[i].setLength(400);
			//tentacles1[i].setBodyThickness(60);
			tentacles1[i].setColor(sf::Color::Black);
		}
		for (int i = 0; i < N2; i++)
		{
			tentacles2[i].setLength(100);
			tentacles2[i].setBodyThickness(20);
			tentacles2[i].setColor(sf::Color(244, 195, 0));
		}
	};

	auto update1 = [&](sf::Time elapsed) -> void
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			{
				static double variable = 300;
				static double direction = 1;
				variable += direction * elapsed.asSeconds() * 150;
				if (variable > 600)
				{
					variable = 600;
					direction = -1;
				}
				if (variable < 300)
				{
					variable = 300;
					direction = 1;
				}
				tentacle.setLength(variable);
			}
		}
		tentacle.update(elapsed);
	};

	auto update2 = [&](sf::Time elapsed) -> void
	{
		for (int i = 0; i < N1; i++)
			tentacles1[i].update(elapsed);
		for (int i = 0; i < N2; i++)
			tentacles2[i].update(elapsed);
	};

	auto render1 = [&]() -> void
	{
		RS().window.draw(tentacle);
	};

	auto render2 = [&]() -> void
	{
		for (int i = 0; i < N1; i++)
			RS().window.draw(tentacles1[i]);
		//for (int i = 0; i < N2; i++)
		//	RS().window.draw(tentacles2[i]);
	};

	if (MODE == 1)
		init1();
	if (MODE == 2)
		init2();

	sf::Clock clock;

	while (RS().window.isOpen())
	{
		sf::Event event;
		while (RS().window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				RS().window.close();

			static const double moveSpeed = 25;
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::A ||
				event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::D))
			{
				sf::View view = RS().window.getView();
				if (event.key.code == sf::Keyboard::W)
					view.move(0, -moveSpeed);
				if (event.key.code == sf::Keyboard::A)
					view.move(-moveSpeed, 0);
				if (event.key.code == sf::Keyboard::S)
					view.move(0, moveSpeed);
				if (event.key.code == sf::Keyboard::D)
					view.move(moveSpeed, 0);
				RS().window.setView(view);
			}
		}

		sf::Time elapsed = clock.restart();

		if (MODE == 1)
			update1(elapsed);
		if (MODE == 2)
			update2(elapsed);

		RS().window.clear(sf::Color::White);

		if (MODE == 1)
			render1();
		if (MODE == 2)
			render2();

		RS().window.display();
	}

	return 0;
}
