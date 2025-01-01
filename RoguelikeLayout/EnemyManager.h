#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include "Enemies.h"
#include "Map.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include "Player.h"

// Forward declaration of the Player class
class Player;

struct Room;

class EnemyManager {
public:
	~EnemyManager();

	void spawnEnemies(int roomX, int roomY, int roomWidth, int roomHeight, std::vector<std::vector<char>>& map);
	void updateEnemies(const std::vector<std::vector<char>>& map, int playerX, int playerY);
	void renderEnemies(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player);
	const std::vector<Enemy*>& getEnemies() const;
	bool allEnemiesDead() const;
	void clearEnemies();

private:
	std::vector<Enemy*> enemies;
};



#endif // ENEMYMANAGER_H
