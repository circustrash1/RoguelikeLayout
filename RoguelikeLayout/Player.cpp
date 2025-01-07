// Player.cpp
#include "Player.h"
#include "Map.h"
#include "SoundManager.h"
#include "Knockback.h"
#include <iostream>

Player::Player(int x, int y, char symbol, int health, int attackDmg, const Stat& stats, ClassType classType)
	: x(x), y(y), symbol(symbol), health(health), maxHealth(health), attackDmg(attackDmg), classType(classType),
	stats(stats), elementalDamage(ElementalType::None), fireDamage(0), statManager(*this), isTakingDamage(false),
	lifestealChance(5), lifestealAmount(0), baseMaxHealth(health), baseAttackDamage(attackDmg),
	glassCannonHealthModifier(1.0f), glassCannonDamageModifier(1.0f) {

}

//void Player::updateHealth() {
//	maxHealth = baseMaxHealth + (stats.constitution * 10);
//	if (health > maxHealth) {
//		health = maxHealth;
//	}
//}

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

void Player::loseHealth(int amount, Enemy* attacker) {
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

	// Apply thorns effect
	if (thornsDamage > 0 && attacker != nullptr) {
		float reflectedDamage = amount * thornsDamage;
		std::cout << "Player took " << amount << " damage. Thorns damage percentage: " << thornsDamage * 100 << "%" << std::endl;
		std::cout << "Reflected damage: " << reflectedDamage << std::endl;
		attacker->takeDamage(static_cast<int>(reflectedDamage));
	}
}

void Player::gainHealth(int amount) {
	health += amount;
	if (health > maxHealth) {
		health = maxHealth;
	}
}

int Player::getMaxHealth() {
	return maxHealth;
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

void Player::attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) {
	// Stun mechanic
	if (stunned && stunClock.getElapsedTime().asSeconds() < stunDuration) {
		return; // Prevent attack if stunned
	}
	else if (stunned) {
		clearStun(); // Clear stun if duration is over
	}
	if (attackCooldownClock.getElapsedTime().asSeconds() < attackCooldown) {
		return; // Cooldown between attacks
	}

	bool attacked = false;
	for (Enemy* enemy : enemies) {
		if (Boss* boss = dynamic_cast<Boss*>(enemy)) {
			for (const auto& part : boss->getBodyParts()) {
				if (!boss->isDying() && ((std::abs(part.first - x) <= 1 && part.second == y) || (std::abs(part.second - y) <= 1 && part.first == x))) {
					int damage = getModifiedAttackDamage();
					if (isCriticalHit()) {
						damage *= 2;
						std::cout << "Critical hit!\n";
					}
					boss->takeDamage(damage);
					if (attackCounter % 2 == 0 && elementalDamage == ElementalType::Fire) {
						int totalFireDamage = getTotalFireDamage();
						boss->applyFireDamage(totalFireDamage);
					}
					attacked = true;
					if (!boss->isAlive() && hasReapersScythe) {		// If boss is dead, apply reapers scythe damage and start clock
						applyReapersScytheDamage();
					}
					if (!boss->isAlive() && hasSoulEater) {
						applySoulEaterDamage();
					}
					if (!boss->isAlive()) {
						bossKillCount++;
					}

					applyKnockback(boss, map, room);

					break;
				}
			}
		}
		else {
			int enemyX = enemy->getX();
			int enemyY = enemy->getY();

			if (!enemy->isDying() && ((std::abs(enemyX - x) <= 1 && enemyY == y) || (std::abs(enemyY - y) <= 1 && enemyX == x))) {
				int damage = getModifiedAttackDamage();
				if (isCriticalHit()) {
					damage *= 2;
					std::cout << "Critical hit!\n";
				}
				enemy->takeDamage(damage);
				if (attackCounter % 2 == 0 && elementalDamage == ElementalType::Fire) {
					int totalFireDamage = getTotalFireDamage();
					enemy->applyFireDamage(totalFireDamage);
				}
				attacked = true;
				if (!enemy->isAlive() && hasReapersScythe) {
					applyReapersScytheDamage();	// If enemy is dead, apply reapers scythe damage and start clock
				}
				if (!enemy->isAlive() && hasSoulEater) {
					applySoulEaterDamage();
				}
				if (!enemy->isAlive()) {
					totalKillCount++;
				}

				applyKnockback(enemy, map, room);
				
				break;
			}
		}
	}

	if (attacked) {
		// Lifesteal mechanic
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(1, 100);
		if (dist(rng) <= lifestealChance) {
			gainHealth(lifestealAmount);
			std::cout << "Lifesteal! Gained " << lifestealAmount << " health." << std::endl;
		}

		SoundManager::getInstance().playSound("player_attack");
		attackCooldownClock.restart();

		attackCounter++;
		if (attackCounter % 5 == 0 && hasLuckyFive) {
			applyGuaranteedCrit();
		}
		else {
			guaranteedCrit = false;
		}
	}
}

bool Player::isAdjacent(Enemy* enemy) const {
	int dx = std::abs(enemy->getX() - x);
	int dy = std::abs(enemy->getY() - y);
	return (dx + dy == 1); // Check if the enemy is adjacent
}

void Player::move(char direction, const std::vector<std::vector<char>>& map, const std::vector<Enemy*>& enemies, const std::vector<Room>& rooms) {
	// Stun mechanic
	if (stunned && stunClock.getElapsedTime().asSeconds() < stunDuration) {
		return;
	}
	else if (stunned) {
		clearStun();
	}

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
	if (newX >= 0 && newX < map[0].size() && newY >= 0 && newY < map.size() && map[newY][newX] == '.'
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
	baseAttackDamage += amount;
	recalculateStats();
}

void Player::increaseMaxHealth(int amount) {
	baseMaxHealth += amount;
	recalculateStats();
	std::cout << "Increasing max health by " << amount << " to " << maxHealth << std::endl;
}

void Player::increaseSpeed(float amount) {
	speed += amount;
}

void Player::increaseAttackSpeed(float amount) {
	float diminishingFactor = 0.5f / (1.0f + std::exp(attackCooldown - 1.0f));
	attackCooldown -= amount * diminishingFactor;
	if (attackCooldown <= 0.1f) {
		attackCooldown = 0.1f;
	}
}

void Player::increaseLifesteal(float amount) {
	lifestealAmount += amount;
}

void Player::applyGlassCannon() {
	glassCannonHealthModifier = 0.75f;
	glassCannonDamageModifier = 1.30f;
	recalculateStats();
}

void Player::applyThorns(int amount) {
	float diminishingFactor = 1.0f / (1.0f + thornsDamage);
	thornsDamage += (amount * diminishingFactor) / 100;
}

void Player::applyCritChance(int amount) {
	if (critChance < 0.5f) {
		// Calculate the maximum possible increase, which diminishes as critChance approaches 0.5
		float maxIncrease = 0.1f * (0.5f - critChance);  // A diminishing increase based on how close we are to 0.5
		float increase = std::min(amount * 0.01f, maxIncrease);  // Apply the limit to the increase

		critChance += increase;

		// Ensure critChance doesn't exceed 0.5
		if (critChance > 0.5f) {
			critChance = 0.5f;
		}
	}
	std::cout << "Crit chance: " << critChance << std::endl;
}

bool Player::isCriticalHit() const {
	if (guaranteedCrit) {
		return true;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	return dis(gen) < critChance;
}

void Player::applyDyingRage(int amount) {
	dyingRageDamage += amount;
}

int Player::getModifiedAttackDamage() const {
	int modifiedAttackDamage = attackDmg + reapersScytheBonusDamage + soulEaterBonusDamage;
	if (health <= maxHealth * 0.25f) {
		modifiedAttackDamage += dyingRageDamage;
	}
	return modifiedAttackDamage;
}

void Player::applyLuckyFive() {
	hasLuckyFive = true;
}

void Player::applyGuaranteedCrit() {
	guaranteedCrit = true;
	std::cout << "Guaranteed critical hit on the next attack!" << std::endl;
}

void Player::applyReapersScythe() {
	hasReapersScythe = true;
}

void Player::applyReapersScytheDamage() {
	reapersScytheBonusDamage += 5;
	reapersScytheClock.restart();
	std::cout << "Reaper's Scythe activated! Bonus damage: " << reapersScytheBonusDamage << std::endl;
}

void Player::resetReapersScythe() {
	reapersScytheBonusDamage = 0;
	std::cout << "Reaper's Scythe bonus damage set to 0" << std::endl;
}

void Player::updateReapersScythe() {
	if (reapersScytheClock.getElapsedTime().asSeconds() >= 5.0f) {
		resetReapersScythe();
	}
}

void Player::applySoulEater() {
	hasSoulEater = true;
}

void Player::applySoulEaterDamage() {
	soulEaterBonusDamage++;
	killCount++;
	std::cout << "+1 Kill, +1 Damage. Total Soul Eater damage: " << soulEaterBonusDamage << std::endl;
}

void Player::applyShockwave() {
	knockbackDistance++;
}

void Player::recalculateStats() {
	float healthPercentage = static_cast<float>(health) / maxHealth;
	maxHealth = static_cast<int>(baseMaxHealth * glassCannonHealthModifier);
	attackDmg = static_cast<int>(baseAttackDamage * glassCannonDamageModifier);
	health = static_cast<int>(maxHealth * healthPercentage);
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

void Player::update(const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) {
	if (hasReapersScythe) {
		updateReapersScythe();		// Update to track reapers scythe clock
	}

    for (Enemy* enemy : enemies) {
        if (enemy->getAttackCooldownClock().getElapsedTime().asSeconds() >= enemy->getAttackCooldown()) {
            int enemyX = enemy->getX();
            int enemyY = enemy->getY();
            if ((std::abs(enemyX - x) <= 1 && std::abs(enemyY - y) <= 1)) {
                SoundManager::getInstance().playSound("goblin_attack");
                loseHealth(enemy->getAttackDamage(), enemy);
                const_cast<sf::Clock&>(enemy->getAttackCooldownClock()).restart();
                break;
            }
        }
        enemy->updateFireDamage();

        // Check if the enemy is a SkeletonArcher and within attack range
        if (SkeletonArcher* archer = dynamic_cast<SkeletonArcher*>(enemy)) {
            int distance = std::abs(archer->getX() - x) + std::abs(archer->getY() - y);
            if (distance <= 5) { // Adjust the range as needed
                archer->attack(this, map);
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
	else if (stunned & stunClock.getElapsedTime().asSeconds() < stunDuration) {
		text.setFillColor(sf::Color::Yellow);
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

bool Player::isInBossRoom(const std::vector<Room>& rooms) const {
	for (const Room& room : rooms) {
		if (room.isBossRoom && x >= room.startX && x <= room.endX && y >= room.startY && y <= room.endY) {
			return true;
		}
	}
	return false;
}

bool Player::isInBossCorridor(const std::vector<Room>& rooms) const {
	for (const Room& room : rooms) {
		if (room.isBossRoom && !room.bossRoomEntered && x == room.startX + 2) {
			return true;
		}
	}
	return false;
}

// Stun mechanic
void Player::applyStun(float duration) {
	stunned = true;
	stunDuration = duration;
	stunClock.restart();
}

void Player::clearStun() {
	stunned = false;
	stunDuration = 0.0f;
}

bool Player::isStunned() const {
	return stunned;
}

// Knockback mechanic
void Player::applyKnockback(Enemy* enemy, const std::vector<std::vector<char>>& map, const Room& room) {
	int enemyX = enemy->getX();
	int enemyY = enemy->getY();
	int originalX = enemyX;
	int originalY = enemyY;

	if (Boss* boss = dynamic_cast<Boss*>(enemy)) {
		Knockback::applyKnockback(enemyX, enemyY, x, y, map, knockbackDistance, room.startX, room.startY, room.endX, room.endY, boss->getBodyParts());
		int deltaX = enemyX - originalX;
		int deltaY = enemyY - originalY;
		boss->updateBodyParts(deltaX, deltaY);
	}
	else {
		Knockback::applyKnockback(enemyX, enemyY, x, y, map, knockbackDistance, room.startX, room.startY, room.endX, room.endY, {});
	}

	enemy->setPosition(enemyX, enemyY);
}


StatManager& Player::getStatManager() {
	return statManager;
}