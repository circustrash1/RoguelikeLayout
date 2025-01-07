#ifndef MAGE_H
#define MAGE_H

#include "Player.h"
#include "Enemies.h"
#include "Projectile.h"
#include <vector>

class Mage : public Player {
public:
	Mage(int x, int y);
	~Mage() override = default;
	void attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) override;
	void renderProjectile(sf::RenderWindow& window, int charSize);

private:
	Projectile* projectile;

	float mageAttackCooldown;
	float maxRange;
};

#endif // MAGE_H