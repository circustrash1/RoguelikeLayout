#include "GuiElements.h"
#include <iostream>

void alignText(sf::Text& text, const sf::RectangleShape& button, float padding) {
    sf::FloatRect textRect = text.getLocalBounds();
    sf::FloatRect buttonRect = button.getGlobalBounds();
    text.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f);
    text.setPosition(buttonRect.left + padding, buttonRect.top + buttonRect.height / 2.0f);
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
