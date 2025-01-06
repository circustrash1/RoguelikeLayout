#include "FontManager.h"
#include <iostream>
#include <filesystem>

std::unordered_map<std::string, sf::Font> FontManager::fonts;
std::unordered_map<std::string, bool> FontManager::isLoaded;

sf::Font& FontManager::getFont(const std::string& fontName) {
    if (isLoaded.find(fontName) == isLoaded.end() || !isLoaded[fontName]) {
        std::string fontPath = fontName + ".ttf";
        std::cout << "Loading font: " << fontPath << std::endl;
        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
        if (!fonts[fontName].loadFromFile(fontPath)) {
            std::cerr << "Error: Failed to load font: " << fontPath << std::endl;
            throw std::runtime_error("FontManager::getFont() - Failed to load font: " + fontPath);
        }
        isLoaded[fontName] = true;
    }
    return fonts[fontName];
}