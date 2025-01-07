#ifndef EVENTMAPS_H
#define EVENTMAPS_H

#include <vector>
#include <random>
#include "Map.h"

struct Room;


class EventMaps {
public:
	static void convertToMerchantRoom(Room& room);
	static void convertToEventRoom(Room& room);

	static void handleMerchantRoom(Room& room, Player& player);
	static void handleEventRoom(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize);

	static bool checkMerchantInteraction(Room& room, Player& player);  // Merchant window is handled in game.cpp
	static void checkEventInteraction(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize);

	static void displayMerchantWindow(Room* room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize);
	static void renderSkillCheckText(Room& room, Player& player, sf::RenderWindow& window, const sf::Font& font, int charSize);

	static std::string upgradeName;
	static sf::Color upgradeColor;

	static const bool debug = true;
};

#endif // EVENTMAPS_H