#include "EnemyManager.h"
#include "Player.h"
#include <iostream>
#include <cstdlib>
#include "Boss.h"
#include <functional>

EnemyManager::EnemyManager() : healthScaleFactor(1.0f), damageScaleFactor(1.0f) {
	initBossConstructors();
}

EnemyManager::~EnemyManager() {
	for (Enemy* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}

void EnemyManager::spawnEnemies(int roomX, int roomY, int roomWidth, int roomHeight, std::vector<std::vector<char>>& map, const Room& room) {
	int numEnemies = std::rand() % 2 + 2;

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

void EnemyManager::initBossConstructors() {
	bossConstructors = {
		[](int x, int y) { return new Havok(x, y);  },
		//[](int x, int y) { return new Infernos(x, y); }	// Removed for debugging
	};
}

void EnemyManager::spawnRandomBoss(int x, int y) {
	if (spawnedBosses.size() == bossConstructors.size()) {
		spawnedBosses.clear();
	}
	
	std::vector<std::function<Enemy* (int, int)>> availableBosses;
	for (const auto& constructor : bossConstructors) {
		std::string bossName = typeid(*constructor(x, y)).name();
		if (spawnedBosses.find(bossName) == spawnedBosses.end()) {
			availableBosses.push_back(constructor);
		}
	}

	if (availableBosses.empty()) {
		std::cerr << "Error: No available bosses to spawn." << std::endl;
		return;
	}

	int randomIndex = std::rand() % bossConstructors.size();
	Enemy* boss = bossConstructors[randomIndex](x, y);
	boss->scaleAttributes(healthScaleFactor, damageScaleFactor);
	spawnedBosses.insert(typeid(*boss).name());
	enemies.push_back(boss);
}

void EnemyManager::incrementScaleFactors() {
	healthScaleFactor += 0.5f; // Increment health scale factor
	damageScaleFactor += 0.5f; // Increment damage scale factor
	std::cout << "Health scale factor: " << healthScaleFactor << ", Damage scale factor: " << damageScaleFactor << std::endl;
}

void EnemyManager::resetScaleFactors() {
	healthScaleFactor = 1.0f; // Reset health scale factor
	damageScaleFactor = 1.0f; // Reset damage scale factor
}

void EnemyManager::updateEnemies(const std::vector<std::vector<char>>& map, int playerX, int playerY, Player* player) {
	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* enemy = *it;
		if (!enemy->isAlive() && enemy->isDeathAnimationComplete()) {
			delete enemy;
			it = enemies.erase(it);
		}
		else {
			enemy->move(map, playerX, playerY, enemies);
			enemy->attack(player, map);
			++it;
		}
	}
}

void EnemyManager::renderEnemies(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<std::vector<char>>& map) {
	for (Enemy* enemy : enemies) {
		if (enemy->isAlive() || enemy->isPlayingDeathAnimation)
			enemy->render(window, charSize, playerX, playerY, player, enemies, map);
	}
}

bool EnemyManager::allEnemiesDead() const {
	return enemies.empty();
}

std::vector<Enemy*>& EnemyManager::getEnemies() {
	return enemies;
}

void EnemyManager::clearEnemies() {
	for (Enemy* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}