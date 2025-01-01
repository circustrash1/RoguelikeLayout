#ifndef UPGRADEMANAGER_H
#define UPGRADEMANAGER_H

#include "Upgrade.h"
#include <vector>
#include <random>
#include <set>

class UpgradeManager {
public:
	UpgradeManager();
	~UpgradeManager() = default;
	std::vector<Upgrade> generateUpgrades(int count, ClassType classType) const;

private:
	Rarity getRandomRarity() const;
	Upgrade generateUpgrade(Rarity rarity, ClassType classType) const;

	std::vector<Upgrade> allUpgrades;
	mutable std::default_random_engine rng;
};

#endif // UPGRADEMANAGER_H