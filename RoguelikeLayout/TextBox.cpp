#include "TextBox.h"
#include <iostream>

TextBox::TextBox(const std::string& text, const sf::Font& font, unsigned int characterSize, float x, float y)
    : fullText(text), timePerLetter(0.05f), elapsedTime(0.0f), complete(false) {
    this->text.setFont(font);
    this->text.setCharacterSize(characterSize);
    this->text.setPosition(x, y);
    this->text.setFillColor(sf::Color::White);
    this->text.setString("");
}

void TextBox::update(float deltaTime) {
    if (complete) {
        if (clearText) {
            clearTimer += deltaTime;
            if (clearTimer >= clearAfterSeconds) {
                text.setString("");
                clearText = false; // Stop the timer after clearing the text
            }
        }
        return;
    }

    elapsedTime += deltaTime;
    if (elapsedTime >= timePerLetter) {
        if (displayedText.size() < fullText.size()) {
            displayedText += fullText[displayedText.size()];
            text.setString(displayedText);
            elapsedTime = 0.0f;
        }
        else {
            complete = true;
        }
    }
}

void TextBox::render(sf::RenderWindow& window) {
    window.draw(text);
}

bool TextBox::isComplete() const {
    return complete;
}

void TextBox::skip() {
    displayedText = fullText;
    text.setString(displayedText);
    complete = true;
}

const std::string TextBox::getFullText() const {
    return fullText;
}

void TextBox::clearAfter(float seconds) {
    clearAfterSeconds = seconds;
    clearText = true;
    clearTimer = 0.0f;
}
