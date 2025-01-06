// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Enemies.h" // Include the Enemies header file
#include "Stats.h"
#include "Upgrade.h"
#include "ElementalType.h"
#include "StatManager.h"

// Forward declarations
class Enemy;
class Upgrade;
enum class ClassType;
struct Room;

class Player {
public:
	Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats, ClassType classType);
	virtual ~Player() = default;
	virtual void move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies, const std::vector<Room>& rooms);
	virtual void attack(std::vector<Enemy*>& enemies) = 0;
	virtual void update(const std::vector<Enemy*>& enemies);
	virtual void render(sf::RenderWindow& window, int charSize);
	int getX() const;
	int getY() const;
	void setPosition(int newX, int newY);
	int getHealth() const;
	void loseHealth(int amount);
	void gainHealth(int amount);
	void updateHealth();
	void updateAttackDamage();
	bool attemptDodge() const;
	sf::Clock attackCooldownClock;
	const Stat& getStats() const;
	Stat& getMutableStats();

	ClassType getClassType() const;

	bool isInMerchantRoom(const std::vector<Room>& rooms) const;

	// Upgrades
	void applyUpgrade(const Upgrade& upgrade);
	void increaseAttackDamage(int amount);
	void increaseSpeed(float amount);
	void increaseAttackSpeed(float amount);
	void lifesteal(float amount);

	// Elemental
	void setElementalDamage(ElementalType type, int damage);
	int getTotalFireDamage() const;
	int attackCounter;

	// Gold system
	int getGold() const;
	void addGold(int amount);
	void spendGold(int amount);

	// Display upgrades
	const std::vector<Upgrade>& getCollectedUpgrades() const {
		return appliedUpgrades;
	}

	// StatManager
	StatManager& getStatManager();

	bool isCollidingWithBoss(int newX, int newY, const std::vector<Enemy*>& enemies) const;

private:
	sf::Clock damageClock;  // Player take damage flash effect clock
	bool isTakingDamage;
	int gold;

protected:
	int x, y;
	char symbol;
	int health;
	int attackDmg;
	float speed;
	float attackCooldown = 0;
	Stat stats;
	ClassType classType;
	StatManager statManager;

	// Elemental damage
	ElementalType elementalDamage;
	int fireDamage;
	std::vector<Upgrade> appliedUpgrades;
};

#endif // PLAYER_H
