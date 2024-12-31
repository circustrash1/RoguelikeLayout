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

class Game {
public:
    Game();
    ~Game();
    void run();

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

    void displayCharacterSelection();
    Player* createPlayer(const std::string& className);
    void updateHealth();
    HealthBar* healthBar;
    ShowStats* showStats;

    // Screen shake
    void screenShake(float intensity, float duration);
    sf::View originalView; // Store original view
    sf::Clock shakeClock;
    bool isShaking;
    float shakeIntensity;

    void processEvents();
    void update();
    void render();
};

#endif // GAME_H