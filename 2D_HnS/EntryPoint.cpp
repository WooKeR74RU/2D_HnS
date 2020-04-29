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
#include "Animation.h"
#include "PlayerUnit.h"
#include "AsciiFilter.h"

int main()
{
	RS().output.precision(4);
	RS().output.setf(std::ios::fixed);

	static const int WINDOW_WIDTH = 1280;
	static const int WINDOW_HEIGHT = 720;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	RS().window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D_HnS", sf::Style::Close, settings);
	//RS().window.setVerticalSyncEnabled(true);

	sf::View view(RS().window.getView());
	view.move(-WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2);
	RS().window.setView(view);

	sf::Image icon128;
	if (!icon128.loadFromFile("Resources/icon128.png"))
		std::exit(EXIT_FAILURE);
	RS().window.setIcon(128, 128, icon128.getPixelsPtr());

	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("Resources/background.png"))
		std::exit(EXIT_FAILURE);
	backgroundTexture.setRepeated(true);
	int symbolWidth = 200;
	int symbolHeight = 200;
	int symbolsW = 2 * WINDOW_WIDTH / symbolWidth;
	int symbolsH = 2 * WINDOW_HEIGHT / symbolHeight;
	sf::Sprite backgroundSprite(backgroundTexture, sf::IntRect(0, 0, symbolsW * symbolWidth, symbolsH * symbolHeight));
	//backgroundSprite.setColor(sf::Color(255, 255, 255, 128));
	backgroundSprite.move(-WINDOW_WIDTH, -WINDOW_HEIGHT);

	PlayerUnit playerUnit;

	//static const int N = 10;
	//Tentacle tentacles[N];
	//static const double BODY_THICKNESS = 50;
	//for (int i = 0; i < N; i++)
	//{
	//	tentacles[i].setRotation(i * 360.0 / N);
	//	tentacles[i].setBodyThickness(BODY_THICKNESS);
	//}
	//sf::CircleShape body(BODY_THICKNESS / 2);
	//body.setPosition(-BODY_THICKNESS / 2, -BODY_THICKNESS / 2);
	//body.setFillColor(sf::Color::Black);

	AsciiFilter asciiFilter(WINDOW_WIDTH, WINDOW_HEIGHT, "Resources/consolas.ttf", 12, true);

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

		//for (int i = 0; i < N; i++)
		//	tentacles[i].update(elapsed);
		playerUnit.update(elapsed);

		RS().window.clear();

		//for (int i = 0; i < N; i++)
		//	RS().window.draw(tentacles[i]);
		//RS().window.draw(body);

		RS().window.draw(backgroundSprite);
		RS().window.draw(playerUnit);

		asciiFilter.applyTo(RS().window);

		RS().window.display();
	}

	return 0;
}
