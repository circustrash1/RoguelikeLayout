#include "Mage.h"
#include <cmath>
#include <iostream>

Mage::Mage(int x, int y)
    : Player(x, y, '@', 100, 10, generateRandomStats(0, 0, 0, 3, 3, 0), ClassType::Mage), isProjectileActive(false), 
    projectileX(0), projectileY(0), projectileTargetX(0), projectileTargetY(0) {
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
        projectileTargetX = closestEnemy->getX();
        projectileTargetY = closestEnemy->getY();
        isProjectileActive = true;
        projectileClock.restart();
        attackCooldownClock.restart();
    }
}

void Mage::renderProjectile(sf::RenderWindow& window, int charSize) {
    if (!isProjectileActive) {
        return;
    }

    float elapsed = projectileClock.getElapsedTime().asSeconds();
    float duration = 0.1f; // Duration for the projectile to reach the target

    if (elapsed < duration) {
        float t = elapsed / duration;
        int currentX = static_cast<int>(x + t * (projectileTargetX - x));
        int currentY = static_cast<int>(y + t * (projectileTargetY - y));

        sf::Font font;
        if (!font.loadFromFile("fs-min.ttf")) {
            return;
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(charSize);
        text.setString('*'); // ASCII character for the projectile
        text.setFillColor(sf::Color::White);
        text.setPosition(currentX * charSize, currentY * charSize);
        window.draw(text);
    } else {
        isProjectileActive = false;
    }
}
