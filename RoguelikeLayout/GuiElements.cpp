#include "GuiElements.h"
#include "EventMaps.h"
#include "SharedData.h"
#include <iostream>
#include <sstream>

extern SharedData sharedData;

void alignText(sf::Text& text, const sf::RectangleShape& button, float padding) {
	sf::FloatRect textRect = text.getLocalBounds();
	sf::FloatRect buttonRect = button.getGlobalBounds();
	text.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f);
	text.setPosition(buttonRect.left + padding, buttonRect.top + buttonRect.height / 2.0f);
}

void alignCards(sf::Text& name, sf::Text& art, sf::Text& positiveEffects, sf::Text& negativeEffects, sf::Text& cardCost, float padding, float columnX) {
	sf::FloatRect nameRect = name.getLocalBounds();
	sf::FloatRect positiveRect = positiveEffects.getLocalBounds();
	sf::FloatRect negativeRect = negativeEffects.getLocalBounds();
	sf::FloatRect artRect = art.getLocalBounds();
	sf::FloatRect costRect = cardCost.getLocalBounds();

	// Calculate center
	float centerX = (nameRect.width + positiveRect.width + artRect.width) / 2.0f;
	float centerY = (nameRect.height + positiveRect.height + artRect.height) / 2.0f;

	// Align card cost just above the card name
	cardCost.setOrigin(costRect.left + costRect.width / 2.0f, costRect.top + costRect.height / 2.0f);
	cardCost.setPosition(columnX + 162, centerY - (positiveRect.height + artRect.height + padding) / 2.0f + 125 - costRect.height / 2.0f - padding);

	// Align name
	name.setOrigin(nameRect.left + nameRect.width / 2.0f, nameRect.top + nameRect.height / 2.0f);
	name.setPosition(columnX + 162, cardCost.getPosition().y + costRect.height / 2.0f + padding + nameRect.height / 2.0f);

	// Align art
	art.setOrigin(artRect.left + artRect.width / 2.0f, artRect.top + artRect.height / 2.0f);
	art.setPosition(columnX + 162, name.getPosition().y + nameRect.height / 2.0f + padding + artRect.height / 2.0f);

	// Align positive effects
	positiveEffects.setOrigin(positiveRect.left + positiveRect.width / 2.0f, positiveRect.top + positiveRect.height / 2.0f);
	positiveEffects.setPosition(columnX + 162, art.getPosition().y + artRect.height / 2.0f + padding + positiveRect.height / 2.0f);

	// Align negative effects
	negativeEffects.setOrigin(negativeRect.left + negativeRect.width / 2.0f, negativeRect.top + negativeRect.height / 2.0f);
	negativeEffects.setPosition(columnX + 162, positiveEffects.getPosition().y + positiveRect.height / 2.0f + padding + negativeRect.height / 2.0f);
}

std::string wrapText(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth) {
	std::string wrappedText;
	std::istringstream words(text);
	std::string word;
	sf::Text tempText;
	tempText.setFont(font);
	tempText.setCharacterSize(characterSize);

	float spaceWidth = tempText.getLocalBounds().width;
	float lineWidth = 0;

	while (words >> word) {
		tempText.setString(word);
		float wordWidth = tempText.getLocalBounds().width;

		if (lineWidth + wordWidth + spaceWidth > maxWidth * 1.5f) { // Allow text to be a bit wider before wrapping
			wrappedText += "\n";
			lineWidth = 0;
		}
		else if (!wrappedText.empty()) {
			wrappedText += " ";
			lineWidth += spaceWidth;
		}

		wrappedText += word;
		lineWidth += wordWidth;
	}

	return wrappedText;
}

HealthBar::HealthBar(float x, float y, float size, int maxHealth, float scaleX, float scaleY)
	: x(x), y(y), size(size), scaleX(scaleX), scaleY(scaleY) {
	if (!font.loadFromFile("fs-min.ttf")) {
		std::cerr << "Error loading font!" << std::endl;
		exit(-1);
	}
	healthText.setFont(font);
	healthText.setCharacterSize(size * scaleY);
	healthText.setFillColor(sf::Color::Red);
	healthText.setPosition(x * scaleX, y * scaleY);

	std::cout << "HealthBar Position: (" << x * scaleX << ", " << y * scaleY << ")" << std::endl;
}

void HealthBar::update(int currentHealth, int maxHealth) {
	int barLength = (currentHealth * 10) / maxHealth;
	if (currentHealth > 0) {
		barLength = std::max(1, barLength);
	}
	std::string healthBar = "[";
	for (int i = 0; i < 10; ++i) {
		if (i < barLength) {
			healthBar += "#";
		}
		else {
			healthBar += "_";
		}
	}
	healthBar += "]";
	healthText.setString(healthBar);
}

void HealthBar::render(sf::RenderWindow& window) {
	window.draw(healthText);
}

ShowStats::ShowStats(const Player& player) : player(player) {
	if (!font.loadFromFile("fs-min.ttf")) {
		std::cerr << "Error loading font!" << std::endl;
		exit(-1);
	}

	statsText.setFont(font);

	statsText.setFillColor(sf::Color::White);
}

void ShowStats::renderPlayerStats(sf::RenderWindow& window, const Player& player, int charSize, float scaleX, float scaleY, const Room* eventRoom, const std::string& relevantStat) {
	const Stat& stats = player.getStats();
	float x = 500 * scaleX;
	float y = 280 * scaleY;

	// Calculate the maximum width of the stat names
	float maxStatNameWidth = 0;
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("STR:", charSize));
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("DEX:", charSize));
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("CON:", charSize));
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("INT:", charSize));
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("WIS:", charSize));
	maxStatNameWidth = std::max(maxStatNameWidth, getTextWidth("CHA:", charSize));

	renderStat(window, "STR", stats.strength, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
	y += charSize;
	renderStat(window, "DEX", stats.dexterity, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
	y += charSize;
	renderStat(window, "CON", stats.constitution, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
	y += charSize;
	renderStat(window, "INT", stats.intelligence, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
	y += charSize;
	renderStat(window, "WIS", stats.wisdom, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
	y += charSize;
	renderStat(window, "CHA", stats.charisma, x, y, charSize, sharedData.relevantStat, maxStatNameWidth);
}

void ShowStats::renderStat(sf::RenderWindow& window, const std::string& statName, int statValue, float x, float y, int charSize, const std::string& relevantStat, float maxStatNameWidth) {
	sf::Text statText;
	statText.setFont(font);
	statText.setCharacterSize(charSize);
	statText.setFillColor(sf::Color::White);

	sf::Text valueText;
	valueText.setFont(font);
	valueText.setCharacterSize(charSize);
	valueText.setFillColor(sf::Color::White);

	if (statName == relevantStat) {
		statText.setFillColor(sf::Color::Green);
		valueText.setFillColor(sf::Color::Green);
	}

	statText.setString(statName + ":");
	statText.setPosition(x, y);
	window.draw(statText);

	valueText.setString(std::to_string(statValue));
	valueText.setPosition(x + maxStatNameWidth + 15, y); // Align numbers vertically
	window.draw(valueText);
}

float ShowStats::getTextWidth(const std::string& text, int charSize) {
	sf::Text tempText;
	tempText.setFont(font);
	tempText.setCharacterSize(charSize);
	tempText.setString(text);
	return tempText.getLocalBounds().width;
}

ShowGold::ShowGold() {
	if (!font.loadFromFile("fs-min.ttf")) {
		std::cerr << "Error loading font!" << std::endl;
		exit(-1);
	}
}

void ShowGold::renderGold(sf::RenderWindow& window, int gold, float x, float y, int charSize, float scaleX, float scaleY) {
	sf::Text goldText;
	goldText.setFont(font);
	goldText.setCharacterSize(charSize);
	goldText.setFillColor(sf::Color::Yellow);
	goldText.setString("Gold : " + std::to_string(gold));
	goldText.setPosition(x * scaleX, y * scaleY);
	window.draw(goldText);
}

ShowStage::ShowStage() {
	if (!font.loadFromFile("fs-min.ttf")) {
		std::cerr << "Error loading font!" << std::endl;
		exit(-1);
	}
}

void ShowStage::renderStage(sf::RenderWindow& window, int stage, float x, float y, int charSize, float scaleX, float scaleY) {
	sf::Text stagetext;
	stageText.setFont(font);
	stageText.setCharacterSize(charSize);
	stageText.setFillColor(sf::Color::White);
	stageText.setString("Stage: " + std::to_string(stage));
	stageText.setPosition(x * scaleX, y * scaleY);
	window.draw(stageText);
}