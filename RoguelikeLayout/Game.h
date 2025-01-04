#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Map.h"
#include "Player.h" // Include Player header
#include "Warrior.h" // Include Warrior header"
#include "Mage.h"
#include "GuiElements.h"
#include <vector>
#include <iostream>

class Game {
public:
    Game();
    ~Game();
    void run();
    void handleUpgradePickup();
    void initialize();

    void renderGoldDrops(sf::RenderWindow& window);
    void clearGoldDrops();

    int getStageCount() const;
	void incrementStageCount();

    void displayCollectedUpgrades();


    void restartGame();

    // Cheats
    void modifyPlayerGold(int amount);

private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<Button*> buttons;
    sf::Text healthText;
    sf::RectangleShape mainBar;
    sf::RectangleShape sideBar;
    Map map;
    Player* player; // Add player as a member of Game class
    int playerX;
    int playerY;
    float scaleX; // Add scaleX
    float scaleY; // Add scaleY
    int stageCount;

    void displayCharacterSelection();
    Player* createPlayer(const std::string& className);
    void displayUpgradeSelection();
    void displayMerchantWindow(Room& room);
    void renderUpgradeOrMerchantWindow(const std::string& titleText, const std::vector<Upgrade>& upgrades, bool isMerchantShop);
    void updateHealth();


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
    void update();
    void render();
};




#endif // GAME_H