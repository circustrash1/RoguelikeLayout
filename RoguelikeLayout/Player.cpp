// Player.cpp
#include "Player.h"
#include "Map.h"
#include "SoundManager.h""
#include <iostream>

Player::Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats, ClassType classType)
	: x(x), y(y), symbol(symbol), health(health), attackDmg(attackDmg), classType(classType),
	stats(stats), elementalDamage(ElementalType::None), statManager(*this), isTakingDamage(false) {
}

void Player::updateHealth() {
	health = statManager.getMaxHealth();
}

void Player::updateAttackDamage() {
	attackDmg = statManager.getAttackDamage();
}

bool Player::attemptDodge() const {
	float dodgeChance = statManager.getDodgeChance();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	std::cout << "Dodge chance: " << dodgeChance << std::endl;
	return dis(gen) < dodgeChance;
}

int Player::getX() const {
	return x;
}

int Player::getY() const {
	return y;
}

void Player::setPosition(int newX, int newY) {
	x = newX;
	y = newY;
}

int Player::getHealth() const {
	return health;
}

void Player::loseHealth(int amount) {
	if (attemptDodge()) {
		std::cout << "Dodge successful!" << std::endl;
		SoundManager::getInstance().playSound("player_dodge");
		return;
	}
	SoundManager::getInstance().playSound("player_damage");
	health -= amount;
	if (health < 0) {
		health = 0;
	}
	isTakingDamage = true;
	damageClock.restart();
}

void Player::gainHealth(int amount) {
	health += amount;
}

int Player::getGold() const {
	return gold;
}

void Player::addGold(int amount) {
	gold += amount;
}

void Player::spendGold(int amount) {
	gold -= amount;
}

void Player::move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies, const std::vector<Room>& rooms) {

	
	int newX = x, newY = y;

	switch (direction) {
	case 'w': // Up
		newY--;
		break;
	case 's': // Down
		newY++;
		break;
	case 'a': // Left
		newX--;
		break;
	case 'd': // Right
		newX++;
		break;
	case 'q': // Up-Left
		newX--;
		newY--;
		break;
	case 'e': // Up-Right
		newX++;
		newY--;
		break;
	case 'z': // Down-Left
		newX--;
		newY++;
		break;
	case 'c': // Down-Right
		newX++;
		newY++;
		break;
	default:
		break;
	}

	// Check for collision with enemies
	for (const Enemy* enemy : enemies) {
		if (enemy->getX() == newX && enemy->getY() == newY && enemy->isAlive()) {
			return; // Collision detected, do not move
		}
	}


	// Check for collisions with the boss
	if (isCollidingWithBoss(newX, newY, enemies)) {
		return;
	}

	// Check for collision with merchant and eventChar
	for (const Room& room : rooms) {
		if (room.isMerchantRoom && room.merchantSpawned) {
			if (room.merchantPosition.first == newX && room.merchantPosition.second == newY) {
				return;
			}
		}
		if (room.eventCharVisible) {
			if (room.eventCharPosition.first == newX && room.eventCharPosition.second == newY) {
				return;
			}
		}
	}

	// Check for wall collisions
	if (newX >= 0 && newX < map[0].size() && newY >= 0 && newY < map.size() //&& map[newY][newX] == '.'
		) {
		x = newX;
		y = newY;
	}
}

bool Player::isCollidingWithBoss(int newX, int newY, const std::vector<Enemy*>& enemies) const {
	for (const auto& enemy : enemies) {
		if (const Boss* boss = dynamic_cast<const Boss*>(enemy)) {
			for (const auto& part : boss->getBodyParts()) {
				if (part.first == newX && part.second == newY) {
					return true;
				}
			}
		}
	}
	return false;
}


// UPGRADES GO HERE

void Player::applyUpgrade(const Upgrade& upgrade) {
	upgrade.applyUpgrade(*this);
	appliedUpgrades.push_back(upgrade);
	std::cout << "Applied upgrade: " << upgrade.getName() << " with fire damage: " << upgrade.getFireDamage() << std::endl;
}

void Player::increaseAttackDamage(int amount) {
	attackDmg += amount;
}

void Player::increaseSpeed(float amount) {
	speed += amount;
}

void Player::increaseAttackSpeed(float amount) {
	float diminishingFactor = 1.0f / (1.0f + std::exp(attackCooldown - 1.0f));
	attackCooldown -= amount * diminishingFactor;
}

//void Player::lifesteal(float amount) {
//	health += amount;
//	if (health > maxHealth) {
//		health = maxHealth;
//	}
//}

void Player::setElementalDamage(ElementalType type, int damage) {
	elementalDamage = type;
	if (type == ElementalType::Fire) {
		fireDamage = damage;
	}
}

int Player::getTotalFireDamage() const {
	int totalFireDamage = 0;
	std::cout << "Calculating total fire damage..." << std::endl; // Debugging
	for (const auto& upgrade : appliedUpgrades) {
		std::cout << "Checking upgrade: " << upgrade.getName() << " with fire damage: " << upgrade.getFireDamage() << std::endl; // Debugging
		if (upgrade.getElementalType() == ElementalType::Fire) {
			totalFireDamage += upgrade.getFireDamage();
			std::cout << "You now have " << totalFireDamage << " fire damage." << std::endl;
		}
	}
	return totalFireDamage;
}

void Player::update(const std::vector<Enemy*>& enemies) {
	for (Enemy* enemy : enemies) {
		if (enemy->getAttackCooldownClock().getElapsedTime().asSeconds() >= enemy->getAttackCooldown()) {
			int enemyX = enemy->getX();
			int enemyY = enemy->getY();
			if ((std::abs(enemyX - x) <= 1 && std::abs(enemyY - y) <= 1)) {
				SoundManager::getInstance().playSound("goblin_attack");
				loseHealth(enemy->getAttackDamage());
				const_cast<sf::Clock&>(enemy->getAttackCooldownClock()).restart();
				break;
			}
		}
		enemy->updateFireDamage();

		// Check if the enemy is a SkeletonArcher and within attack range
		if (SkeletonArcher* archer = dynamic_cast<SkeletonArcher*>(enemy)) {
			int distance = std::abs(archer->getX() - x) + std::abs(archer->getY() - y);
			if (distance <= 5) { // Adjust the range as needed
				archer->attack(this);
			}
		}
	}
}

const Stat& Player::getStats() const {
	return stats;
}

Stat& Player::getMutableStats() {
	return stats;
}

void Player::render(sf::RenderWindow& window, int charSize) {
	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);

	if (isTakingDamage && damageClock.getElapsedTime().asSeconds() < 0.1f) {
		text.setFillColor(sf::Color::White);
	}
	else {
		text.setFillColor(sf::Color::Blue);
		isTakingDamage = false;
	}

	text.setString(symbol);
	text.setPosition(x * charSize, y * charSize);
	window.draw(text);
}

ClassType Player::getClassType() const {
	return classType;
}

bool Player::isInMerchantRoom(const std::vector<Room>& rooms) const {
	for (const Room& room : rooms) {
		if (room.isMerchantRoom && x >= room.startX && x <= room.endX && y >= room.startY && y <= room.endY) {
			return true;
		}
	}
	return false;
}

StatManager& Player::getStatManager() {
	return statManager;
}