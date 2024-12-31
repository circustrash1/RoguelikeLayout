#ifndef ENEMIES_H
#define ENEMIES_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Player.h"

// Forward declaration of Player class
class Player;

class Enemy {
public:
    Enemy(int x, int y, char symbol, int enemyHealth); // Updated constructor declaration
    virtual ~Enemy() = default;

    virtual void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) = 0;
    virtual void attack() = 0;
    void render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies);
    void takeDamage(int damage);
    void displayDamage(sf::RenderWindow& window, int charSize);
    bool isAlive() const;
    virtual void playDeathAnimation(sf::RenderWindow& window) = 0;

    int getX() const;
    int getY() const;
    char getSymbol() const;

    int getHealth() const;

protected:
    int x, y;
    char symbol;
    int enemyHealth;
    sf::Clock enemyClock;
    sf::Clock enemyDamageClock;
    sf::Clock damageDisplayClock;

    bool isTakingDamage;
    bool isDisplayingDamage;
    bool alive;
    int lastDamageAmount;
};

class Goblin : public Enemy {
public:
    Goblin(int x, int y);
    void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
    void attack() override;
    void playDeathAnimation(sf::RenderWindow& window) override;
};

class Orc : public Enemy {
public:
    Orc(int x, int y);
    void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
    void attack() override;
    void playDeathAnimation(sf::RenderWindow& window) override;
};

#endif // ENEMIES_H
