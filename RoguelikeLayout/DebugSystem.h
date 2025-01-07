#ifndef DEBUGSYSTEM_H
#define DEBUGSYSTEM_H
#include "Player.h"
#include "Game.h"
#include <SFML/Window.hpp>

class Game;

class DebugSystem {
public:
    DebugSystem(Player& player, Game& game);
    void handleInput();
    void update();

private:
    Player& player;
    Game& game;
    bool debugMode;
    bool upgrade1Pressed; // Track the state of the ; key
    bool upgrade2Pressed; // Track the state of the ' key
	bool refreshShopPressed; // Track the state of the R key
    void toggleDebugMode();
    void applyAbility(const std::string& ability);
    void removeAbility(const std::string& ability);
    void increaseHealth(int amount);
    void decreaseHealth(int amount);
    void refreshShop();
};

#endif // DEBUGSYSTEM_H

