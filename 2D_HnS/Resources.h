#pragma once

#include <fstream>
#include <SFML/Graphics.hpp>
#include "EventInput.h"

class Resources
{
	friend Resources& RS();
public:
	std::ifstream input;
	std::ofstream output;
	sf::RenderWindow window;
	EventInput eventInput;

	Resources(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources& operator=(Resources&&) = delete;
private:
	Resources();
};

Resources& RS();
