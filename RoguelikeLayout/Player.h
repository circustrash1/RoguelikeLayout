// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Enemies.h" // Include the Enemies header file
#include "Stats.h"
#include "Upgrade.h"
#include "ElementalType.h"


class Enemy; // Forward declaration of Enemy class
class Upgrade;
enum class ClassType;

class Player {
public:
    Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats, ClassType classType);
    virtual ~Player() = default;
    virtual void move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies);
    virtual void attack(std::vector<Enemy*>& enemies) = 0;
    virtual void update(const std::vector<Enemy*>& enemies);
    virtual void render(sf::RenderWindow& window, int charSize) const;
    int getX() const;
    int getY() const;
    void setPosition(int newX, int newY);
    int getHealth() const;
    void loseHealth(int amount);
    sf::Clock attackCooldownClock;
    const Stat& getStats() const;
    ClassType getClassType() const;

    // Upgrades
    void applyUpgrade(const Upgrade& upgrade);
    void increaseAttackDamage(int amount);
    void increaseSpeed(float amount);


    // Elemental
    void setElementalDamage(ElementalType type, int damage);
    int getTotalFireDamage() const;
    int attackCounter;

private:
    sf::Clock damageClock;


protected:
    int x, y;
    char symbol;
    int health;
    int attackDmg;
    float speed;
    Stat stats;
    ClassType classType;

    // Elemental damage
    ElementalType elementalDamage;
    int fireDamage;
    std::vector<Upgrade> appliedUpgrades;
};

#endif // PLAYER_H
