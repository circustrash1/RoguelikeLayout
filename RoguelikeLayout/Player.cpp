// Player.cpp
#include "Player.h"
#include <iostream>

Player::Player(int x, int y, char symbol, int health, int attackDmg) : x(x), y(y), symbol(symbol), health(health), attackDmg(attackDmg) {}

int Player::getX() const {
    return x;
}

int Player::getY() const {
    return y;
}

int Player::getHealth() const {
    return health;
}

void Player::loseHealth(int amount) {
    health -= amount;
    if (health < 0) {
        health = 0;
    }
}

void Player::move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies) {
    int newX = x, newY = y;

    switch (direction) {
    case 'w': newY -= 1; break; // Move up
    case 's': newY += 1; break; // Move down
    case 'a': newX -= 1; break; // Move left
    case 'd': newX += 1; break; // Move right
    }

    // Check if the new position is walkable
    if (map[newY][newX] != '.') {
        return;
    }


    // Check for collision with enemies
    for (const Enemy* enemy : enemies) {
        if (enemy->getX() == newX && enemy->getY() == newY) {
            return; // Collision detected, do not move
        }
    }

    // Move the player to the new position
    x = newX;
    y = newY;
}

void Player::update(const std::vector<Enemy*>& enemies) {
    if (damageClock.getElapsedTime().asSeconds() >= 1.0f) {
        for (const Enemy* enemy : enemies) {
            int enemyX = enemy->getX();
            int enemyY = enemy->getY();
            if ((std::abs(enemyX - x) <= 1 && enemyY == y) || (std::abs(enemyY - y) <= 1 && enemyX == x)) {
				std::cout << "Player attacked by enemy!" << std::endl;
                loseHealth(10);
                break;
            }
        }
        damageClock.restart();
    }
}

void Player::render(sf::RenderWindow& window, int charSize) const {
    sf::Font font;
    if (!font.loadFromFile("fs-min.ttf")) {
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::White);
    text.setString(symbol);
    text.setPosition(x * charSize, y * charSize);
    window.draw(text);
}
