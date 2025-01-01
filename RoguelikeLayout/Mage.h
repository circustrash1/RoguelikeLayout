#ifndef MAGE_H
#define MAGE_H

#include "Player.h"
#include "Enemies.h"
#include <vector>

class Mage : public Player {
public:
    Mage(int x, int y);
    ~Mage() override = default;
    void attack(std::vector<Enemy*>& enemies) override;
    void renderProjectile(sf::RenderWindow& window, int charSize);

private:
    bool isProjectileActive = false;
    int projectileX, projectileY;
    int projectileTargetX, projectileTargetY;
    sf::Clock projectileClock;
};

#endif // MAGE_H