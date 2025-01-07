#include "Boss.h"
#include "FontManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Boss::Boss(int x, int y, int enemyHealth, int attackDamage, float attackCooldown, const sf::Color& color)
    : Enemy(x, y, ' ', enemyHealth, attackDamage, attackCooldown, color), moveCooldown(1.0f) { // Use ' ' as a placeholder symbol
    initBodyParts(x, y);
    moveClock.restart();
}

void Boss::initBodyParts(int x, int y) {
    // Example ASCII art for the boss
    asciiArt = {
        " .-.",
        " (o.o)",
        " |=|",
        " __|__",
        " //.=|=.\\\\",
        " // .=|=. \\\\",
        " \\\\ .=|=. //",
        " \\\\(_=_)//",
        " (:| |:)",
        " || ||",
        " () ()",
        " || ||",
        " || ||",
        " ==' '=="
    };

    for (int dy = 0; dy < asciiArt.size(); ++dy) {
        for (int dx = 0; dx < asciiArt[dy].size(); ++dx) {
            if (asciiArt[dy][dx] != ' ') {
                bodyParts.push_back({ x + dx - asciiArt[dy].size() / 2, y + dy - asciiArt.size() / 2 });
                bodyPartChars.push_back(asciiArt[dy][dx]);
            }
        }
    }
}

bool Boss::checkCollision(const Player& player) const {
    int playerX = player.getX();
    int playerY = player.getY();

    for (const auto& part : bodyParts) {
        if (part.first == playerX && part.second == playerY) {
            return true;
        }
    }
    return false;
}

void Boss::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
    if (!alive) return;

    // Check if the move cooldown has elapsed
    if (moveClock.getElapsedTime().asSeconds() < moveCooldown) {
        return;
    }

    // Calculate the direction to move towards the player
    int dx = 0, dy = 0;
    if (x < playerX) dx = 1;
    else if (x > playerX) dx = -1;

    if (y < playerY) dy = 1;
    else if (y > playerY) dy = -1;

    // Calculate the new position for the boss
    int newX = x + dx;
    int newY = y + dy;

    // Check for collisions with the map, other enemies, and the player, disregarding its own body parts
    bool collision = false;
    for (const Enemy* enemy : enemies) {
        if (enemy != this) {
            for (const auto& part : bodyParts) {
                if (enemy->getX() == part.first + dx && enemy->getY() == part.second + dy) {
                    collision = true;
                    break;
                }
            }
        }
        if (collision) break;
    }

    // Check for map boundaries and player collision
    for (const auto& part : bodyParts) {
        int partNewX = part.first + dx;
        int partNewY = part.second + dy;
        if (partNewX < 0 || partNewX >= map[0].size() || partNewY < 0 || partNewY >= map.size() || map[partNewY][partNewX] != '.') {
            collision = true;
            break;
        }
        if (partNewX == playerX && partNewY == playerY) {
            collision = true;
            break;
        }
    }

    if (!collision) {
        // Update the position of all body parts
        x = newX;
        y = newY;
        for (auto& part : bodyParts) {
            part.first += dx;
            part.second += dy;
        }

        // Restart the move clock
        moveClock.restart();
    }
}

void Boss::attack(Player* player, const std::vector<std::vector<char>>& map) {
    // Boss attack logic
}

void Boss::playDeathAnimation(sf::RenderWindow& window) {
    // Boss death animation
}

void Boss::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) {
    sf::Font& font = FontManager::getFont("IBM_BIOS");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);

    int viewDistance = player->getStatManager().getViewDistance() - (player->getStats().wisdom * 1.75) + 5;
    int minOpacity = 0;
    int maxOpacity = 255;

    for (size_t i = 0; i < bodyParts.size(); ++i) {
        float distance = std::sqrt(std::pow(playerX - bodyParts[i].first, 2) + std::pow(playerY - bodyParts[i].second, 2));
        int opacity = maxOpacity - (distance * (maxOpacity - minOpacity) / viewDistance);
        if (distance > viewDistance) {
            opacity = minOpacity; // Set opacity to 0 if outside view distance
        }

        text.setString(bodyPartChars[i]);
        text.setPosition(bodyParts[i].first * charSize, bodyParts[i].second * charSize);

        if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) { // Flash white when taking damage
            text.setFillColor(sf::Color(255, 255, 255, opacity));
        }
        else if (isTakingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.25f) { // Flash red when taking fire damage
            text.setFillColor(sf::Color(255, 0, 0, opacity));
        }
        else {
            text.setFillColor(sf::Color(color.r, color.g, color.b, opacity));
        }

        window.draw(text);
    }

    // Display damage numbers
    if (isDisplayingDamage && damageDisplayClock.getElapsedTime().asSeconds() < 0.5f) {
        displayDamage(window, charSize);
    }
    else {
        isDisplayingDamage = false;
    }

    if (isDisplayingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.5f) {
        displayFireDamage(window, charSize);
    }
    else {
        isDisplayingFireDamage = false;
    }


    // Reset the damage flags after rendering
    if (enemyDamageClock.getElapsedTime().asSeconds() >= 0.1f) {
        isTakingDamage = false;
    }
    if (fireDamageClock.getElapsedTime().asSeconds() >= 0.25f) {
        isTakingFireDamage = false;
    }
}

void Boss::updateBodyParts(int deltaX, int deltaY) {
    for (auto& part : bodyParts) {
        part.first += deltaX;
        part.second += deltaY;
    }
}

void Boss::renderBossHealthBar(sf::RenderWindow& window, int charSize, float scaleX, float scaleY) const {
    float barWidth = 50 * charSize; // Width of the health bar
    float barHeight = charSize; // Height of the health bar
    float healthPercentage = static_cast<float>(enemyHealth) / maxHealth;
    float healthBarWidth = barWidth * healthPercentage;

    sf::RectangleShape healthBarBackground(sf::Vector2f(barWidth, barHeight));
    healthBarBackground.setFillColor(sf::Color(255, 0, 0, 75)); // Red background

    // Calculate the position to be between the left side of the screen and the left side of the side bar
    float xPos = (window.getSize().x - 140 * scaleX - barWidth) / 2;

    healthBarBackground.setPosition(xPos, charSize * scaleY);

    sf::RectangleShape healthBar(sf::Vector2f(healthBarWidth, barHeight));
    healthBar.setFillColor(sf::Color(0, 255, 0, 75)); // Green foreground
    healthBar.setPosition(xPos, charSize * scaleY);

    window.draw(healthBarBackground);
    window.draw(healthBar);

	std::cout << "Boss health: " << enemyHealth << " / " << maxHealth << std::endl;
}

// Havok boss
Havok::Havok(int x, int y)
    : Boss(x, y, 2500, 50, 2.0f, sf::Color(128, 128, 128)), isCharging(false), chargeDuration(2.5f), chargeCooldown(5.0f), hasAppliedStun(false),
    displayAOE(false) {
}

void Havok::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
    if (!isCharging) {
        Boss::move(map, playerX, playerY, enemies);
    }
}

bool Havok::isPlayerInRange(int playerX, int playerY) const {
    for (const auto& part : bodyParts) {
        int distance = std::abs(part.first - playerX) + std::abs(part.second - playerY);
        if (distance <= 4) {
            return true;
        }
    }
    return false;
}

void Havok::attack(Player* player, const std::vector<std::vector<char>>& map) {
    adjustChargeDuration();

    if (!isCharging && chargeCooldownClock.getElapsedTime().asSeconds() >= chargeCooldown) {
        if (isPlayerInRange(player->getX(), player->getY())) {
            isCharging = true;
            hasAppliedStun = false;
            chargeClock.restart();
            std::cout << "Havok is charging stun ability!" << std::endl;
            chargeFlashClock.restart();
            SoundManager::getInstance().playSound("havok_charging");

            // Get the original length of the audio clip
            float originalLength = SoundManager::getInstance().getSoundDuration("havok_charging");
            if (originalLength > 0) {
                // Calculate the required pitch
                float pitch = originalLength / chargeDuration;
                SoundManager::getInstance().setSoundPitch("havok_charging", pitch);
            }
        }
    }

    if (isCharging) {
        if (chargeClock.getElapsedTime().asSeconds() >= chargeDuration && !hasAppliedStun) {
            std::cout << "Havok releases stun ability!" << std::endl;
            applyStunAndDamage(player->getX(), player->getY(), player);
            hasAppliedStun = true;
            isCharging = false;
            chargeCooldownClock.restart();
            displayAOE = true;
            aoeDisplayClock.restart();
            SoundManager::getInstance().playSound("havok_stun");
        }
    }
    else {
        hasAppliedStun = false;
    }
}

void Havok::adjustChargeDuration() {
    float healthPercentage = static_cast<float>(enemyHealth) / maxHealth;
    chargeDuration = 2.8f * std::sqrt(healthPercentage) + 0.5f * (1.0f - std::sqrt(healthPercentage));
}


void Havok::applyStunAndDamage(int playerX, int playerY, Player* player) {
    aoeCenterX = x; // Set the center position of the AOE circle
    aoeCenterY = y;
    int radius = 18; // Increase the radius
    damagedTiles.clear(); // Clear previous damaged tiles

    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance <= radius) {
                int tileX = aoeCenterX + dx;
                int tileY = aoeCenterY + dy;
                damagedTiles.push_back({ tileX, tileY });
                if (tileX == playerX && tileY == playerY) {
                    player->loseHealth(attackDamage, this); // Apply heavy damage
                    player->applyStun(2.0f);
                }
            }
        }
    }
}

void Havok::renderAOECircle(sf::RenderWindow& window, int charSize, const std::vector<std::vector<char>>& map) {
    if (aoeDisplayClock.getElapsedTime().asSeconds() >= 2.0f) {
        displayAOE = false;
        return;
    }

    sf::Font& font = FontManager::getFont("IBM_BIOS");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::Yellow);

    for (const auto& tile : damagedTiles) {
        // Check if the tile is within bounds
        if (tile.second < 0 || tile.second >= map.size() || tile.first < 0 || tile.first >= map[0].size()) {
            continue;
        }

        // Check if the tile is occupied by a body part
        bool isBodyPart = false;
        for (const auto& part : bodyParts) {
            if (tile.first == part.first && tile.second == part.second) {
                isBodyPart = true;
                break;
            }
        }

        // Check if the tile is a wall
        bool isWall = (map[tile.second][tile.first] != '.');

        // Skip rendering if the tile is occupied by a body part or a wall
        if (isBodyPart || isWall) {
            continue;
        }

        // Check if the tile is on the boundary of the circle
        bool isBoundary = false;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int neighborX = tile.first + dx;
                int neighborY = tile.second + dy;
                float distance = std::sqrt((neighborX - aoeCenterX) * (neighborX - aoeCenterX) + (neighborY - aoeCenterY) * (neighborY - aoeCenterY));
                if (distance > 18) {
                    isBoundary = true;
                    break;
                }
            }
            if (isBoundary) break;
        }

        // Render only the boundary tiles
        if (isBoundary) {
            text.setString("X");
            text.setPosition(tile.first * charSize, tile.second * charSize);
            window.draw(text);
        }
    }
}


void Havok::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) {
    sf::Font& font = FontManager::getFont("IBM_BIOS");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);

    int viewDistance = player->getStatManager().getViewDistance() - (player->getStats().wisdom * 1.75) + 5;
    int minOpacity = isCharging ? 50 : 0; // Set minimum opacity to 50 if charging
    int maxOpacity = 255;

    for (size_t i = 0; i < bodyParts.size(); ++i) {
        float distance = std::sqrt(std::pow(playerX - bodyParts[i].first, 2) + std::pow(playerY - bodyParts[i].second, 2));
        int opacity = maxOpacity - (distance * (maxOpacity - minOpacity) / viewDistance);
        if (distance > viewDistance && !isCharging) {
            opacity = minOpacity; // Set opacity to 0 if outside view distance and not charging
        }

        text.setString(bodyPartChars[i]);
        text.setPosition(bodyParts[i].first * charSize, bodyParts[i].second * charSize);

        if (isCharging && chargeFlashClock.getElapsedTime().asSeconds() < chargeDuration) { // Flash yellow when charging
            if (static_cast<int>(chargeFlashClock.getElapsedTime().asMilliseconds() / 100) % 2 == 0) {
                text.setFillColor(sf::Color(255, 255, 0, opacity));
            }
        }
        else if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) { // Flash white when taking damage
            text.setFillColor(sf::Color(255, 255, 255, opacity));
        }
        else if (isTakingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.25f) { // Flash red when taking fire damage
            text.setFillColor(sf::Color(255, 0, 0, opacity));
        }
        else {
            text.setFillColor(sf::Color(color.r, color.g, color.b, opacity));
        }

        window.draw(text);
    }

    // Display damage numbers
    if (isDisplayingDamage && damageDisplayClock.getElapsedTime().asSeconds() < 0.5f) {
        displayDamage(window, charSize);
    }
    else {
        isDisplayingDamage = false;
    }

    if (isDisplayingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.5f) {
        displayFireDamage(window, charSize);
    }
    else {
        isDisplayingFireDamage = false;
    }

    if (displayAOE) {
        renderAOECircle(window, charSize, map);
    }



    // Reset the damage flags after rendering
    if (enemyDamageClock.getElapsedTime().asSeconds() >= 0.1f) {
        isTakingDamage = false;
    }
    if (fireDamageClock.getElapsedTime().asSeconds() >= 0.25f) {
        isTakingFireDamage = false;
    }
}

void Havok::playDeathAnimation(sf::RenderWindow& window) {
    // Havok death animation
}

// Infernos boss
Infernos::Infernos(int x, int y)
    : Boss(x, y, 500, 10, 2.0f, sf::Color(128, 128, 128)) {
}

void Infernos::move(const std::vector<std::vector<char>>& map, int playerX, int playerY, const std::vector<Enemy*>& enemies) {
    Boss::move(map, playerX, playerY, enemies);
}

void Infernos::attack(Player* player, const std::vector<std::vector<char>>& map) {
    // Infernos attack logic
}

void Infernos::playDeathAnimation(sf::RenderWindow& window) {
    // Infernos death animation
}

void Infernos::render(sf::RenderWindow& window, int charSize, int playerX, int playerY, Player* player, const std::vector<Enemy*>& enemies, const std::vector<std::vector<char>>& map) {
    // Call the base class render function to handle common rendering logic
    Boss::render(window, charSize, playerX, playerY, player, enemies, map);

    sf::Font& font = FontManager::getFont("IBM_BIOS");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);

    for (size_t i = 0; i < bodyParts.size(); ++i) {
        text.setString(bodyPartChars[i]);
        text.setPosition(bodyParts[i].first * charSize, bodyParts[i].second * charSize);

        if (isTakingDamage && enemyDamageClock.getElapsedTime().asSeconds() < 0.1f) { // Flash white when taking damage
            text.setFillColor(sf::Color::White);
        }
        else if (isTakingFireDamage && fireDamageClock.getElapsedTime().asSeconds() < 0.25f) { // Flash red when taking fire damage
            text.setFillColor(sf::Color::Red);
        }
        else {
            text.setFillColor(color);
        }

        window.draw(text);
    }

    // Reset the damage flags after rendering
    if (enemyDamageClock.getElapsedTime().asSeconds() >= 0.1f) {
        isTakingDamage = false;
    }
    if (fireDamageClock.getElapsedTime().asSeconds() >= 0.25f) {
        isTakingFireDamage = false;
    }
}
