#ifndef GAME_H
#define GAME_H

#include "DebugSystem.h"
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Map.h"
#include "Player.h"
#include "Warrior.h"
#include "Mage.h"
#include "GuiElements.h"
#include "BossRoom.h"
#include <vector>
#include <iostream>

class DebugSystem;

class Game {
public:
	Game();
	~Game();
	void run();
	void handleUpgradePickup();
	void handleInput();
	void initialize();

	void updateHealth();

	void renderGoldDrops(sf::RenderWindow& window);
	void clearGoldDrops();

	int getStageCount() const;
	void incrementStageCount();

	void displayUpgradeSelection();
	void displayCollectedUpgrades();

	sf::Clock shopCloseClock;
	bool isShopWindowOpen;

	void restartGame();

	void enterBossRoom();

	Room* getCurrentRoom();

	std::pair<int, int> getPlayerKillCount();

	// Cheats
	void modifyPlayerGold(int amount);
	void refreshShop();

private:
	sf::RenderWindow window;
	sf::Font font;
	std::vector<Button*> buttons;
	sf::Text healthText;
	sf::RectangleShape mainBar;
	sf::RectangleShape sideBar;
	Map map;
	Player* player; 
	int playerX;
	int playerY;
	float scaleX; // Add scaleX
	float scaleY; // Add scaleY
	int stageCount;

	BossRoom bossRoom;

	void displayCharacterSelection();
	Player* createPlayer(const std::string& className);

	void displayMerchantWindow(Room& room);
	void renderUpgradeOrMerchantWindow(const std::string& titleText, const std::vector<Upgrade>& upgrades, bool isMerchantShop);

	HealthBar* healthBar;
	ShowStats* showStats;
	ShowGold* showGold;
	ShowStage* showStage;

	std::vector<std::tuple<int, int, int>> goldDrops;   // Store x, y and gold amount

	// Screen shake
	void screenShake(float intensity, float duration);
	sf::View originalView; // Store original view
	sf::Clock shakeClock;
	bool isShaking;
	float shakeIntensity;

	std::vector<Upgrade> availableUpgrades;

	void processEvents();
	void update(float deltaTime);
	void render();

	// Debug
	DebugSystem* debugSystem;
};

#endif // GAME_H