#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include <SFML/Graphics.hpp>
#include "Player.h"

void alignText(sf::Text& text, const sf::RectangleShape& button, float padding = 10.0f);



class HealthBar {
public:
	HealthBar(float x, float y, float size, int maxHealth, float scaleX, float scaleY);
	void update(int currentHealth);
	void render(sf::RenderWindow& window);

private:
	sf::Text healthText;
	sf::Font font;
	int maxHealth;
	float size;
	float x;
	float y;
	float scaleX;
	float scaleY;

};

class ShowStats {
public:
	ShowStats(const Player& player);
	void renderPlayerStats(sf::RenderWindow& window, const Player& player, int charSize, float scaleX, float scaleY);
private:
	sf::Text statsText;
	sf::Font font;
	const Player& player;
	float charSize;
	float x;
	float y;
	float scaleX;
	float scaleY;
};

#endif