#include "Enemies.h"
#include "Player.h"
#include <iostream>

Enemy::Enemy(int x, int y, char symbol, int enemyHealth)
	: x(x), y(y), symbol(symbol), isTakingDamage(false), isDisplayingDamage(false), enemyHealth(enemyHealth), alive(true) {
}

int Enemy::getX() const {
	return x;
}

int Enemy::getY() const {
	return y;
}

char Enemy::getSymbol() const {
	return symbol;
}

void Enemy::takeDamage(int damage) {
	enemyHealth -= damage;
	if (enemyHealth < 0) {
		enemyHealth = 0;
		alive = false;
	}
	isTakingDamage = true;
	isDisplayingDamage = true;
	enemyDamageClock.restart();
	damageDisplayClock.restart();
	lastDamageAmount = damage;
}

bool Enemy::isAlive() const {
	return alive;
}

void Enemy::displayDamage(sf::RenderWindow& window, int charSize) {
	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize / 2);
	text.setString(std::to_string(lastDamageAmount));
	text.setFillColor(sf::Color::Yellow);

	float offsetX = (charSize - text.getLocalBounds().width) / 2;
	float offsetY = charSize / 2;

	text.setPosition((x * charSize - 5) + offsetX, (y * charSize) - offsetY);
	window.draw(text);
}

void Enemy::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies) {
	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);
	text.setString(this->symbol);

	int distance = std::abs(playerX - x) + std::abs(playerY - y);
	int maxDistance = 10;
	int minOpacity = 0;
	int maxOpacity = 255;

	int opacity = maxOpacity - (distance * (maxOpacity - minOpacity) / maxDistance);
	if (opacity < minOpacity) {
		opacity = minOpacity;
	}

	text.setFillColor(sf::Color(255, 0, 0, opacity));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		player->attack(const_cast<std::vector<Enemy*>&>(enemies));
		std::cout << "Enemy at (" << x << ", " << y << ") has " << enemyHealth << " health." << std::endl;
	}

	if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) {
		text.setFillColor(sf::Color(255, 255, 255, opacity));
	}
	else {
		text.setFillColor(sf::Color(255, 0, 0, opacity));
		isTakingDamage = false;
	}

	if (isDisplayingDamage && damageDisplayClock.getElapsedTime().asSeconds() < 0.5f) {
		displayDamage(window, charSize);
	}
	else {
		isDisplayingDamage = false;
	}

	text.setPosition(x * charSize, y * charSize);
	window.draw(text);
}

Goblin::Goblin(int x, int y) : Enemy(x, y, 'G', 50) {}

void Goblin::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
	int dx = std::rand() % 3 - 1;
	int dy = std::rand() % 3 - 1;

	if (enemyClock.getElapsedTime().asSeconds() > 1) {
		if (x < playerX) dx = 1;
		else if (x > playerX) dx = -1;

		if (y < playerY) dy = 1;
		else if (y > playerY) dy = -1;

		int newX = x + dx;
		int newY = y + dy;

		bool collision = false;
		for (const Enemy* enemy : enemies) {
			if (enemy != this && enemy->getX() == newX && enemy->getY() == newY) {
				collision = true;
				break;
			}
		}

		if (newX == playerX && newY == playerY) {
			collision = true;
		}

		if (!collision && map[newY][newX] == '.') {
			x += dx;
			y += dy;
		}
		enemyClock.restart();
	}
}

void Goblin::attack() {
	// Implement Goblin-specific attack logic
}

void Goblin::playDeathAnimation(sf::RenderWindow& window) {
	// Implement Goblin-specific death animation
}

Orc::Orc(int x, int y) : Enemy(x, y, 'O', 100) {}

void Orc::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
	int dx = std::rand() % 3 - 1;
	int dy = std::rand() % 3 - 1;

	if (enemyClock.getElapsedTime().asSeconds() > 1) {
		if (x < playerX) dx = 1;
		else if (x > playerX) dx = -1;

		if (y < playerY) dy = 1;
		else if (y > playerY) dy = -1;

		int newX = x + dx;
		int newY = y + dy;

		bool collision = false;
		for (const Enemy* enemy : enemies) {
			if (enemy != this && enemy->getX() == newX && enemy->getY() == newY) {
				collision = true;
				break;
			}
		}

		if (newX == playerX && newY == playerY) {
			collision = true;
		}

		if (!collision && map[newY][newX] == '.') {
			x += dx;
			y += dy;
		}
		enemyClock.restart();
	}
}

void Orc::attack() {
	// Implement Orc-specific attack logic
}

void Orc::playDeathAnimation(sf::RenderWindow& window) {
	// Implement Orc-specific death animation
}
