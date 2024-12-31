#include "Mage.h"
#include <iostream>

Mage::Mage(int x, int y)
	: Player(x, y, '@', 100, 10) {
}

void Mage::attack(std::vector<Enemy*>& enemies) {
	for (Enemy* enemy : enemies) {
		int enemyX = enemy->getX();
		int enemyY = enemy->getY();
		if ((std::abs(enemyX - x) <= 1 && enemyY == y) || (std::abs(enemyY - y) <= 1 && enemyX == x)) {
			std::cout << "Warrior attacks enemy at (" << enemyX << ", " << enemyY << ")!" << std::endl;
			enemy->takeDamage(attackDmg);
			break;
		}
	}
}