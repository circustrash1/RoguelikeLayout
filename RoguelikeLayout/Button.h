// Button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
	Button(float width, float height, float x, float y, const std::string& text, const sf::Font& font, float textSize, float padding = 10.0f);
	void draw(sf::RenderWindow& window);
	bool isClicked(const sf::Vector2i& mousePos) const;
	const sf::Text& getText() const; // Getter for button text
	void setText(const std::string& text); // Setter for button text
	void setColor(const sf::Color& color); // Setter for button color
	void setSize(float width, float height); // Setter for button size

private:
	sf::RectangleShape buttonShape; // Shape of the button
	sf::Text buttonText; // Text on the button
	void alignText(float padding); // Function to align text within the button
};

#endif // BUTTON_H