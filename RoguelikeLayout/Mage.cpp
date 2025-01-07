#include "Mage.h"
#include <iostream>

Mage::Mage(int x, int y)
	: Player(x, y, '@', 100, 10, generateRandomStats(0, 0, 0, 3, 3, 0), ClassType::Mage), projectile(nullptr), maxRange(25.0f) {
	attackCooldown = 0.50f;
}

void Mage::attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) {
    if (attackCooldownClock.getElapsedTime().asSeconds() < attackCooldown) { // Cooldown between attacks
        return;
    }

    // Call base class attack function
    Player::attack(enemies, map, room);

    Enemy* closestEnemy = nullptr;
    float closestDistance = maxRange; // Maximum range for the attack

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
        applyKnockback(closestEnemy, map, room);
        if (attackCounter % 2 == 0 && elementalDamage == ElementalType::Fire) {
            int totalFireDamage = getTotalFireDamage();
            closestEnemy->applyFireDamage(totalFireDamage);
            std::cout << "Fire attack!\n";
        }
        std::cout << "Mage attacks enemy at (" << enemyX << ", " << enemyY << ")!" << std::endl;
        projectile = new Projectile(x, y, closestEnemy->getX(), closestEnemy->getY());

        // Lifesteal mechanic
        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 100);
        if (dist(rng) <= lifestealChance) {
            gainHealth(lifestealAmount);
            std::cout << "Lifesteal! Gained " << lifestealAmount << " health." << std::endl;
        }

        SoundManager::getInstance().playSound("player_attack");
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