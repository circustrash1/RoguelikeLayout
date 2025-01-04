#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <SFML/Graphics.hpp>
#include <string>

class TitleScreen {
public:
	TitleScreen(const sf::Font& font, float scaleX, float scaleY);
	void display(sf::RenderWindow& window) const;

private:
	sf::Text titleText;
	sf::Text controlsText;
};

#endif // TITLESCREEN_H