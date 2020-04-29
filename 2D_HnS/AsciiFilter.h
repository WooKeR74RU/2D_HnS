#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

class AsciiFilter
{
public:
	AsciiFilter(int windowWidth, int windowHeight, const std::string& fontPath, int characterSize, bool isBoldOn, sf::Color backgroundColor = sf::Color::Black);

	void applyTo(sf::RenderWindow& window);

private:
	int getGlyphId(int brightness);

	int _windowWidth;
	int _windowHeight;
	sf::Font _font;
	int _characterSize;
	bool _isBoldOn;
	sf::Color _backgroundColor;

	int _charCount;
	int _glyphWidth;
	int _glyphHeight;
	int _matrixW;
	int _matrixH;
	std::vector<std::pair<int, int>> _glyphsBrightness;

	sf::Texture _windowContent;
	sf::RenderTexture _renderTexture;

	sf::Texture _customFontTexture;
	std::map<int, int> _glyphsOffset;
	sf::VertexArray _vertexArray;
};
