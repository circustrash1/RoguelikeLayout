#include "BossRoom.h"
#include "FontManager.h"
#include <iostream>

BossRoom::BossRoom() : textBox(nullptr) {
    try {
        font = FontManager::getFont("fs-min");
        std::cout << "Font loaded successfully." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void BossRoom::enter() {
    textBox = new TextBox("... your bones will crack like twigs beneath my feet", font, 24, 130, 180);
    textBox->clearAfter(2.0f);
}

void BossRoom::update(float deltaTime) {
    if (textBox) {
        textBox->update(deltaTime);
        if (textBox->isComplete() && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            std::cout << "TextBox complete and 'E' pressed" << std::endl;
            delete textBox;
            textBox = nullptr;
        }
    }
}

void BossRoom::render(sf::RenderWindow& window) {
    if (textBox) {
        textBox->render(window);
    }
}

bool BossRoom::isTextComplete() const {
    return textBox == nullptr;
}

void BossRoom::clearTextBox() {
    if (textBox) {
        delete textBox;
        textBox = nullptr;
    }
}
