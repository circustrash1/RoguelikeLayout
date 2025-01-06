#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class FontManager {
public:
    static sf::Font& getFont(const std::string& fontName);
private:
    static std::unordered_map<std::string, sf::Font> fonts;
    static std::unordered_map<std::string, bool> isLoaded;
};

#endif // FONT_MANAGER_H
