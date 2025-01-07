#include "EventMaps.h"
#include "Map.h"
#include "Stats.h"
#include "SharedData.h"
#include "Game.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

extern SharedData sharedData;
extern Game game;

// Define the static member variable
std::string EventMaps::upgradeName = "";
sf::Color EventMaps::upgradeColor = sf::Color::White;

void EventMaps::convertToMerchantRoom(Room& room) {
	room.enemiesSpawned = false;
	room.upgradeSpawned = false;
	room.upgradeCollected = false;
	room.isMerchantRoom = true;
}

void EventMaps::convertToEventRoom(Room& room) {
	room.enemiesSpawned = false;
	room.upgradeSpawned = false;
	room.upgradeCollected = false;
	room.isEventRoom = true;
}

void displayMessage(sf::RenderWindow& window, const std::string& message, const sf::Font& font) {
	sf::Text text;
	text.setFont(font);
	text.setString(message);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setPosition(50, 300);

	window.draw(text);
	window.display();

	sf::sleep(sf::seconds(2));
}

void displayDiceRoll(sf::RenderWindow& window, int roll, int statValue, const std::string& statName, const sf::Font& font) {
	std::vector<std::string> parts = {
		"Dice Roll : " + std::to_string(roll),
		" + " + std::to_string(statValue) + " " + statName,
		" = " + std::to_string(roll + statValue)
	};

	sf::Text text;
	std::string fullText;
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setPosition(300, 200);

	for (const auto& part : parts) {
		fullText += part;
		text.setString(fullText);
		window.clear();
		window.draw(text);
		window.display();
		sf::sleep(sf::seconds(1));
	}
}

void displayResult(sf::RenderWindow& window, bool success, const std::string& effectDescription, const sf::Font& font, const std::string& upgradeName = "", const sf::Color& upgradeColor = sf::Color::White) {
	sf::Text text;
	text.setFont(font);
	std::string resultMessage = success ? "Success!\n" + effectDescription : "Failure!\n" + effectDescription;
	if (success && !upgradeName.empty()) {
		resultMessage += "\n\n+1 ";
		text.setString(resultMessage);
		text.setCharacterSize(24);
		text.setFillColor(success ? sf::Color::Green : sf::Color::Red);
		text.setPosition(300, 300);

		window.clear();
		window.draw(text);

		// Draw the upgrade name with the specified color
		sf::Text upgradeText;
		upgradeText.setFont(font);
		upgradeText.setString(upgradeName);
		upgradeText.setCharacterSize(24);
		upgradeText.setFillColor(upgradeColor);

		// Calculate the position of the upgrade name
		sf::FloatRect textBounds = text.getGlobalBounds();
		upgradeText.setPosition(textBounds.left + textBounds.width - 550, textBounds.top + textBounds.height - text.getCharacterSize());

		window.draw(upgradeText);
	}
	else {
		text.setString(resultMessage);
		text.setCharacterSize(24);
		text.setFillColor(success ? sf::Color::Green : sf::Color::Red);
		text.setPosition(300, 300);

		window.clear();
		window.draw(text);
	}

	window.display();

	sf::sleep(sf::seconds(2));
}

void EventMaps::handleMerchantRoom(Room& room, Player& player) {
	if (!room.merchantSpawned) {
		std::cout << "Merchant spawned in room at (" << room.startX << ", " << room.startY << ")" << std::endl;
		int centerX = (room.startX + room.endX) / 2;
		int centerY = (room.startY + room.endY) / 2;
		room.merchantPosition = { centerX, centerY };
		room.merchantSpawned = true;
	}
}

void EventMaps::handleEventRoom(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize) {
	if (room.eventTriggered) {
		return;
	}

	std::vector<std::string> events = {
		"You found a locked chest!",
		"You found an ancient library!",
		"You found a mystical fountain!",
		"You discovered a statue of riddles!"
	};

	std::string eventText;

	if (debug) {
		eventText = "You discovered a statue of riddles!";
	}
	else {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, events.size() - 1);
		int eventIndex = dist(rng);
		eventText = events[eventIndex];
	}

	std::cout << eventText << std::endl;

	room.eventText = eventText;
	room.eventTextClock.restart();
	room.eventTextDuration = sf::seconds(3);

	int centerX = (room.startX + room.endX) / 2;
	int centerY = (room.startY + room.endY) / 2;

	room.eventCharVisible = true;
	room.eventCharPosition = { centerX, centerY };

	room.eventTriggered = true;
	room.eventText = eventText;
}

void EventMaps::checkEventInteraction(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize) {
	if (!room.eventTriggered || room.eventResolved) {
		return;
	}

	int playerX = player.getX();
	int playerY = player.getY();
	int charX = (room.startX + room.endX) / 2;
	int charY = (room.startY + room.endY) / 2;

	bool isNearEventChar = ((std::abs(playerX - charX) == 1 && playerY == charY) || (std::abs(playerY - charY) == 1 && playerX == charX));

	if (isNearEventChar && (sf::Keyboard::isKeyPressed(sf::Keyboard::E))) {
		window.clear();

		bool success = false;
		int roll = 0;
		int statValue = 0;
		std::string statName;
		std::string effectDescription;

		if (room.eventText == "You found a locked chest!") {
			statValue = player.getStats().dexterity;
			statName = "DEX";
			std::tie(roll, success) = skillCheck(statValue, 15);
			effectDescription = success ? "+1 Free Card" : "-10 Health";
		}
		else if (room.eventText == "You found an ancient library!") {
			statValue = player.getStats().intelligence;
			statName = "INT";
			std::tie(roll, success) = skillCheck(statValue, 15);
			effectDescription = success ? "+1 Intelligence" : "-10 Health";
		}
		else if (room.eventText == "You found a mystical fountain!") {
			statValue = player.getStats().wisdom;
			statName = "WIS";
			std::tie(roll, success) = skillCheck(statValue, 15);
			effectDescription = success ? "+10 Health" : "-10 Health";
		}
		else if (room.eventText == "You discovered a statue of riddles!") {
			statValue = player.getStats().intelligence;
			statName = "INT";
			std::tie(roll, success) = skillCheck(statValue, 1);
			effectDescription = success ? "+1 Random Existing Upgrade" : "-10 Health";
		}



		if (success) {
			if (room.eventText == "You found a locked chest!") {
				game.handleUpgradePickup();
			}
			else if (room.eventText == "You found an ancient library!") {
				modifyStat(player.getMutableStats(), "intelligence", 1, player);
			}
			else if (room.eventText == "You found a mystical fountain!") {
				player.gainHealth(10);
			}
			else if (success && room.eventText == "You discovered a statue of riddles!") {
				// Re-apply a random upgrade that the player already has
				auto& upgrades = player.getMutableCollectedUpgrades();
				if (!upgrades.empty()) {
					std::default_random_engine rng(std::random_device{}());
					std::uniform_int_distribution<int> dist(0, upgrades.size() - 1);
					int upgradeIndex = dist(rng);
					Upgrade randomUpgrade = upgrades[upgradeIndex];
					player.applyUpgrade(randomUpgrade);
					upgradeName = randomUpgrade.getName();
					upgradeColor = randomUpgrade.getColor(); // Assuming getColor() returns the color based on rarity
				}
			}
		}
		else {
			if (room.eventText == "You found a locked chest!") {
				player.loseHealth(10, nullptr);
			}
			else if (room.eventText == "You found an ancient library!") {
				player.loseHealth(10, nullptr);
			}
			else if (room.eventText == "You found a mystical fountain!") {
				player.loseHealth(10, nullptr);
			}
			else if (room.eventText == "You discovered a statue of riddles!") {
				player.loseHealth(10, nullptr);
			}
		}

		displayDiceRoll(window, roll, statValue, statName, font);
		displayResult(window, success, effectDescription, font, upgradeName, upgradeColor);

		window.clear();

		window.display();

		room.eventResolved = true;
		room.eventCharVisible = false;
		sharedData.relevantStat.clear();
	}
	else if (!isNearEventChar) {
		sharedData.relevantStat.clear();
	}
}

bool EventMaps::checkMerchantInteraction(Room& room, Player& player) {
	if (!room.merchantSpawned) {
		return false;
	}
	int playerX = player.getX();
	int playerY = player.getY();
	int merchantX = room.merchantPosition.first;
	int merchantY = room.merchantPosition.second;
	bool isNearMerchant = ((std::abs(playerX - merchantX) == 1 && playerY == merchantY) || (std::abs(playerY - merchantY) == 1 && playerX == merchantX));
	if (isNearMerchant && (sf::Keyboard::isKeyPressed(sf::Keyboard::E))) {
		return true;
	}
	return false;
}

void EventMaps::renderSkillCheckText(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize) {
	if (!room.eventTriggered || room.eventResolved) {
		return;
	}

	int playerX = player.getX();
	int playerY = player.getY();
	int charX = (room.startX + room.endX) / 2;
	int charY = (room.startY + room.endY) / 2;

	// Calculate distance between player and eventChar
	int distance = std::abs(playerX - charX) + std::abs(playerY - charY);

	if (distance <= 2) {
		// Display skill check text
		sf::Text skillCheckText;
		skillCheckText.setFont(font);
		skillCheckText.setCharacterSize(12);
		skillCheckText.setFillColor(sf::Color::White);
		skillCheckText.setLineSpacing(2);
		skillCheckText.setPosition(charX * charSize - 100, (charY - 3) * charSize);

		std::string skillCheckMsg = "Skill Check Required: ";
		if (room.eventText == "You found a locked chest!") {
			skillCheckMsg += "Dexterity\nSuccess: +1 Dexterity\nFailure: -10 Health";
			sharedData.relevantStat = "DEX";
		}
		else if (room.eventText == "You found an ancient library!") {
			skillCheckMsg += "Intelligence\nSuccess: +1 Intelligence\nFailure: -10 Health";
			sharedData.relevantStat = "INT";
		}
		else if (room.eventText == "You found a mystical fountain!") {
			skillCheckMsg += "Wisdom\nSuccess: +10 Health\nFailure: -10 Health";
			sharedData.relevantStat = "WIS";
		}
		else if (room.eventText == "You discovered a statue of riddles!") {
			skillCheckMsg += "Intelligence\nSuccess: +1 Random Existing Upgrade\nFailure: -10 Health";
			sharedData.relevantStat = "INT";
		}

		skillCheckText.setString(skillCheckMsg);
		window.draw(skillCheckText);
	}
}