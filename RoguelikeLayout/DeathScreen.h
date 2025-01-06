#ifndef DEATHSCREEN_H
#define DEATHSCREEN_H

#include <SFML/Graphics.hpp>

class Game;

class DeathScreen {
public:
	DeathScreen(const sf::Font& font, float scaleX, float scaleY);
	void display(sf::RenderWindow& window, Game& game) const;

private:
	sf::Text deathText;
	sf::Text restartText;
};

#endif // DEATHSCREEN_H