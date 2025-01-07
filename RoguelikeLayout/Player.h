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
	virtual void attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) = 0;
	bool isAdjacent(Enemy* enemy) const;
	virtual void update(const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map);
	virtual void render(sf::RenderWindow& window, int charSize);
	int getX() const;
	int getY() const;
	void setPosition(int newX, int newY);
	int getHealth() const;
	void loseHealth(int amount, Enemy* attacker);
	void gainHealth(int amount);
	int getMaxHealth();
	void updateHealth();
	void updateAttackDamage();
	bool attemptDodge() const;
	sf::Clock attackCooldownClock;
	int bossKillCount = 0;
	int totalKillCount = 0;

	const Stat& getStats() const;
	Stat& getMutableStats();

	ClassType getClassType() const;

	bool isInMerchantRoom(const std::vector<Room>& rooms) const;
	bool isInBossRoom(const std::vector<Room>& rooms) const;
	bool isInBossCorridor(const std::vector<Room>& rooms) const;

	// Stun mechanic
	void applyStun(float duration);
	void clearStun();
	bool isStunned() const;

	// Knockback mechanic
	void applyKnockback(Enemy* enemy, const std::vector<std::vector<char>>& map, const Room& room);

	// Upgrades
	void applyUpgrade(const Upgrade& upgrade);
	void increaseAttackDamage(int amount);
	void increaseMaxHealth(int amount);
	void increaseSpeed(float amount);
	void increaseAttackSpeed(float amount);
	void increaseLifesteal(float amount);
	void recalculateStats();
	void applyGlassCannon();
	void applyThorns(int amount);
	void applyCritChance(int amount);
	bool isCriticalHit() const;
	void applyGuaranteedCrit();
	void applyDyingRage(int amount);
	int getModifiedAttackDamage() const;
	void applyLuckyFive();
	void applyReapersScythe();
	void applyReapersScytheDamage();
	void resetReapersScythe();
	void updateReapersScythe();
	void applySoulEater();
	void applySoulEaterDamage();
	void applyShockwave();


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
	std::vector<Upgrade>& getMutableCollectedUpgrades() {
		return appliedUpgrades;
	}

	// StatManager
	StatManager& getStatManager();

	bool isCollidingWithBoss(int newX, int newY, const std::vector<Enemy*>& enemies) const;

private:
	sf::Clock damageClock;  // Player take damage flash effect clock
	bool isTakingDamage;
	int gold;

	// Stun mechanic
	bool stunned = false;
	sf::Clock stunClock;
	float stunDuration = 0.0f;

protected:
	int x, y;
	char symbol;
	int health;
	int maxHealth;
	int baseMaxHealth; // Base max health for scaling
	int attackDmg;
	int baseAttackDamage; // Base attack damage for scaling
	float speed;
	float attackCooldown = 0;


	// UPGRADES
	float lifestealChance;
	float lifestealAmount;
	float glassCannonHealthModifier;
	float glassCannonDamageModifier;
	float thornsDamage = 0.0f;
	float critChance = 0.0f;
	int dyingRageDamage = 0;
	bool guaranteedCrit = false;
	bool hasLuckyFive = false;
	int reapersScytheBonusDamage = 0;
	sf::Clock reapersScytheClock;
	bool hasReapersScythe = false;
	bool hasSoulEater = false;
	int soulEaterBonusDamage = 0;
	int killCount = 0;
	int knockbackDistance = 0;

	Stat stats;
	ClassType classType;
	StatManager statManager;

	// Elemental damage
	ElementalType elementalDamage;
	int fireDamage;
	std::vector<Upgrade> appliedUpgrades;
};

#endif // PLAYER_H
