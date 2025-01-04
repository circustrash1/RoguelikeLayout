#include "TitleScreen.h"


TitleScreen::TitleScreen(const sf::Font& font, float scaleX, float scaleY) {
	titleText.setFont(font);
	titleText.setString("Roguelike Game");
	titleText.setCharacterSize(24 * scaleY);
	titleText.setFillColor(sf::Color::White);
	titleText.setStyle(sf::Text::Bold);
	titleText.setPosition(100 * scaleX, 50 * scaleY);

	controlsText.setFont(font);
	controlsText.setString("Controls: \n - Move: WASD\n - Attack: Spacebar\n - Interact: E\n - Pause: P");
	controlsText.setCharacterSize(16 * scaleY);
	controlsText.setFillColor(sf::Color::White);
	controlsText.setStyle(sf::Text::Bold);
	controlsText.setPosition(100 * scaleX, 150 * scaleY);
}

void TitleScreen::display(sf::RenderWindow& window) const {
	window.clear(sf::Color::Black);
	window.draw(titleText);
	window.draw(controlsText);
	window.display();

	sf::Event event;
	while (true) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::KeyPressed) {
				return;
			}
		}
	}
}