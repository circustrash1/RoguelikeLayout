// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Enemies.h" // Include the Enemies header file
#include "Stats.h"

class Enemy; // Forward declaration of Enemy class

class Player {
public:
    Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats);
    virtual ~Player() = default;
    virtual void move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies);
    virtual void attack(std::vector<Enemy*>& enemies) = 0;
    virtual void update(const std::vector<Enemy*>& enemies);
    virtual void render(sf::RenderWindow& window, int charSize) const;
    int getX() const;
    int getY() const;
    int getHealth() const;
    void loseHealth(int amount);
    sf::Clock attackCooldownClock;
    const Stat& getStats() const;

private:
    sf::Clock damageClock;


protected:
    int x, y;
    char symbol;
    int health;
    int attackDmg;
    Stat stats;
};

#endif // PLAYER_H
