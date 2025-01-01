#include "GuiElements.h"
#include <iostream>

void alignText(sf::Text& text, const sf::RectangleShape& button, float padding) {
    sf::FloatRect textRect = text.getLocalBounds();
    sf::FloatRect buttonRect = button.getGlobalBounds();
    text.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f);
    text.setPosition(buttonRect.left + padding, buttonRect.top + buttonRect.height / 2.0f);
}

void alignCards(sf::Text& name, sf::Text& art, sf::Text& positiveEffects, sf::Text& negativeEffects, float padding, float columnX) {
    sf::FloatRect nameRect = name.getLocalBounds();
    sf::FloatRect positiveRect = positiveEffects.getLocalBounds();
    sf::FloatRect negativeRect = negativeEffects.getLocalBounds();
    sf::FloatRect artRect = art.getLocalBounds();

    // Calculate center
    float centerX = (nameRect.width + positiveRect.width + artRect.width) / 2.0f;
	float centerY = (nameRect.height + positiveRect.height + artRect.height) / 2.0f;

    // Align name
    name.setOrigin(nameRect.left + nameRect.width / 2.0f, nameRect.top + nameRect.height / 2.0f);
    name.setPosition(columnX + 162, centerY - (positiveRect.height + artRect.height + padding) / 2.0f + 125);

    // Align art
    art.setOrigin(artRect.left + artRect.width / 2.0f, artRect.top + artRect.height / 2.0f);
    art.setPosition(columnX + 162, centerY +(nameRect.height + positiveRect.height + padding) / 2.0f + 125);

    // Align positive effects
    positiveEffects.setOrigin(positiveRect.left + positiveRect.width / 2.0f, positiveRect.top + positiveRect.height / 2.0f);
    positiveEffects.setPosition(columnX + 162, centerY + 350);

    // Align negative effects
    negativeEffects.setOrigin(negativeRect.left + negativeRect.width / 2.0f, negativeRect.top + negativeRect.height / 2.0f);
    negativeEffects.setPosition(columnX + 162, positiveEffects.getPosition().y + positiveRect.height / 2.0f + padding + negativeRect.height / 2.0f);


}

HealthBar::HealthBar(float x, float y, float size, int maxHealth, float scaleX, float scaleY)
    : x(x), y(y), size(size), maxHealth(maxHealth), scaleX(scaleX), scaleY(scaleY) {
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

void HealthBar::update(int currentHealth) {
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

void ShowStats::renderPlayerStats(sf::RenderWindow& window, const Player& player, int charSize, float scaleX, float scaleY) {
    const Stat& stats = player.getStats();
    std::string statsString = "STR: " + std::to_string(stats.strength) + "\n" +
        "DEX: " + std::to_string(stats.dexterity) + "\n" +
        "CON: " + std::to_string(stats.constitution) + "\n" +
        "INT: " + std::to_string(stats.intelligence) + "\n" +
        "WIS: " + std::to_string(stats.wisdom) + "\n" +
        "CHA: " + std::to_string(stats.charisma);
    statsText.setString(statsString);
    statsText.setCharacterSize(charSize);
    statsText.setPosition(500 * scaleX, 280 * scaleY);
    window.draw(statsText);
}
