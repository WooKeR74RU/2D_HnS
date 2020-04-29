#pragma once

#include <SFML/Graphics.hpp>
#include <set>

class EventInput
{
public:
	bool isKeyPressed(sf::Keyboard::Key key) const;
	bool isKeyReleased(sf::Keyboard::Key key) const;
	bool isMouseButtonPressed(sf::Mouse::Button button) const;
	bool isMouseButtonReleased(sf::Mouse::Button button) const;

	void handeEvent(sf::Event event);
	void resetData();
private:
	std::set<sf::Keyboard::Key> keyPressed;
	std::set<sf::Keyboard::Key> keyReleased;
	std::set<sf::Mouse::Button> mouseButtonPressed;
	std::set<sf::Mouse::Button> mouseButtonReleased;
};
