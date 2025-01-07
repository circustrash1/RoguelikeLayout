#include "DebugSystem.h"
#include <iostream>
#include "Warrior.h"

DebugSystem::DebugSystem(Player& player, Game& game) : player(player), game(game), debugMode(false), upgrade1Pressed(false), upgrade2Pressed(false), refreshShopPressed(false) {}

void DebugSystem::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
        toggleDebugMode();
    }

    if (debugMode) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
            applyAbility("FireDamage");
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
            removeAbility("FireDamage");
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen)) {
            applyAbility("Cleave");
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal)) {
            removeAbility("Cleave");
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            increaseHealth(10);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            decreaseHealth(10);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Semicolon)) {
            if (!upgrade1Pressed) {
                applyAbility("Upgrade1");
                upgrade1Pressed = true;
            }
        }
        else {
            upgrade1Pressed = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            if (!upgrade2Pressed) {
                applyAbility("Upgrade2");
                upgrade2Pressed = true;
            }
        }
        else {
            upgrade2Pressed = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            if (!refreshShopPressed) {
                game.refreshShop();
                refreshShopPressed = true;
            }
        }
        else {
            refreshShopPressed = false;
        }
    }
}

void DebugSystem::update() {
    // Update logic if needed
}

void DebugSystem::toggleDebugMode() {
    debugMode = !debugMode;
    std::cout << "Debug mode " << (debugMode ? "enabled" : "disabled") << std::endl;
}

void DebugSystem::applyAbility(const std::string& ability) {
    if (&player != nullptr) {
        if (ability == "FireDamage") {
            player.setElementalDamage(ElementalType::Fire, 10); // Example value
        }
        else if (ability == "Cleave") {
            if (Warrior* warrior = dynamic_cast<Warrior*>(&player)) {
                warrior->enableCleave();
                std::cout << "Cleave enabled" << std::endl;
            }
        }
        else if (ability == "Upgrade1") {
            Upgrade upgrade1("Shockwave", Rarity::Epic, 1, {
            " _________ ",
            "|Q |~~~|  |",
            "|+ /o,o\\  |",
            "|  \\_-_/  |",
            "| _-~+_-~ |",
            "|  /~-~\\  |",
            "|  \\o`o/ +|",
            "|  |___| Q|",
            " ~~~~~~~~~ "
                }, "+1 Knockback Distance", ElementalType::None, 0, ClassType::Universal);
            player.applyUpgrade(upgrade1);
            std::cout << "Upgrade 1 applied" << std::endl;
        }
        else if (ability == "Upgrade2") {
            Upgrade upgrade2("Sharp Focus", Rarity::Common, 1, {}, "+10% Crit Chance", ElementalType::None, 0, ClassType::Universal);
            player.applyUpgrade(upgrade2);
            std::cout << "Upgrade 2 applied" << std::endl;
        }
    }
    else {
        std::cerr << "Error: Player instance is null." << std::endl;
    }
}

void DebugSystem::removeAbility(const std::string& ability) {
    if (&player != nullptr) {
        if (ability == "FireDamage") {
            player.setElementalDamage(ElementalType::None, 0);
            std::cout << "Fire damage removed" << std::endl;
        }
        else if (ability == "Cleave") {
            if (Warrior* warrior = dynamic_cast<Warrior*>(&player)) {
                warrior->disableCleave(); // Implement disableCleave() in Warrior class
                std::cout << "Cleave disabled" << std::endl;
            }
        }
    }
    else {
        std::cerr << "Error: Player instance is null." << std::endl;
    }
}

void DebugSystem::increaseHealth(int amount) {
    if (&player != nullptr) {
        player.increaseMaxHealth(amount);
        game.updateHealth(); // Update the health bar
    }
    else {
        std::cerr << "Error: Player instance is null." << std::endl;
    }
}

void DebugSystem::decreaseHealth(int amount) {
    if (&player != nullptr) {
        player.loseHealth(amount, nullptr);
        game.updateHealth(); // Update the health bar
    }
    else {
        std::cerr << "Error: Player instance is null." << std::endl;
    }
}



