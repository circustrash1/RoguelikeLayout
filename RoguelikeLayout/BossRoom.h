#ifndef BOSSROOM_H
#define BOSSROOM_H

#include "TextBox.h"
#include <SFML/Graphics.hpp>

class BossRoom {
public:
	BossRoom();
	void enter();
	void update(float deltaTime);
	void render(sf::RenderWindow& window);
	bool isTextComplete() const;

	void clearTextBox();

private:
	TextBox* textBox;
	sf::Font font;
};


#endif // BOSSROOM_H