#include "Mage.h"
#include <iostream>

Mage::Mage(int x, int y)
    : Player(x, y, '@', 100, 10, generateRandomStats(0, 0, 0, 3, 3, 0), ClassType::Mage), projectile(nullptr) {
    attackCooldown = 0.50f;
}

void Mage::attack(std::vector<Enemy*>& enemies) {
    if (attackCooldownClock.getElapsedTime().asSeconds() < attackCooldown) { // Cooldown between attacks
        return;
    }

    Enemy* closestEnemy = nullptr;
    float closestDistance = 4.0f; // Maximum range for the attack

    for (Enemy* enemy : enemies) {
        int enemyX = enemy->getX();
        int enemyY = enemy->getY();
        float distance = std::sqrt(std::pow(enemyX - x, 2) + std::pow(enemyY - y, 2));

        if (distance <= closestDistance) {
            closestDistance = distance;
            closestEnemy = enemy;
        }
    }

    if (closestEnemy) {
        int enemyX = closestEnemy->getX();
        int enemyY = closestEnemy->getY();
        closestEnemy->takeDamage(attackDmg);
        if (attackCounter % 2 == 0 && elementalDamage == ElementalType::Fire) {
            int totalFireDamage = getTotalFireDamage();
            closestEnemy->applyFireDamage(totalFireDamage);
            std::cout << "Fire attack!\n";
        }
        std::cout << "Mage attacks enemy at (" << enemyX << ", " << enemyY << ")!" << std::endl;
        projectile = new Projectile(x, y, closestEnemy->getX(), closestEnemy->getY());
        attackCooldownClock.restart();
    }
}

void Mage::renderProjectile(sf::RenderWindow& window, int charSize) {
    if (projectile != nullptr) {
        projectile->update();
        projectile->render(window, charSize);
        if (!projectile->isActive()) {
            delete projectile;
            projectile = nullptr;
        }
    }
}
