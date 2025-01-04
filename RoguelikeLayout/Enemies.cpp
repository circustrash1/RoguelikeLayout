#include "Enemies.h"
#include "Player.h"
#include "ElementalType.h"
#include "SoundManager.h"
#include <iostream>
#include <random>

Enemy::Enemy(int x, int y, char symbol, int enemyHealth)
	: x(x), y(y), symbol(symbol), isTakingDamage(false), isTakingFireDamage(false), isDisplayingDamage(false), isDisplayingFireDamage(false),
	enemyHealth(enemyHealth), maxHealth(enemyHealth), alive(true), isPlayingDeathAnimation(false), fireDamageOverTime(0), goldAmountAdded(false) {
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


// Gold system
int Enemy::dropGold() {
	std::default_random_engine rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(1, 5);
	goldAmount = dist(rng);
	return goldAmount;
}

bool Enemy::isGoldDropped() const {
	return goldDropped;
}

bool Enemy::isGoldAmountAdded() const {
	return goldAmountAdded;
}

void Enemy::setGoldAmountAdded(bool value) {
	goldAmountAdded = value;
}

void Enemy::takeDamage(int damage) {
	if (!alive) return;
	enemyHealth -= damage;
	if (enemyHealth <= 0) {
		enemyHealth = 0;
		alive = false;
		isPlayingDeathAnimation = true;
		deathAnimationClock.restart();
		SoundManager::getInstance().playSound("enemy_death");
	}
	isTakingDamage = true;
	isDisplayingDamage = true;
	enemyDamageClock.restart();
	damageDisplayClock.restart();
	lastDamageAmount = damage;
	healthBarNeedsUpdate = true;
	SoundManager::getInstance().playSound("enemy_take_damage");

	if (!alive && !goldDropped) {
		goldDropped = true;
		dropGold();
	}
}

void Enemy::takeFireDamage(int damage) {
	if (!alive) return;
	std::cout << "Enemy at (" << x << ", " << y << ") takes " << damage << " fire damage." << std::endl;
	enemyHealth -= damage;
	if (enemyHealth <= 0) {
		enemyHealth = 0;
		alive = false;
		isPlayingDeathAnimation = true;
		deathAnimationClock.restart();
		SoundManager::getInstance().playSound("enemy_death");
	}
	isTakingFireDamage = true;
	isDisplayingFireDamage = true;
	fireDamageClock.restart();
	lastFireDamageAmount = damage;
	healthBarNeedsUpdate = true;
	SoundManager::getInstance().playSound("burn_damage");
}

void Enemy::applyFireDamage(int damage) {
	if (!alive) return;
	fireDamageOverTime += damage;
	fireDamageClock.restart();
}

void Enemy::updateFireDamage() {
	if (fireDamageClock.getElapsedTime().asSeconds() >= 1.0f && fireDamageOverTime > 0) {
		takeFireDamage(fireDamageOverTime);
		fireDamageOverTime -= 1;
		fireDamageClock.restart();
	}
}

void Enemy::setColor(const sf::Color& color) {
	this->color = color;
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

void Enemy::displayFireDamage(sf::RenderWindow& window, int charSize) {
	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize / 2);
	text.setString(std::to_string(lastFireDamageAmount));
	text.setFillColor(sf::Color::Red);

	float offsetX = (charSize - text.getLocalBounds().width) / 2;
	float offsetY = charSize / 2;

	text.setPosition((x * charSize - 5) + offsetX, (y * charSize) - offsetY - 10);
	window.draw(text);
}

void Enemy::renderHealthBar(sf::RenderWindow& window, int charSize) const {
    if (!alive || isPlayingDeathAnimation) return;

    sf::Sprite healthBarSprite(healthBarTexture.getTexture());
    healthBarSprite.setPosition(x * charSize - 3, (y * charSize) + 30);
    window.draw(healthBarSprite);
}

void Enemy::playDeathAnimation(sf::RenderWindow& window) {
	std::cout << "Running death animation for enemy at (" << x << ", " << y << ")." << std::endl;
	if (deathAnimationClock.getElapsedTime().asSeconds() < 1.0f) {
		if (static_cast<int>(deathAnimationClock.getElapsedTime().asMilliseconds() / 100) % 2 == 0) {
			symbol = '!';
		}
		else {
			symbol = ' ';
		}
	}
	else {
		isPlayingDeathAnimation = false;
	}
}

bool Enemy::isDeathAnimationComplete() const {
	return deathAnimationClock.getElapsedTime().asSeconds() >= 1.0f;
}


void Enemy::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies) {
	static sf::Font font;
	static bool fontLoaded = false;
	if (!fontLoaded) {
		if (!font.loadFromFile("fs-min.ttf")) {
			return;
		}
		fontLoaded = true;
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

	if (isAlive()) {
		text.setFillColor(sf::Color(255, 0, 0, opacity));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			player->attack(const_cast<std::vector<Enemy*>&>(enemies));
		}

		if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) {        // Flash white when taking damage
			text.setFillColor(sf::Color(255, 255, 255, opacity));
		}
		else {
			text.setFillColor(sf::Color(0, 255, 0, opacity));
			isTakingDamage = false;
		}

		if (isTakingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.25f) {        // Flash red when taking fire damage
			text.setFillColor(sf::Color(255, 0, 0, opacity));
		}

		if (isDisplayingDamage && damageDisplayClock.getElapsedTime().asSeconds() < 0.5f) {        // Show basic attack damage numbers
			displayDamage(window, charSize);
		}
		else {
			isDisplayingDamage = false;
		}
		if (isDisplayingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.5f) {    // Show fire damage numbers
			displayFireDamage(window, charSize);
		}
		else {
			isDisplayingFireDamage = false;
		}

		// Only draw text if the enemy is within a certain distance from the player
		if (distance <= maxDistance) {
			text.setPosition(x * charSize, y * charSize);
			updateHealthBar(charSize);
			renderHealthBar(window, charSize);
			window.draw(text);
		}
	}
	else if (isPlayingDeathAnimation) {
		std::cout << "Playing death animation for enemy at (" << x << ", " << y << ")." << std::endl;
		text.setFillColor(sf::Color(0, 255, 0, opacity));
		playDeathAnimation(window);

		// Draw the death animation text
		text.setPosition(x * charSize, y * charSize);
		window.draw(text);
	}

	// Ensure fire damage display logic is executed
	if (isDisplayingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.5f) {    // Show fire damage numbers
		displayFireDamage(window, charSize);
	}
	else {
		isDisplayingFireDamage = false;
	}
}

Goblin::Goblin(int x, int y) : Enemy(x, y, 'G', 30) {}

void Goblin::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
	if (!alive) return;
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
	if (deathAnimationClock.getElapsedTime().asSeconds() < 1.0f) {
		if (static_cast<int>(deathAnimationClock.getElapsedTime().asMilliseconds() / 100) % 2 == 0) {
			symbol = '!';
		}
		else {
			symbol = ' ';
		}
	}
	else {
		isPlayingDeathAnimation = false;
	}
}

Orc::Orc(int x, int y) : Enemy(x, y, 'O', 60) {}

void Orc::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
	if (!alive) return;
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
	if (deathAnimationClock.getElapsedTime().asSeconds() < 1.0f) {
		if (static_cast<int>(deathAnimationClock.getElapsedTime().asMilliseconds() / 100) % 2 == 0) {
			symbol = '!';
		}
		else {
			symbol = ' ';
		}
	}
	else {
		isPlayingDeathAnimation = false;
	}
}

void Enemy::updateHealthBar(int charSize) {
	if (!healthBarNeedsUpdate) return;

	healthBarTexture.create(charSize, charSize / 4);
	healthBarTexture.clear(sf::Color::Transparent);

	sf::RectangleShape healthBarBackground(sf::Vector2f(charSize, charSize / 4));
	healthBarBackground.setFillColor(sf::Color(255, 0, 0, 75));
	healthBarBackground.setPosition(0, 0);

	float healthPercentage = static_cast<float>(enemyHealth) / maxHealth;
	sf::RectangleShape healthBar(sf::Vector2f(charSize * healthPercentage, charSize / 4));
	healthBar.setFillColor(sf::Color(0, 255, 0, 75));
	healthBar.setPosition(0, 0);

	healthBarTexture.draw(healthBarBackground);
	healthBarTexture.draw(healthBar);
	healthBarTexture.display();

	healthBarNeedsUpdate = false;
}
