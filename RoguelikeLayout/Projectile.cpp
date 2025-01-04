#include "Projectile.h"
#include <cmath>

Projectile::Projectile(int startX, int startY, int targetX, int targetY, int damage, Player* player, sf::Color color)
    : x(startX), y(startY), targetX(targetX), targetY(targetY), damage(damage), player(player), color(color), active(true) {
    // Calculate the duration based on the distance to the target
    float distance = std::sqrt(std::pow(targetX - startX, 2) + std::pow(targetY - startY, 2));
    duration = distance * 0.05f; // Adjust the multiplier as needed to control the speed
    clock.restart();
}

void Projectile::update() {
    if (!active) return;

    float elapsed = clock.getElapsedTime().asSeconds();

    if (elapsed < duration) {
        float t = elapsed / duration;
        int currentX = static_cast<int>(x + t * (targetX - x));
        int currentY = static_cast<int>(y + t * (targetY - y));

        // Check for collision with the player if player reference is provided
        if (player && currentX == player->getX() && currentY == player->getY()) {
            player->loseHealth(damage);
            active = false;
        }
    }
    else {
        active = false;
    }
}

void Projectile::render(sf::RenderWindow& window, int charSize) {
    if (!active) return;

    float elapsed = clock.getElapsedTime().asSeconds();

    if (elapsed < duration) {
        float t = elapsed / duration;
        int currentX = static_cast<int>(x + t * (targetX - x));
        int currentY = static_cast<int>(y + t * (targetY - y));

        sf::Font font;
        if (!font.loadFromFile("fs-min.ttf")) {
            return;
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(charSize);
        text.setString('*'); // ASCII character for the projectile
        text.setFillColor(color); // Set the color of the projectile
        text.setPosition(currentX * charSize, currentY * charSize);
        window.draw(text);
    }
    else {
        active = false;
    }
}

bool Projectile::isActive() const {
    return active;
}


