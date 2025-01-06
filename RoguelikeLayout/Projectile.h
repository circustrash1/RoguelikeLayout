#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "SoundManager.h"

class Player;

class Projectile {
public:
	Projectile(int startX, int startY, int targetX, int targetY, int damage = 0, Player* player = nullptr, sf::Color color = sf::Color::White);
	void update();
	void render(sf::RenderWindow& window, int charSize);
	bool isActive() const;

private:
	bool active;
	int x, y;
	int targetX, targetY;
	int damage;
	float duration; // Duration for the projectile to reach the target
	sf::Clock clock;
	Player* player;
	sf::Color color; // Color attribute for the projectile
};

#endif // PROJECTILE_H
