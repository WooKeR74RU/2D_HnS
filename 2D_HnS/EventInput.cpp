#include "EventInput.h"

bool EventInput::isKeyPressed(sf::Keyboard::Key key) const
{
	return keyPressed.count(key) == 1;
}
bool EventInput::isKeyReleased(sf::Keyboard::Key key) const
{
	return keyReleased.count(key) == 1;
}
bool EventInput::isMouseButtonPressed(sf::Mouse::Button button) const
{
	return mouseButtonPressed.count(button) == 1;
}
bool EventInput::isMouseButtonReleased(sf::Mouse::Button button) const
{
	return mouseButtonReleased.count(button) == 1;
}

void EventInput::handeEvent(sf::Event event)
{
	if (event.type == sf::Event::KeyPressed)
		keyPressed.insert(event.key.code);
	if (event.type == sf::Event::KeyReleased)
		keyReleased.insert(event.key.code);
	if (event.type == sf::Event::MouseButtonPressed)
		mouseButtonPressed.insert(event.mouseButton.button);
	if (event.type == sf::Event::MouseButtonReleased)
		mouseButtonReleased.insert(event.mouseButton.button);
}
void EventInput::resetData()
{
	keyPressed.clear();
	keyReleased.clear();
	mouseButtonPressed.clear();
	mouseButtonReleased.clear();
}
