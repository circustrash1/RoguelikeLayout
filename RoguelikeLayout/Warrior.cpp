#include "Warrior.h"
#include <iostream>

Warrior::Warrior(int x, int y)
	: Player(x, y, '@', 150, 15, generateRandomStats(3, 0, 3, 0, 0, 0), ClassType::Warrior), cleaveEnabled(false){
}

void Warrior::attack(std::vector<Enemy*>& enemies) {
	if (attackCooldownClock.getElapsedTime().asSeconds() < 0.25f) {	// Cooldown between melee attacks
		return;
	}
	for (Enemy* enemy : enemies) {
		int enemyX = enemy->getX();
		int enemyY = enemy->getY();

		if ((std::abs(enemyX - x) <= 1 && enemyY == y) || (std::abs(enemyY - y) <= 1 && enemyX == x)) {
			enemy->takeDamage(attackDmg);

			if (attackCounter % 2 == 0 && elementalDamage == ElementalType::Fire) {
				int totalFireDamage = getTotalFireDamage();
				enemy->applyFireDamage(totalFireDamage);
				std::cout << "Fire attack!\n";
			}

			if (cleaveEnabled) {
				for (Enemy* adjacentEnemy : enemies) {
					if (adjacentEnemy != enemy &&
						((std::abs(adjacentEnemy->getX() - enemyX) <= 1 && adjacentEnemy->getY() == enemyY) ||
							(std::abs(adjacentEnemy->getY() - enemyY) <= 1 && adjacentEnemy->getX() == enemyX))) {
						adjacentEnemy->takeDamage(attackDmg / 2);
						std::cout << "Warrior cleaves adjacent enemies at (" << adjacentEnemy->getX() << ", " << adjacentEnemy->getY() << ")!" << std::endl;
					}
				}
				
			}
			attackCooldownClock.restart();
			break;
		}
	}
}

void Warrior::enableCleave() {
	cleaveEnabled = true;
}