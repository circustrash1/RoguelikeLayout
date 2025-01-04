#include "DeathScreen.h"
#include "Game.h"


DeathScreen::DeathScreen(const sf::Font& font, float scaleX, float scaleY) {
	deathText.setFont(font);
	deathText.setString("You are dead.");
	deathText.setCharacterSize(24 * scaleY);
	deathText.setFillColor(sf::Color::Red);
	deathText.setStyle(sf::Text::Bold);
	deathText.setPosition(100 * scaleX, 50 * scaleY);

	restartText.setFont(font);
	restartText.setString("Restart?\n ENTER to play again\nESC to quit");
	restartText.setCharacterSize(12 * scaleY);
	restartText.setFillColor(sf::Color::White);
	restartText.setStyle(sf::Text::Bold);
	restartText.setPosition(100 * scaleX, 100 * scaleY);

}

void DeathScreen::display(sf::RenderWindow& window, Game& game) const {
	window.clear(sf::Color::Black);
	window.draw(deathText);
	window.draw(restartText);
	window.display();

	bool decisionMade = false;
	while (!decisionMade) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
				decisionMade = true;
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				game.restartGame();
				decisionMade = true;
			}
		}
	}
}
