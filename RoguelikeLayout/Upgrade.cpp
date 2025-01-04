#include "Upgrade.h"
#include "Player.h"
#include "Warrior.h"
#include <string>
#include <sstream>
#include <iostream>

Upgrade::Upgrade(const std::string& name, Rarity rarity, int level, const std::vector<std::string>& asciiArt, 
	const std::string& description, ElementalType elementalType, int fireDamage, ClassType classType, bool cleave) 
	: name(name), rarity(rarity), level(level), asciiArt(asciiArt), description(description), elementalType(elementalType), 
	fireDamage(fireDamage), classType(classType), cleave(cleave), purchased(false) {}

std::string Upgrade::getName() const {
	return name;
}

std::string Upgrade::getDescription() const {
	return description;
}

Rarity Upgrade::getRarity() const {
	return rarity;
}

int Upgrade::getLevel() const {
	return level;
}

sf::Color Upgrade::getColor() const {
	switch (rarity) {
	case Rarity::Common:
		return sf::Color::White;
	case Rarity::Uncommon:
		return sf::Color::Green;
	case Rarity::Rare:
		return sf::Color::Blue;
	case Rarity::Epic:
		return sf::Color::Magenta;
	case Rarity::Legendary:
		return sf::Color::Yellow;
	}
}


std::string elementalTypeToString(ElementalType type) {
	switch (type) {
	case ElementalType::None:
		return "None";
	case ElementalType::Fire:
		return "Fire";
	default:
		return "Unknown";
	}
}

void Upgrade::applyUpgrade(Player& player) const {
	if (name == "Attack Boost") {
		player.increaseAttackDamage(10);
	}
	else if (name == "Speed Boost") {
		player.increaseSpeed(5.0f);
	}
	else if (name == "Attack Speed Boost") {
		player.increaseAttackSpeed(0.02f);
	}

	// Apply elemental damage
	std::cout << "You picked up: " << elementalTypeToString(elementalType) << " elemental.\n";
	if (elementalType == ElementalType::Fire) {
		player.setElementalDamage(elementalType, fireDamage);
		std::cout << "You now have " << fireDamage << " fire damage.\n";
	}

	if (cleave) {
		static_cast<Warrior&>(player).enableCleave();
	}

}

const std::vector<std::string>& Upgrade::getAsciiArt() const {
	return asciiArt;
}

std::string Upgrade::getPositiveEffects() const {
	std::string positiveEffects;
	std::istringstream stream(description);
	std::string line;
	while (std::getline(stream, line)) {
		if (!line.empty() && line[0] == '+') {
			positiveEffects += line + "\n";
		}
	}
	return positiveEffects;
}

std::string Upgrade::getNegativeEffects() const {
	std::string negativeEffects;
	std::istringstream stream(description);
	std::string line;
	while (std::getline(stream, line)) {
		if (!line.empty() && line[0] == '-') {
			negativeEffects += line + "\n";
		}
	}
	return negativeEffects;
}

ElementalType Upgrade::getElementalType() const {
	return elementalType;
}

int Upgrade::getFireDamage() const {
	return fireDamage;
}

ClassType Upgrade::getClassType() const {
	return classType;
}

bool Upgrade::hasCleave() const {
	return cleave;
}

int Upgrade::getCost() const {
	switch (rarity) {
	case Rarity::Common:
		return 10;
	case Rarity::Uncommon:
		return 25;
	case Rarity::Rare:
		return 50;
	case Rarity::Epic:
		return 100;
	case Rarity::Legendary:
		return 200;

	}
}