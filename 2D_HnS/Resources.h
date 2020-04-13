#pragma once
#include <fstream>
#include <SFML/Graphics.hpp>

class Resources
{
	friend Resources& RS();
public:
	std::ifstream input;
	std::ofstream output;
	sf::RenderWindow window;

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;
private:
	Resources();
};

Resources& RS();
