#ifndef WARRIOR_H
#define WARRIOR_H

#include "Player.h"
#include "Enemies.h"
#include <vector>

class Warrior : public Player {
public:
	Warrior(int x, int y);
	void attack(std::vector<Enemy*>& enemies) override;
};

#endif // WARRIOR_H