#include "AsciiFilter.h"

#include <set>

AsciiFilter::AsciiFilter(int windowWidth, int windowHeight, const std::string& fontPath, int characterSize, sf::Color glyphsColor, Mode mode, bool isBoldOn, sf::Color backgroundColor)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	if (!_font.loadFromFile(fontPath))
		std::exit(EXIT_FAILURE);
	_glyphSize = characterSize;
	_glyphsColor = glyphsColor;
	_mode = mode;
	_isBoldOn = isBoldOn;
	_backgroundColor = backgroundColor;

	static const int ASCII_L = 32;
	static const int ASCII_R = 126;

	int charCount = ASCII_R - ASCII_L + 1;
	if (_isBoldOn)
		charCount *= 2;

	auto getGlyphParamertesById = [](int id) -> std::pair<int, bool>
	{
		if (id < ASCII_R - ASCII_L + 1)
			return std::make_pair(id + ASCII_L, false);
		return std::make_pair(id - (ASCII_R - ASCII_L + 1) + ASCII_L, true);
	};

	for (int i = 0; i < charCount; i++)
		_font.getGlyph(getGlyphParamertesById(i).first, _glyphSize, getGlyphParamertesById(i).second);
	sf::Texture fontTexture(_font.getTexture(_glyphSize));
	sf::Image glyphsImage(fontTexture.copyToImage());

	auto getGlyphBrightnessById = [&](int id) -> int
	{
		int glyphBrightness = 0;
		std::pair<int, bool> glyphParamertes(getGlyphParamertesById(id));
		sf::Glyph glyph(_font.getGlyph(glyphParamertes.first, _glyphSize, glyphParamertes.second));
		for (int i = 0; i < glyph.textureRect.height; i++)
		{
			for (int j = 0; j < glyph.textureRect.width; j++)
				glyphBrightness += glyphsImage.getPixel(glyph.textureRect.left + j, glyph.textureRect.top + i).a;
		}
		return glyphBrightness;
	};

	static const int FORBIDDEN_GLYPHS_COUNT = 1;
	static const int FORBIDDEN_GLYPHS[FORBIDDEN_GLYPHS_COUNT] = { 'm' };
	std::set<int> glyphsId;
	for (int i = 0; i < charCount; i++)
	{
		bool isForbidden = false;
		int glyphCode = getGlyphParamertesById(i).first;
		for (int j = 0; j < FORBIDDEN_GLYPHS_COUNT; j++)
		{
			if (glyphCode == FORBIDDEN_GLYPHS[j])
				isForbidden = true;
		}
		if (!isForbidden)
			glyphsId.insert(i);
	}

	[&]() -> void // filter glyphsId by brightness
	{
		std::vector<std::pair<int, int>> glyphsBrightness;
		for (int id : glyphsId)
			glyphsBrightness.push_back(std::make_pair(getGlyphBrightnessById(id), id));

		int minSymbolBrightness = INT_MAX;
		int maxSymbolBrightness = 0;
		for (int i = 0; i < glyphsBrightness.size(); i++)
		{
			minSymbolBrightness = std::min(minSymbolBrightness, glyphsBrightness[i].first);
			maxSymbolBrightness = std::max(maxSymbolBrightness, glyphsBrightness[i].first);
		}
		int symbolBrightnessRange = maxSymbolBrightness - minSymbolBrightness;
		for (int i = 0; i < glyphsBrightness.size(); i++)
			glyphsBrightness[i].first = (double)(glyphsBrightness[i].first - minSymbolBrightness) / symbolBrightnessRange * 255;

		static const int PRIORITY_SIZE = 94;
		static const int PRIORITY_ORDER[PRIORITY_SIZE] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
			'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', '.', '-', '_', '\'', '"', '`', '|', '/', '\\', '#', '^', '<', '>', '=', '+', '*', ':', '~',
			'!', '?', '$', '@', '&', '%', ',', ';', '(', ')', '[', ']', '{', '}' };
		std::map<int, int> priority;
		for (int i = 0; i < PRIORITY_SIZE; i++)
			priority[PRIORITY_ORDER[i]] = i;

		auto compareByPriority = [&](int symbolIdA, int symbolIdB) -> bool
		{
			int codeA = getGlyphParamertesById(symbolIdA).first;
			int codeB = getGlyphParamertesById(symbolIdB).first;
			return priority[codeA] < priority[codeB];
		};

		auto cmp = [&](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool
		{
			if (a.first == b.first)
				return compareByPriority(a.second, b.second);
			return a.first < b.first;
		};

		std::sort(glyphsBrightness.begin(), glyphsBrightness.end(), cmp);

		auto isEqual = [](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool
		{
			return a.first == b.first;
		};

		auto last = std::unique(glyphsBrightness.begin(), glyphsBrightness.end(), isEqual);
		glyphsBrightness.resize(last - glyphsBrightness.begin());

		static const double ALLOWANCE = 3;
		int maxInterval = 255.0 / (glyphsBrightness.size() - 1) * ALLOWANCE;

		static const double ALLOWABLE_REDUNDANCY = 0.05;
		int maxRedundantCount = ALLOWABLE_REDUNDANCY * glyphsBrightness.size();

		int redundantLeft = 0;
		for (int i = 0; i < maxRedundantCount; i++)
		{
			int curInterval = glyphsBrightness[i + 1].first - glyphsBrightness[i].first;
			if (maxInterval < curInterval)
				redundantLeft = i + 1;
		}

		int redundantRight = 0;
		for (int i = 0; i < maxRedundantCount; i++)
		{
			int indexL = glyphsBrightness.size() - i - 2;
			int indexR = glyphsBrightness.size() - i - 1;
			int curInterval = glyphsBrightness[indexR].first - glyphsBrightness[indexL].first;
			if (maxInterval < curInterval)
				redundantRight = i + 1;
		}

		glyphsId.clear();
		for (int i = redundantLeft; i < glyphsBrightness.size() - redundantRight; i++)
			glyphsId.insert(glyphsBrightness[i].second);
	}();

	[&]() -> void // filter glyphsId by width and height, find _glyphWidth and _glyphWidth
	{
		std::map<int, std::vector<int>, std::greater<int>> counterWidth;
		std::map<int, std::vector<int>, std::greater<int>> counterHeight;
		for (int id : glyphsId)
		{
			std::pair<int, bool> glyphParamertes(getGlyphParamertesById(id));
			sf::Glyph glyph(_font.getGlyph(glyphParamertes.first, _glyphSize, glyphParamertes.second));
			counterWidth[glyph.textureRect.width].push_back(id);
			counterHeight[glyph.textureRect.height].push_back(id);
		}

		static const double ALLOWABLE_REDUNDANCY = 0.2;
		int maxRedundantCount = ALLOWABLE_REDUNDANCY * glyphsId.size();

		int totalCountW = 0;
		auto itW = counterWidth.begin();
		_glyphWidth = itW->first;
		while (totalCountW + itW->second.size() < maxRedundantCount)
		{
			auto left = itW;
			auto right = ++itW;
			for (int id : left->second)
				glyphsId.erase(id);
			totalCountW += left->second.size();
			_glyphWidth = right->first + 1;
		}

		int totalCountH = 0;
		auto itH = counterHeight.begin();
		_glyphHeight = itH->first;
		while (totalCountH + itH->second.size() < maxRedundantCount)
		{
			auto left = itH;
			auto right = ++itH;
			for (int id : left->second)
				glyphsId.erase(id);
			totalCountH += left->second.size();
			_glyphHeight = right->first + 1;
		}

		static const int COLUMNS_INDENT = 0;
		static const int LINES_INDENT = 2;
		_glyphWidth += COLUMNS_INDENT;
		_glyphHeight += LINES_INDENT;
	}();

	_matrixW = _windowWidth / _glyphWidth;
	_matrixH = _windowHeight / _glyphHeight;

	[&]() -> void // fill _glyphsBrightness
	{
		for (int id : glyphsId)
			_glyphsBrightness.push_back(std::make_pair(getGlyphBrightnessById(id), id));
		int minSymbolBrightness = INT_MAX;
		int maxSymbolBrightness = 0;
		for (int i = 0; i < _glyphsBrightness.size(); i++)
		{
			minSymbolBrightness = std::min(minSymbolBrightness, _glyphsBrightness[i].first);
			maxSymbolBrightness = std::max(maxSymbolBrightness, _glyphsBrightness[i].first);
		}
		int symbolBrightnessRange = maxSymbolBrightness - minSymbolBrightness;
		for (int i = 0; i < _glyphsBrightness.size(); i++)
			_glyphsBrightness[i].first = (double)(_glyphsBrightness[i].first - minSymbolBrightness) / symbolBrightnessRange * 255;

		std::sort(_glyphsBrightness.begin(), _glyphsBrightness.end());
	}();

	[&]() -> void // create _customFontTexture, calculate _glyphsOffset
	{
		sf::RenderTexture fontRenderTexture;
		fontRenderTexture.create(glyphsId.size() * _glyphWidth, _glyphHeight);
		fontRenderTexture.clear(sf::Color(255, 255, 255, 0));
		int offsetIndex = 0;
		for (int id : glyphsId)
		{
			std::pair<int, bool> glyphParamertes(getGlyphParamertesById(id));
			sf::Glyph glyph(_font.getGlyph(glyphParamertes.first, _glyphSize, glyphParamertes.second));

			sf::Sprite sprite(fontTexture, glyph.textureRect);
			int spriteX = offsetIndex * _glyphWidth + (_glyphWidth - glyph.textureRect.width) / 2;
			int spriteY = (_glyphHeight - glyph.textureRect.height) / 2;
			sprite.setPosition(spriteX, spriteY);
			_glyphsOffset[id] = offsetIndex++;

			fontRenderTexture.draw(sprite);
		}
		fontRenderTexture.display();

		sf::Image customFontImage(fontRenderTexture.getTexture().copyToImage());
		for (int i = 0; i < customFontImage.getSize().y; i++)
		{
			for (int j = 0; j < customFontImage.getSize().x; j++)
			{
				sf::Color pixel(_backgroundColor);
				pixel.a = 255 - customFontImage.getPixel(j, i).a;
				customFontImage.setPixel(j, i, pixel);
			}
		}
		_customFontTexture.loadFromImage(customFontImage);
	}();

	_vertexArray.setPrimitiveType(sf::Quads);
	_vertexArray.resize(_matrixW * _matrixH * 4);
	for (int i = 0; i < _matrixH; i++)
	{
		for (int j = 0; j < _matrixW; j++)
		{
			int curQuad = (i * _matrixW + j) * 4;
			_vertexArray[curQuad].position = sf::Vector2f(j * _glyphWidth, i * _glyphHeight);
			_vertexArray[curQuad + 1].position = sf::Vector2f((j + 1) * _glyphWidth, i * _glyphHeight);
			_vertexArray[curQuad + 2].position = sf::Vector2f((j + 1) * _glyphWidth, (i + 1) * _glyphHeight);
			_vertexArray[curQuad + 3].position = sf::Vector2f(j * _glyphWidth, (i + 1) * _glyphHeight);
		}
	}

	_windowContent.create(_windowWidth, _windowHeight);
	_renderTexture.create(_matrixW, _matrixH);
}

void AsciiFilter::setMode(Mode mode)
{
	_mode = mode;
}

void AsciiFilter::applyTo(sf::RenderWindow& window)
{
	if (_mode == Off)
		return;

	_windowContent.update(window);
	sf::Sprite contentSprite(_windowContent);
	contentSprite.setScale((double)_matrixW / _windowWidth, (double)_matrixH / _windowHeight);

	_renderTexture.clear();
	_renderTexture.draw(contentSprite);
	_renderTexture.display();

	sf::Image rescaledImage(_renderTexture.getTexture().copyToImage());
	for (int i = 0; i < rescaledImage.getSize().y; i++)
	{
		for (int j = 0; j < rescaledImage.getSize().x; j++)
		{
			sf::Color pixel(rescaledImage.getPixel(j, i));
			int brightness = 0;
			brightness = std::max((int)pixel.r, brightness);
			brightness = std::max((int)pixel.g, brightness);
			brightness = std::max((int)pixel.b, brightness);
			int glyphId = getGlyphId(brightness);
			int curQuad = (i * rescaledImage.getSize().x + j) * 4;
			_vertexArray[curQuad].texCoords = sf::Vector2f(_glyphsOffset[glyphId] * _glyphWidth, 0);
			_vertexArray[curQuad + 1].texCoords = sf::Vector2f((_glyphsOffset[glyphId] + 1) * _glyphWidth, 0);
			_vertexArray[curQuad + 2].texCoords = sf::Vector2f((_glyphsOffset[glyphId] + 1) * _glyphWidth, _glyphHeight);
			_vertexArray[curQuad + 3].texCoords = sf::Vector2f(_glyphsOffset[glyphId] * _glyphWidth, _glyphHeight);
		}
	}

	sf::View view = window.getView();
	window.setView(window.getDefaultView());

	if (_mode == GlyphsOnly || _mode == Adaptive)
		window.clear(_glyphsColor);

	window.draw(_vertexArray, sf::RenderStates(&_customFontTexture));

	if (_mode == Adaptive)
	{
		contentSprite.setScale(1, 1);
		contentSprite.setColor(sf::Color(255, 255, 255, 96));
		window.draw(contentSprite);
	}

	window.setView(view);
}

int AsciiFilter::getGlyphId(int brightness)
{
	auto it = std::lower_bound(_glyphsBrightness.begin(), _glyphsBrightness.end(), std::make_pair(brightness, 0));
	if (it == _glyphsBrightness.end())
		it--;
	int index = it - _glyphsBrightness.begin();
	if (index == 0)
		index++;
	int dist1 = _glyphsBrightness[index].first - brightness;
	int dist2 = brightness - _glyphsBrightness[index - 1].first;
	index = dist1 < dist2 ? index : index - 1;
	return _glyphsBrightness[index].second;
}
