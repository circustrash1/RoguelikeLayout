#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SFML/Graphics.hpp>
#include <string>

class TextBox {
public:
	TextBox(const std::string& text, const sf::Font& font, unsigned int charSize, float x, float y);
	void update(float deltaTime);
	void render(sf::RenderWindow& window);
	const std::string getFullText() const;
	bool isComplete() const;
	void skip();

	void clearAfter(float seconds);

private:
	std::string fullText;
	std::string displayedText;
	sf::Text text;
	float timePerLetter;
	float elapsedTime;
	bool complete;
	bool clearText;
	float clearTimer;
	float clearAfterSeconds;
};


#endif	// TEXTBOX_H