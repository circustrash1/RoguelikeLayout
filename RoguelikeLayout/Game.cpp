// Game.cpp
#include "Game.h"
#include <iostream>
#include "Player.h"
#include "Warrior.h"
#include "Mage.h"

Game::Game()
    : scaleX(static_cast<float>(sf::VideoMode::getDesktopMode().width) / 640.0f),
    scaleY(static_cast<float>(sf::VideoMode::getDesktopMode().height) / 360.0f),
    window(sf::VideoMode(640, 360), "Roguelike Layout"), map(62, 32), player(nullptr), isShaking(false), shakeIntensity(0.1f), healthBar(nullptr) {

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    std::cout << "scaleX: " << scaleX << ", scaleY: " << scaleY << std::endl;

    window.create(desktop, "Roguelike Layout", sf::Style::Fullscreen);

    if (!font.loadFromFile("fs-min.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        exit(-1);
    }

    mainBar.setSize(sf::Vector2f(640 * scaleX, 100 * scaleY));
    mainBar.setFillColor(sf::Color(50, 50, 50));
    mainBar.setPosition(0, 260 * scaleY);

    sideBar.setSize(sf::Vector2f(140 * scaleX, 260 * scaleY));
    sideBar.setFillColor(sf::Color(70, 70, 70));
    sideBar.setPosition(500 * scaleX, 0);

    healthText.setFont(font);
    healthText.setString("Health: ");
    healthText.setCharacterSize(static_cast<unsigned int>(12 * scaleY));
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(20 * scaleX, 280 * scaleY);

    Button* upgradeButton = new Button(60 * scaleX, 15 * scaleY, 515 * scaleX, 25 * scaleY, "Upgrade", font, 8 * scaleY);
    upgradeButton->setColor(sf::Color::Green);
    buttons.push_back(upgradeButton);

    Button* sellButton = new Button(60 * scaleX, 15 * scaleY, 515 * scaleX, 50 * scaleY, "Sell", font, 8 * scaleY);
    sellButton->setColor(sf::Color::Red);
    buttons.push_back(sellButton);

    map.generate();
    originalView = window.getDefaultView(); // Store the original view
    window.setView(originalView); // Set the view to the original view

    displayCharacterSelection();
}

Player* Game::createPlayer(const std::string& className) {
    if (className == "Warrior") {
        return new Warrior(62 / 2, 32 / 2);
    }
    else if (className == "Mage") {
        return new Mage(62 / 2, 32 / 2);
    }
    else {
        return nullptr;
    }
}

void Game::displayCharacterSelection() {
    window.clear(sf::Color::Black);

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(12 * scaleY);
    title.setFillColor(sf::Color::White);
    title.setString("Select your character");
    title.setPosition(200 * scaleX, 50 * scaleY);
    window.draw(title);

    sf::Text warriorText;
    warriorText.setFont(font);
    warriorText.setCharacterSize(10 * scaleY);
    warriorText.setFillColor(sf::Color::White);
    warriorText.setString("1. Warrior\nHealth: 150\nAttack: 15");
    warriorText.setPosition(200 * scaleX, 100 * scaleY);
    window.draw(warriorText);

    sf::Text mageText;
    mageText.setFont(font);
    mageText.setCharacterSize(10 * scaleY);
    mageText.setFillColor(sf::Color::White);
    mageText.setString("2. Mage\nHealth: 100\nAttack: 10");
    mageText.setPosition(350 * scaleX, 100 * scaleY);
    window.draw(mageText);

    // MORE CHARACTER SELECTION HERE

    window.display();

    bool characterSelected = false;
    while (!characterSelected) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    player = createPlayer("Warrior");
                    healthBar = new HealthBar(20, 300, 12, 150, scaleX, scaleY); // Initialize HealthBar for Warrior
                    characterSelected = true;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    player = createPlayer("Mage");
                    healthBar = new HealthBar(20, 300, 12, 100, scaleX, scaleY); // Initialize HealthBar for Mage
                    characterSelected = true;
                }
                healthText.setString("Health: " + std::to_string(player->getHealth()));
            }
        }
    }
}

Game::~Game() {
    for (Button* button : buttons) {
        delete button;
    }
    delete player;
    delete healthBar; // Clean up healthBar
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            for (Button* button : buttons) {
                if (button->isClicked(mousePos)) {
                    std::cout << button->getText().getString().toAnsiString() << " button clicked!" << std::endl;
                }
            }
        }
    }
}

void Game::updateHealth() {
    healthText.setString("Health: " + std::to_string(player->getHealth()));
    healthBar->update(player->getHealth());
    std::cout << "Displaying health bar" << std::endl;
}

void Game::update() {
    static sf::Clock clock;
    static sf::Time lastMoveTime = sf::Time::Zero;
    sf::Time moveDelay = sf::milliseconds(100); // Adjust delay as needed

    if (clock.getElapsedTime() - lastMoveTime > moveDelay) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            player->move('w', map.getMap(), map.getEnemies());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            player->move('s', map.getMap(), map.getEnemies());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            player->move('a', map.getMap(), map.getEnemies());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            player->move('d', map.getMap(), map.getEnemies());
        }
        lastMoveTime = clock.getElapsedTime();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        screenShake(5.0f, 0.5f);
    }

    player->update(map.getEnemies());
    updateHealth();
}

void Game::render() {
    window.clear(sf::Color::Black);

    if (isShaking) {
        float elapsed = shakeClock.getElapsedTime().asSeconds();
        if (elapsed < 0.5f) { // Shake duration
            float offsetX = (std::rand() % 3 - 1) * shakeIntensity;
            float offsetY = (std::rand() % 3 - 1) * shakeIntensity;
            sf::View view = originalView;
            view.move(offsetX, offsetY);
            window.setView(view);
        }
        else {
            isShaking = false;
            window.setView(originalView);
        }
    }
    else {
        window.setView(originalView); // Ensure the view is reset to the original view
    }

    window.draw(mainBar);
    window.draw(sideBar);
    window.draw(healthText);

    for (Button* button : buttons) {
        button->draw(window);
    }

    map.render(window, player->getX(), player->getY(), 24, player); // Pass the desired character size
    player->render(window, 24); // Render the player

    healthBar->render(window);

    window.display();
}

void Game::screenShake(float intensity, float duration) {
    isShaking = true;
    shakeIntensity = intensity;
    shakeClock.restart();
}

