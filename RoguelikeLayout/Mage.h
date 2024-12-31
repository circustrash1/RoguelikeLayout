#ifndef MAGE_H
#define MAGE_H

#include "Player.h"
#include "Enemies.h"
#include <vector>

class Mage : public Player {
public:
	Mage(int x, int y);
	void attack(std::vector<Enemy*>& enemies) override;
};

#endif // MAGE_H