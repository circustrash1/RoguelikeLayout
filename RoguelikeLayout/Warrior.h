#ifndef WARRIOR_H
#define WARRIOR_H

#include "Player.h"
#include "Enemies.h"
#include <vector>


class Warrior : public Player {
public:
	Warrior(int x, int y);
	void attack(std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map, const Room& room) override;
	void enableCleave();

	// Debug
	void disableCleave();
private:
	bool cleaveEnabled;
	float warriorAttackCooldown;
};

#endif // WARRIOR_H