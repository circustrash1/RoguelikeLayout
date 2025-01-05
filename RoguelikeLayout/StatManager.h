#ifndef STATMANAGER_H
#define STATMANAGER_H

#include "Stats.h"

class Player;

class StatManager {
public:
    StatManager(Player& player);

    void updateStats();

    int getAttackDamage() const;
    float getDodgeChance() const;
    int getMaxHealth() const;
    float getSpellModifier() const;
    int getViewDistance() const;
    float getMerchantPriceModifier() const;

private:
    Player& player;
    int attackDamage;
    float dodgeChance;
    int maxHealth;
    float spellModifier;
    int viewDistance;
    float merchantPriceModifier;
};

#endif // STATMANAGER_H