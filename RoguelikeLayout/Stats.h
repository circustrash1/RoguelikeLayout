#ifndef STATS_H
#define STATS_H

#include <string>

struct Stat {
	int strength;
	int dexterity;
	int constitution;
	int intelligence;
	int wisdom;
	int charisma;
};

class Player; // Forward declaration

Stat generateRandomStats(int strSkew = 0, int dexSkew = 0, int conSkew = 0, int intSkew = 0, int wisSkew = 0, int chaSkew = 0);

int rollDice(int sides);
bool skillCheck(int stat, int difficulty);
void modifyStat(Stat& stats, const std::string& statName, int amount, Player& player);

#endif // STATS_H