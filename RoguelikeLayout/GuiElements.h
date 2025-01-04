#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include <SFML/Graphics.hpp>
#include "Player.h"

void alignText(sf::Text& text, const sf::RectangleShape& button, float padding = 10.0f);

void alignCards(sf::Text& name, sf::Text& art, sf::Text& positiveEffects, sf::Text& negativeEffects, sf::Text& cardCost, float padding, float columnX);

std::string wrapText(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth);

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
	void renderPlayerStats(sf::RenderWindow& window, const Player& player, int charSize, float scaleX, float scaleY, const Room* eventRoom, const std::string& relevantStat);
	void renderStat(sf::RenderWindow& window, const std::string& statName, int statValue, float x, float y, int charSize, const std::string& relevantStat, float maxStatNameWidth);
	float getTextWidth(const std::string& text, int charSize);

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

class ShowGold {
public:
	ShowGold();
	void renderGold(sf::RenderWindow& window, int gold, float x, float y, int charSize, float scaleX, float scaleY);
	
private:
	sf::Text goldText;
	sf::Font font;
};

#endif