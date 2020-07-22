#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Geometry.h"
#include "Resources.h"
#include "FloatingValue.h"
#include "Tentacle.h"
#include "PlayerUnit.h"
#include "AsciiFilter.h"

int main()
{
	RS().output.precision(4);
	RS().output.setf(std::ios::fixed);

	static const int WINDOW_WIDTH = 1920;
	static const int WINDOW_HEIGHT = 1080;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	RS().window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D_HnS", sf::Style::None, settings);
	//RS().window.setVerticalSyncEnabled(true);

	sf::View view(RS().window.getView());
	view.setCenter(0, 0);
	RS().window.setView(view);

	//sf::Texture backgroundTexture;
	//if (!backgroundTexture.loadFromFile("Resources/background.png"))
	//	std::exit(EXIT_FAILURE);
	//backgroundTexture.setRepeated(true);
	//static const int SYMBOL_WIDTH = 200;
	//static const int SYMBOL_HEIGHT = 200;
	//int symbolsW = 2 * WINDOW_WIDTH / SYMBOL_WIDTH;
	//int symbolsH = 2 * WINDOW_HEIGHT / SYMBOL_HEIGHT;
	//sf::Sprite backgroundSprite(backgroundTexture, sf::IntRect(0, 0, symbolsW * SYMBOL_WIDTH, symbolsH * SYMBOL_HEIGHT));
	//backgroundSprite.setColor(sf::Color(255, 255, 255, 128));
	//backgroundSprite.move(-WINDOW_WIDTH, -WINDOW_HEIGHT);

	//static const int N = 10;
	//Tentacle tentacles[N];
	//for (int i = 0; i < N; i++)
	//	tentacles[i].setRotation(i * 360.0 / N);
	//FloatingValue lengthValue(300, 450, 100);
	//FloatingValue bodyValue(40, 60, 30);
	//EquilateralPolygon body(60 / 2, 4);
	//body.setFillColor(sf::Color::Black);

	static const int N = 100;
	Tentacle tentacles[N];
	for (int i = 0; i < N; i++)
	{
		tentacles[i].setBehaviourSpeed(0.5);
		tentacles[i].setLength(750);
		tentacles[i].setBodyThickness(75);
	}

	PlayerUnit playerUnit(65, 4);
	//playerUnit.setPosition(-WINDOW_WIDTH / 3, 0);

	AsciiFilter asciiFilter(WINDOW_WIDTH, WINDOW_HEIGHT, "Resources/consolas.ttf", 12, sf::Color::Green, AsciiFilter::Off);

	sf::Clock clock;

	while (RS().window.isOpen())
	{
		RS().eventInput.resetData();
		sf::Event event;
		while (RS().window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				RS().window.close();
			RS().eventInput.handeEvent(event);
		}

		sf::Time elapsed = clock.restart();

		for (int i = 0; i < N; i++)
			tentacles[i].update(elapsed);

		playerUnit.update(elapsed);

		if (RS().eventInput.isKeyPressed(sf::Keyboard::Num1))
			asciiFilter.setMode(AsciiFilter::Standard);
		if (RS().eventInput.isKeyPressed(sf::Keyboard::Num2))
			asciiFilter.setMode(AsciiFilter::GlyphsOnly);
		if (RS().eventInput.isKeyPressed(sf::Keyboard::Num3))
			asciiFilter.setMode(AsciiFilter::Adaptive);
		if (RS().eventInput.isKeyPressed(sf::Keyboard::Num4))
			asciiFilter.setMode(AsciiFilter::Off);

		RS().window.clear();

		//for (int i = 0; i < N; i++)
		//	RS().window.draw(tentacles[i]);

		for (int i = 0; i < N; i++)
		{
			float angle = i * 2 * M_PI / N - M_PI / 2;
			float x = std::cos(angle) * 1200;
			float y = std::sin(angle) * 900;
			tentacles[i % 3].setPosition(x, y);
			tentacles[i % 3].setRotation(90 + i * 360.0 / N);
			RS().window.draw(tentacles[i % 3]);
		}

		RS().window.draw(playerUnit);

		asciiFilter.applyTo(RS().window);

		RS().window.display();
	}

	return 0;
}
