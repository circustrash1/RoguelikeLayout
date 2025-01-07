#include "Stats.h"
#include "Player.h"
#include <cstdlib>
#include <ctime>
#include <random>

int rollDice(int sides) {
	return std::rand() % sides + 1;
}

std::pair<int, bool> skillCheck(int stat, int difficulty) {
	int roll = rollDice(20);
    bool success = (roll + stat) >= difficulty;
	return { roll, success };
}

void modifyStat(Stat& stats, const std::string& statName, int amount, Player& player) {
    if (statName == "strength") {
        stats.strength += amount;
        player.updateAttackDamage();
    }
    else if (statName == "dexterity") {
        stats.dexterity += amount;
    }
    else if (statName == "constitution") {
        stats.constitution += amount;
        player.increaseMaxHealth(amount * 10);
    }
    else if (statName == "intelligence") {
        stats.intelligence += amount;
    }
    else if (statName == "wisdom") {
        stats.wisdom += amount;
    }
    else if (statName == "charisma") {
        stats.charisma += amount;
    }
}

Stat generateRandomStats(int strSkew, int dexSkew, int conSkew, int intSkew, int wisSkew, int chaSkew) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(8, 15); // Generate stats between 8 and 15

	Stat stats;
	stats.strength = dis(gen) + strSkew;
	stats.dexterity = dis(gen) + dexSkew;
	stats.constitution = dis(gen) + conSkew;
	stats.intelligence = dis(gen) + intSkew;
	stats.wisdom = dis(gen) + wisSkew;
	stats.charisma = dis(gen) + chaSkew;

	return stats;
}