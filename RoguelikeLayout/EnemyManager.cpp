#include "EnemyManager.h"
#include "Player.h"
#include <cstdlib>

void EnemyManager::spawnEnemies(int roomX, int roomY, int roomWidth, int roomHeight, std::vector<std::vector<char>>& map) {
	int numEnemies = std::rand() % 10 + 3;

	for (int i = 0; i < numEnemies; ++i) {
		int x = 0, y = 0;
		do {
			x = roomX + std::rand() % roomWidth;
			y = roomY + std::rand() % roomHeight;

			if (std::rand() % 2 == 0) {
				enemies.push_back(new Goblin(x, y));
			}
			else {
				enemies.push_back(new Orc(x, y));
			}
		} while (map[y][x] != '.');
	}
}

void EnemyManager::updateEnemies(const std::vector<std::vector<char>>& map, int playerX, int playerY) {
	for (Enemy* enemy : enemies) {
		enemy->move(map, playerX, playerY, enemies); // Ensure the move method in Enemy class takes the map as an argument
		enemy->attack();
	}
}

void EnemyManager::renderEnemies(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player) {
	for (Enemy* enemy : enemies) {
		enemy->render(window, charSize, playerX, playerY, player, enemies);
	}
}

const std::vector<Enemy*>& EnemyManager::getEnemies() const {
	return enemies;
}