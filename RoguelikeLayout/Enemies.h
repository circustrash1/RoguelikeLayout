#ifndef ENEMIES_H
#define ENEMIES_H

#include <string>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ElementalType.h"
#include "Projectile.h"

// Forward declaration
class Player;
class Projectile;

class Enemy {
public:
    Enemy(int x, int y, char symbol, int enemyHealth, int attackDamage, float attackCooldown); // Updated constructor declaration
    virtual ~Enemy() = default;

    virtual void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) = 0;
    virtual void attack(Player* player) = 0;
    void render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies);

    int getAttackDamage() const;
    const sf::Clock& getAttackCooldownClock() const;
	float getAttackCooldown() const;    // Get unique enemy attack cooldown

    // Enemy health bars
    void renderHealthBar(sf::RenderWindow& window, int charSize) const;
    void updateHealthBar(int charSize);
    sf::RenderTexture healthBarTexture;
	bool healthBarNeedsUpdate;

    void takeDamage(int damage);
    void displayDamage(sf::RenderWindow& window, int charSize);
	void setColor(const sf::Color& color);
    bool isAlive() const;
    virtual void playDeathAnimation(sf::RenderWindow& window) = 0;
    bool isDeathAnimationComplete() const;
    bool isPlayingDeathAnimation;

    int getX() const;
    int getY() const;
    char getSymbol() const;

    int getHealth() const;

    // Elemental
    void takeFireDamage(int damage);
    void displayFireDamage(sf::RenderWindow& window, int charSize);
    void applyFireDamage(int damage);
    void updateFireDamage();


	// Gold system
    int dropGold();
    bool isGoldDropped() const;
    bool isGoldAmountAdded() const;
    void setGoldAmountAdded(bool value);

    // Scaling
    void scaleAttributes(float healthFactor, float damageFactor);


protected:
    int x, y;
    char symbol;
    int enemyHealth;
    int maxHealth;
    int attackDamage;
    float attackCooldown;   // Individual enemy attack cooldown
    sf::Clock attackCooldownClock;  // Enemy attack cooldown clock
    sf::Clock deathAnimationClock;
    sf::Clock enemyClock;
	sf::Clock enemyDamageClock; // Damage flash rate
    sf::Clock damageDisplayClock;
    sf::Clock fireDamageClock;  // Fire tick rate

    bool isTakingDamage;
    bool isTakingFireDamage;
    bool isDisplayingDamage;
	bool isDisplayingFireDamage;
    bool alive;

    bool goldDropped = false;
    bool goldAmountAdded = false;
    int goldAmount = 0;


    int lastDamageAmount;
    int lastFireDamageAmount;

    ElementalType elementalType;
    int fireDamageOverTime;

    sf::Color color;
};

class Goblin : public Enemy {
public:
    Goblin(int x, int y);
    void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
    void attack(Player* player) override;
    void playDeathAnimation(sf::RenderWindow& window) override;
};

class Orc : public Enemy {
public:
    Orc(int x, int y);
    void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
    void attack(Player* player) override;
    void playDeathAnimation(sf::RenderWindow& window) override;
};

class SkeletonArcher : public Enemy {
public:
	SkeletonArcher(int x, int y);
	void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
    void attack(Player* player) override;
    void renderProjectiles(sf::RenderWindow& window, int charSize);
	void playDeathAnimation(sf::RenderWindow& window) override;


private:
    Projectile* projectile;
    bool isRetreating;
    sf::Clock moveCooldownClock;
    float moveCooldown;
};
#endif // ENEMIES_H
