#ifndef BOSS_H
#define BOSS_H

#include "Enemies.h"
#include <vector>
#include <string>

class Boss : public Enemy {
public:
	Boss(int x, int y, int enemyHealth, int attackDamage, float attackCooldown, const sf::Color& color = sf::Color::Red);
	void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
	void attack(Player* player) override;
	void playDeathAnimation(sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) override;
	void renderBossHealthBar(sf::RenderWindow& window, int charSize, float scaleX, float scaleY) const;
	const std::vector<std::pair<int, int>>& getBodyParts() const { return bodyParts; }
	bool checkCollision(const Player& player) const;

protected:
	void initBodyParts(int x, int y);
	std::vector<std::pair<int, int>> bodyParts; // Store the positions of the boss's body parts
	std::vector<std::string> asciiArt; // Store the ASCII art for the boss
	std::vector<char> bodyPartChars; // Store the characters for each body part

	sf::Clock moveClock;
	float moveCooldown;
};

class Havok : public Boss { // Bruiser style boss
public:
	Havok(int x, int y);
	void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
	void attack(Player* player) override;
	void playDeathAnimation(sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) override;

private:
	bool isPlayerInRange(int playerX, int playerY) const;
	void adjustChargeDuration();
	void applyStunAndDamage(int playerX, int playerY, Player* player);
	void renderAOECircle(sf::RenderWindow& window, int charSize, const std::vector<std::vector<char>>& map);
	void renderDamagedTiles(sf::RenderWindow& window, int charSize);


	bool isCharging;
	bool hasAppliedStun;
	float chargeDuration;		// Length of charge up
	float chargeCooldown;		// Time between stuns
	sf::Clock chargeClock;
	sf::Clock chargeFlashClock;		// Clock to flash havok yellow on stun charge
	sf::Clock chargeCooldownClock;		// Cooldown between stuns
	sf::Clock aoeDisplayClock;
	bool displayAOE;
	std::vector<std::pair<int, int>> damagedTiles;	// Store damaged tiles for debug

	int aoeCenterX;
	int aoeCenterY;

};

class Infernos : public Boss { // Fire style boss
public:
	Infernos(int x, int y);
	void move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) override;
	void attack(Player* player) override;
	void playDeathAnimation(sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) override;
};

#endif // BOSS_H
