#ifndef UPGRADE_H
#define UPGRADE_H


#include "ElementalType.h"
#include "Player.h"
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

// Forward declaration
class Player;

enum class ClassType {
	Warrior,
	Mage,
	Universal
	// Add more classes here
};

enum class Rarity {
	Common,
	Uncommon,
	Rare,
	Legendary,
	Epic
};

class Upgrade {
public:
	Upgrade(const std::string& name, Rarity rarity, int level, const std::vector<std::string>& asciiArt, const std::string& description,
		ElementalType elementalType = ElementalType::None, int fireDamage = 0, ClassType classType = ClassType::Universal, bool cleave = false);

	std::string getName() const;
	std::string getDescription() const;
	Rarity getRarity() const;
	int getLevel() const;
	void applyUpgrade(Player& player) const;
	const std::vector<std::string>& getAsciiArt() const;
	sf::Color getColor() const;
	std::string getPositiveEffects() const;
	std::string getNegativeEffects() const;
	int getCost() const;

	// Track purchased
	bool isPurchased() const { return purchased; }
	void setPurchased(bool value) { purchased = value; }

	// Elemental Damage
	ElementalType getElementalType() const;
	int getFireDamage() const;

	ClassType getClassType() const;

	// Upgrades
	bool hasCleave() const;

private:
	std::string name;
	Rarity rarity;
	int level;
	std::vector<std::string> asciiArt;
	std::string description;
	ElementalType elementalType;
	int fireDamage;
	ClassType classType;
	bool cleave;
	bool purchased;
	// Add more attributes for different effects
};



#endif // UPGRADE_H