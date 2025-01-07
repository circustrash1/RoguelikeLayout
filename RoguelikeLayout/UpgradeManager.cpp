#include "UpgradeManager.h"

UpgradeManager::UpgradeManager() {
	// Initialize all upgrades
	allUpgrades = {
		// UNIVERSAL UPGRADES
		Upgrade("Attack Boost", Rarity::Legendary, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|O |o.o|  |",
			"|   \\v/   |",
			"|  XXOXX  |",
			"|   /^\\   |",
			"|  |o'o| O|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+10 Attack Damage", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Speed Boost", Rarity::Epic, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|# %*,*%  |",
			"|  \\_o_/  |",
			"| -=<*>=- |",
			"|  /~o~\\  |",
			"|  %*'*% #|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+10 Move Speed \n-10 Attack Speed -Doesn't do anything yet", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Defense Boost", Rarity::Rare, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+5 % Block Chance \n-10 Attack Damage -Doesn't do anything yet", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Attack Speed Boost", Rarity::Common, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+10 Attack Speed", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Thorns", Rarity::Uncommon, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+10% Return Damage", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Sharp Focus", Rarity::Uncommon, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+5% Crit Chance", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Dying Rage", Rarity::Rare, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+10 Damage when under 25% HP", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Lucky Five", Rarity::Rare, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+Guaranteed crit every 5th attack", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Reaper's Scythe", Rarity::Epic, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+5 Damage on kill \n-Resets after 5 seconds without a kill", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Soul Eater", Rarity::Legendary, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+1 Permanent damage on kill", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Shockwave", Rarity::Epic, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+1 Knockback Distance", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Glass Cannon", Rarity::Epic, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+30% Attack Damage \n-25% Max Health", ElementalType::None, 0, ClassType::Universal),

		Upgrade("Life Leech", Rarity::Rare, 1, {
			" _________ ",
			"|Q |~~~|  |",
			"|+ /o,o\\  |",
			"|  \\_-_/  |",
			"| _-~+_-~ |",
			"|  /~-~\\  |",
			"|  \\o`o/ +|",
			"|  |___| Q|",
			" ~~~~~~~~~ "
		}, "+1HP on a lifesteal hit (5% chance)", ElementalType::None, 0, ClassType::Universal),

			// WARRIOR SPECIFIC UPGRADES

			Upgrade("Fire Sword", Rarity::Uncommon, 1, {
				" _________ ",
				"|Q |~~~|  |",
				"|+ /o,o\\  |",
				"|  \\_-_/  |",
				"| _-~+_-~ |",
				"|  /~-~\\  |",
				"|  \\o`o/ +|",
				"|  |___| Q|",
				" ~~~~~~~~~ "
			}, "+1 Fire Damage", ElementalType::Fire, 1, ClassType::Warrior),

			Upgrade("Cleave", Rarity::Uncommon, 1, {
				" _________ ",
				"|Q |~~~|  |",
				"|+ /o,o\\  |",
				"|  \\_-_/  |",
				"| _-~+_-~ |",
				"|  /~-~\\  |",
				"|  \\o`o/ +|",
				"|  |___| Q|",
				" ~~~~~~~~~ "
			}, "+50% damage to adjacent units", ElementalType::None, 0, ClassType::Warrior, true), // Enabling cleave for this upgrade

			// MAGE SPECIFIC UPGRADES

			Upgrade("Fireball", Rarity::Common, 1, {
				" _________ ",
				"|Q |~~~|  |",
				"|+ /o,o\\  |",
				"|  \\_-_/  |",
				"| _-~+_-~ |",
				"|  /~-~\\  |",
				"|  \\o`o/ +|",
				"|  |___| Q|",
				" ~~~~~~~~~ "
			}, "+1 Fire Damage", ElementalType::Fire, 1, ClassType::Mage),

			// Add more upgrades
	};
	rng.seed(std::random_device()());
}

Rarity UpgradeManager::getRandomRarity() const {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	double randomValue = dis(gen);

	if (randomValue < 0.5) {	// 0 - 0.5 = common
		return Rarity::Common;	// 50% chance
	}
	else if (randomValue < 0.8) {	// 0.5 - 0.8 = uncommon
		return Rarity::Uncommon;	// 30% chance
	}
	else if (randomValue < 0.95) {	// 0.8 - 0.95 = rare
		return Rarity::Rare;	// 15% chance
	}
	else if (randomValue < 0.9975) {	// 0.95 - 0.99 = epic
		return Rarity::Epic;	// 5% chance
	}
	else {
		return Rarity::Legendary;	// 1% chance
	}
}

Upgrade UpgradeManager::generateUpgrade(Rarity rarity, ClassType classType) const {
	std::vector<Upgrade> filteredUpgrades;

	// Keep rolling until we find a valid upgrade
	while (filteredUpgrades.empty()) {
		// Filter upgrades by the selected rarity and class type
		for (const auto& upgrade : allUpgrades) {
			if (upgrade.getRarity() == rarity && (upgrade.getClassType() == classType || upgrade.getClassType() == ClassType::Universal)) {
				filteredUpgrades.push_back(upgrade);
			}
		}

		// If no upgrades found, roll for a new rarity
		if (filteredUpgrades.empty()) {
			rarity = getRandomRarity();
		}
	}

	// Select a random upgrade from the filtered list
	std::uniform_int_distribution<int> dist(0, filteredUpgrades.size() - 1);
	return filteredUpgrades[dist(rng)];
}

std::vector<Upgrade> UpgradeManager::generateUpgrades(int count, ClassType classType) const {
	std::vector<Upgrade> upgrades; // Store generated upgrades
	std::set<std::string> uniqueNames; // Set to avoid duplicates

	while (upgrades.size() < count) {
		Rarity rarity = getRandomRarity();
		Upgrade upgrade = generateUpgrade(rarity, classType);
		if (uniqueNames.find(upgrade.getName()) == uniqueNames.end()) {
			uniqueNames.insert(upgrade.getName());
			upgrades.push_back(upgrade);
		}
	}
	return upgrades;
}