// Button.cpp
#include "Button.h"

Button::Button(float width, float height, float x, float y, const std::string& text, const sf::Font& font, float textSize, float padding) {
    buttonShape.setSize(sf::Vector2f(width, height)); // Set size of the button
    buttonShape.setPosition(x, y); // Set position of the button
    buttonShape.setFillColor(sf::Color::Blue); // Set default color of the button

    buttonText.setFont(font); // Set font for the button text
    buttonText.setString(text); // Set string for the button text
    buttonText.setCharacterSize(static_cast<unsigned int>(textSize)); // Set character size for the button text
    buttonText.setFillColor(sf::Color::White); // Set color for the button text

    alignText(padding); // Align the text within the button
}

void Button::alignText(float padding) {
    sf::FloatRect textRect = buttonText.getLocalBounds(); // Get local bounds of the text
    sf::FloatRect buttonRect = buttonShape.getGlobalBounds(); // Get global bounds of the button
    buttonText.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f); // Set origin of the text
    buttonText.setPosition(buttonRect.left + padding, buttonRect.top + buttonRect.height / 2.0f); // Set position of the text
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(buttonShape); // Draw the button shape
    window.draw(buttonText); // Draw the button text
}

bool Button::isClicked(const sf::Vector2i& mousePos) const {
    return buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)); // Check if the button is clicked
}

const sf::Text& Button::getText() const {
    return buttonText; // Return the button text
}

void Button::setText(const std::string& text) {
    buttonText.setString(text); // Set the new text
    alignText(10.0f); // Re-align the text within the button (you can pass the padding value if needed)
}

void Button::setColor(const sf::Color& color) {
    buttonShape.setFillColor(color); // Set the new color of the button
}

void Button::setSize(float width, float height) {
    buttonShape.setSize(sf::Vector2f(width, height)); // Set the new size of the button
    alignText(10.0f); // Re-align the text within the button (you can pass the padding value if needed)
}