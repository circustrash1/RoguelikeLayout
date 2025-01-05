#include "StatManager.h"
#include "Player.h"

StatManager::StatManager(Player& player) : player(player) {}

int StatManager::getAttackDamage() const {
    return player.getStats().strength * 10; // Example: 10 damage per strength point
}

float StatManager::getDodgeChance() const {
    const float maxDodgeChance = 0.3f; // Maximum dodge chance (30%)
    const float targetDexterity = 200.0f; // Dexterity where dodge chance reaches 30%
    const float steepness = 10.0f; // Controls how slowly the curve ramps up

    int dexterity = player.getStats().dexterity;

    // Use a dampened sigmoid growth function for very slow initial growth
    float dodgeChance = maxDodgeChance / (1.0f + steepness * std::exp(-dexterity / (targetDexterity / 4.0f)));

    // Cap the dodge chance at the maximum
    if (dodgeChance > maxDodgeChance) {
        dodgeChance = maxDodgeChance;
    }

    return dodgeChance;
}



int StatManager::getMaxHealth() const {
    return player.getStats().constitution * 10; // Example: 10 HP per constitution point
}

float StatManager::getSpellModifier() const {
    return 1.0f + (player.getStats().intelligence * 0.05f); // Example: 5% spell modifier per intelligence point
}

int StatManager::getViewDistance() const {
    return player.getStats().wisdom * 2; // Example: 2 tiles view distance per wisdom point
}

float StatManager::getMerchantPriceModifier() const {
    return 1.0f - (player.getStats().charisma * 0.02f); // Example: 2% price reduction per charisma point
}