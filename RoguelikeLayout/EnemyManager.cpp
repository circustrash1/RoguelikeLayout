#include "EnemyManager.h"
#include "Player.h"
#include <cstdlib>


EnemyManager::~EnemyManager() {
	for (Enemy* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}

void EnemyManager::spawnEnemies(int roomX, int roomY, int roomWidth, int roomHeight, std::vector<std::vector<char>>& map) {
	int numEnemies = std::rand() % 5 + 5;

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
	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* enemy = *it;
		if (!enemy->isAlive() && enemy->isDeathAnimationComplete()) {
			delete enemy;
			it = enemies.erase(it);
		}
		else {
			enemy->move(map, playerX, playerY, enemies);
			//enemy->attack();
			++it;
		}
	}
}

void EnemyManager::renderEnemies(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player) {
	for (Enemy* enemy : enemies) {
		if (enemy->isAlive() || enemy->isPlayingDeathAnimation)
		enemy->render(window, charSize, playerX, playerY, player, enemies);
	}
}

bool EnemyManager::allEnemiesDead() const {
	return enemies.empty();
}

const std::vector<Enemy*>& EnemyManager::getEnemies() const {
	return enemies;
}

void EnemyManager::clearEnemies() {
	for (Enemy* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}