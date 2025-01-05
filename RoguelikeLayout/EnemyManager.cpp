#include "EnemyManager.h"
#include "Player.h"
#include <cstdlib>

EnemyManager::EnemyManager() : healthScaleFactor(1.0f), damageScaleFactor(1.0f) {
}

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
		} while (map[y][x] != '.');

		int enemyType = std::rand() % 3;
		Enemy* enemy = nullptr;

		if (enemyType == 2) {
			enemy = new Goblin(x, y);
		}
		else if (enemyType == 1) {
			enemy = new Orc(x, y);
		}
		else {
			enemy = new SkeletonArcher(x, y);
		}

		if (enemy) {
			enemy->scaleAttributes(healthScaleFactor, damageScaleFactor);
			enemies.push_back(enemy);
		}
	}
}

void EnemyManager::incrementScaleFactors() {
	healthScaleFactor += 0.5f; // Increment health scale factor
	damageScaleFactor += 0.5f; // Increment damage scale factor
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