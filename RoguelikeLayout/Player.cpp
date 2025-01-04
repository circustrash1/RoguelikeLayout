// Player.cpp
#include "Player.h"
#include "Map.h"
#include <iostream>

Player::Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats, ClassType classType)
	: x(x), y(y), symbol(symbol), health(health), attackDmg(attackDmg), classType(classType),
	stats(stats), elementalDamage(ElementalType::None) {
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
	health -= amount;
	if (health < 0) {
		health = 0;
	}
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
	case 'w': newY -= 1; break; // Move up
	case 's': newY += 1; break; // Move down
	case 'a': newX -= 1; break; // Move left
	case 'd': newX += 1; break; // Move right
	}

	// Check for collision with enemies
	for (const Enemy* enemy : enemies) {
		if (enemy->getX() == newX && enemy->getY() == newY && enemy->isAlive()) {
			return; // Collision detected, do not move
		}
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

	// Check if the new position is within bounds and walkable
	if (newX >= 0 && newY >= 0 && newX < map[0].size() && newY < map.size() && map[newY][newX] == '.') {
		x = newX;
		y = newY;
	}
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
	if (damageClock.getElapsedTime().asSeconds() >= 1.0f) {
		for (const Enemy* enemy : enemies) {
			int enemyX = enemy->getX();
			int enemyY = enemy->getY();
			if ((std::abs(enemyX - x) <= 1 && enemyY == y) || (std::abs(enemyY - y) <= 1 && enemyX == x)) {
				loseHealth(2);
				break;
			}
		}
		damageClock.restart();
	}

	for (Enemy* enemy : enemies) {
		enemy->updateFireDamage();
	}
}

const Stat& Player::getStats() const {
	return stats;
}

Stat& Player::getMutableStats() {
	return stats;
}

void Player::render(sf::RenderWindow& window, int charSize) const {
	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);
	text.setFillColor(sf::Color::White);
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