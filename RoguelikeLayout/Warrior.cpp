#include "Warrior.h"
#include "SoundManager.h"
#include "Boss.h"
#include <iostream>

Warrior::Warrior(int x, int y)
    : Player(x, y, '@', 150, 500, generateRandomStats(3, 0, 3, 0, 0, 0), ClassType::Warrior), cleaveEnabled(false) {
    attackCooldown = 0.25f;
}

void Warrior::attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) {
    if (attackCooldownClock.getElapsedTime().asSeconds() < attackCooldown) { // Cooldown between melee attacks
        return;
    }

    bool attacked = false;

    // Call base class attack function
    Player::attack(enemies, map, room);

    // Check if an attack was performed
    for (Enemy* enemy : enemies) {
        int enemyX = enemy->getX();
        int enemyY = enemy->getY();

        if ((std::abs(enemyX - x) <= 1 && std::abs(enemyY - y) <= 1)) {
            attacked = true;
            break;
        }
    }

    // Handle cleave logic if an attack was performed
    if (attacked && cleaveEnabled) {
        for (Enemy* enemy : enemies) {
            int enemyX = enemy->getX();
            int enemyY = enemy->getY();

            if ((std::abs(enemyX - x) <= 1 && std::abs(enemyY - y) <= 1)) {
                for (Enemy* adjacentEnemy : enemies) {
                    if (adjacentEnemy != enemy &&
                        (std::abs(adjacentEnemy->getX() - enemyX) <= 1 && std::abs(adjacentEnemy->getY() - enemyY) <= 1)) {
                        adjacentEnemy->takeDamage(getModifiedAttackDamage() / 2);
                        std::cout << "Warrior cleaves adjacent enemies at (" << adjacentEnemy->getX() << ", " << adjacentEnemy->getY() << ")!" << std::endl;
                    }
                }
            }
        }
    }

    if (attacked) {
        SoundManager::getInstance().playSound("player_attack");
        attackCooldownClock.restart();
    }
}

void Warrior::enableCleave() {
    cleaveEnabled = true;
}

void Warrior::disableCleave() {
	cleaveEnabled = false;
}
