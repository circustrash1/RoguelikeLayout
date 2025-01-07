#include "Enemies.h"
#include "Player.h"
#include "ElementalType.h"
#include "SoundManager.h"
#include "Boss.h"
#include "Knockback.h"
#include <iostream>
#include <random>

Enemy::Enemy(int x, int y, char symbol, int enemyHealth, int attackDamage, float attackCooldown, const sf::Color& color)
	: x(x), y(y), symbol(symbol), isTakingDamage(false), isTakingFireDamage(false), isDisplayingDamage(false), isDisplayingFireDamage(false),
	enemyHealth(enemyHealth), maxHealth(enemyHealth), attackDamage(attackDamage), attackCooldown(attackCooldown), alive(true), isPlayingDeathAnimation(false),
	fireDamageOverTime(0), goldAmountAdded(false), color(color) {
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

int Enemy::getHealth() const {
	return enemyHealth;
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

	if (!alive && !goldDropped) {
		std::cout << "Enemy died. Dropping gold" << std::endl;
		goldDropped = true;
		dropGold();
	}
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

	if (dynamic_cast<const Boss*>(this) != nullptr) {	// Render damage numbers above boss
		text.setPosition((x * charSize - 5) + offsetX, (y * charSize) - offsetY - 170);
	}

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

	if (dynamic_cast<const Boss*>(this) != nullptr) {	// Render damage numbers above boss
		text.setPosition((x * charSize - 5) + offsetX, (y * charSize) - offsetY - 180);
	}

	window.draw(text);
}

void Enemy::renderHealthBar(sf::RenderWindow& window, int charSize) const {
	if (dynamic_cast<const Boss*>(this) != nullptr) {	// Dont render small health bar if enemy is boss
		return;
	}

	if (!alive || isPlayingDeathAnimation) return;

	sf::Sprite healthBarSprite(healthBarTexture.getTexture());
	healthBarSprite.setPosition(x * charSize - 3, (y * charSize) + 30);
	window.draw(healthBarSprite);
}

void Enemy::playDeathAnimation(sf::RenderWindow& window) {
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

void Enemy::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) {
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

	int viewDistance = player->getStatManager().getViewDistance() - (player->getStats().wisdom * 1.75) + 5;
	float distance = std::sqrt(std::pow(playerX - x, 2) + std::pow(playerY - y, 2));
	int minOpacity = 0;
	int maxOpacity = 255;

	int opacity = maxOpacity - (distance * (maxOpacity - minOpacity) / viewDistance);
	if (distance > viewDistance) {
		opacity = minOpacity; // Set opacity to 0 if outside view distance
	}

	if (isAlive() && distance <= viewDistance) {
		text.setFillColor(sf::Color(color.r, color.g, color.b, opacity));

		if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) {        // Flash white when taking damage
			text.setFillColor(sf::Color(255, 255, 255, opacity));
		}
		else {
			text.setFillColor(sf::Color(color.r, color.g, color.b, opacity));
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

		// Only draw text if the enemy is within the view distance from the player
		text.setPosition(x * charSize, y * charSize);
		updateHealthBar(charSize);
		renderHealthBar(window, charSize);
		window.draw(text);
	}
	else if (isPlayingDeathAnimation) {
		text.setFillColor(sf::Color(color.r, color.g, color.b, opacity));
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

void Enemy::scaleAttributes(float healthFactor, float damageFactor) {
	maxHealth = static_cast<int>(maxHealth * healthFactor);
	enemyHealth = maxHealth;	// Reset current health to max health
	attackDamage = static_cast<int>(attackDamage * damageFactor);
}

// ENEMIES

Goblin::Goblin(int x, int y) : Enemy(x, y, 'G', 30, 1, 3.0f, sf::Color::Green) {}	// Initialize goblin with 30 health, 1 attack damage, and 3 second attack cooldown, green

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

void Goblin::attack(Player* player, const std::vector<std::vector<char>>& map) {
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

Orc::Orc(int x, int y) : Enemy(x, y, 'O', 60, 2, 5.0f, sf::Color(2, 48, 32)) {}	// Initialize orc with 60 health, 2 attack damage, and 5 second attack cooldown, dark green

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

void Orc::attack(Player* player, const std::vector<std::vector<char>>& map) {
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

SkeletonArcher::SkeletonArcher(int x, int y)
	: Enemy(x, y, 'S', 40, 2, 2.0f, sf::Color(169, 169, 169)), projectile(nullptr), isRetreating(false), moveCooldown(0.2f) {
	moveCooldownClock.restart();
}

void SkeletonArcher::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
	if (!alive) return;

	if (moveCooldownClock.getElapsedTime().asSeconds() < moveCooldown) {
		return;
	}
	int dx = 0, dy = 0;
	int distance = std::abs(playerX - x) + std::abs(playerY - y);

	if (distance < 3) { // If too close to the player, retreat
		if (x < playerX) dx = -1;
		else if (x > playerX) dx = 1;

		if (y < playerY) dy = -1;
		else if (y > playerY) dy = 1;

		isRetreating = true;
		attackCooldownClock.restart();
	}
	else if (distance > 5) { // If too far from the player, approach
		if (x < playerX) dx = 1;
		else if (x > playerX) dx = -1;

		if (y < playerY) dy = 1;
		else if (y > playerY) dy = -1;

		isRetreating = false;
	}
	else {
		isRetreating = false;
	}

	int newX = x + dx;
	int newY = y + dy;

	bool collision = false;
	for (const Enemy* enemy : enemies) {
		if (enemy != this && enemy->getX() == newX && enemy->getY() == newY) {
			collision = true;
			break;
		}
	}

	if (!collision && map[newY][newX] == '.') {
		x = newX;
		y = newY;
	}

	moveCooldownClock.restart();
}

void SkeletonArcher::attack(Player* player, const std::vector<std::vector<char>>& map) {
	if (isRetreating || attackCooldownClock.getElapsedTime().asSeconds() < attackCooldown) { // Cooldown between attacks
		return;
	}

	int playerX = player->getX();
	int playerY = player->getY();
	SoundManager::getInstance().playSound("skeleton_attack");
	projectile = new Projectile(x, y, playerX, playerY, attackDamage, player, this, sf::Color::Red);
	attackCooldownClock.restart();
}

void SkeletonArcher::renderProjectiles(sf::RenderWindow& window, int charSize) {
	if (projectile != nullptr) {
		projectile->update();
		projectile->render(window, charSize);
		if (!projectile->isActive()) {
			delete projectile;
			projectile = nullptr;
		}
	}
}

void SkeletonArcher::playDeathAnimation(sf::RenderWindow& window) {
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

int Enemy::getAttackDamage() const {
	return attackDamage;
}

const sf::Clock& Enemy::getAttackCooldownClock() const {
	return attackCooldownClock;
}

float Enemy::getAttackCooldown() const {
	return attackCooldown;
}

bool Enemy::isDying() const {
	return !alive && deathAnimationClock.getElapsedTime().asSeconds() < 1.0f;
}

//// Knockback mechanic
//void Enemy::applyKnockback(Player* player, const std::vector<std::vector<char>>& map) {
//	int playerX = player->getX();
//	int playerY = player->getY();
//	Knockback::applyKnockback(playerX, playerY, x, y, map, knockbackDistance);
//	player->setPosition(playerX, playerY);
//}

void Enemy::setPosition(int newX, int newY) {
	x = newX;
	y = newY;
}