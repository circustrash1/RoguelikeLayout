#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include "Enemies.h"
#include "Map.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "unordered_set"
#include "functional"

// Forward declaration of the Player class
class Player;

struct Room;

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void spawnEnemies(int roomX, int roomY, int roomWidth, int roomHeight, std::vector<std::vector<char>>& map, const Room& room);
	void updateEnemies(const std::vector<std::vector<char>>& map, int playerX, int playerY, Player* player);
	void renderEnemies(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<std::vector<char>>& map);
	const std::vector<Enemy*>& getEnemies() const;
	bool allEnemiesDead() const;
	void clearEnemies();

	void incrementScaleFactors();
	void resetScaleFactors();

	// Boss
	void spawnRandomBoss(int x, int y);

private:
	std::vector<Enemy*> enemies;
	float healthScaleFactor;
	float damageScaleFactor;

	std::unordered_set<std::string> spawnedBosses;
	std::vector<std::function<Enemy*(int, int)>> bossConstructors;
	void initBossConstructors();
};

#endif // ENEMYMANAGER_H
