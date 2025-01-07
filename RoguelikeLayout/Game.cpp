#include "Game.h"
#include <iostream>
#include "Player.h"
#include "Warrior.h"
#include "Mage.h"
#include "SoundManager.h"
#include "EventMaps.h"
#include "TitleScreen.h"
#include "DeathScreen.h"

Game::Game()
	: scaleX(static_cast<float>(sf::VideoMode::getDesktopMode().width) / 640.0f),
	scaleY(static_cast<float>(sf::VideoMode::getDesktopMode().height) / 360.0f),
	window(sf::VideoMode(640, 360), "Roguelike Layout"), map(62, 32), player(nullptr), isShaking(false), shakeIntensity(0.1f),
	healthBar(nullptr), showStats(nullptr), showGold(new ShowGold()), showStage(new ShowStage()), stageCount(0) {
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	window.create(desktop, "Roguelike Layout", sf::Style::Fullscreen);

	if (!font.loadFromFile("fs-min.ttf")) {
		std::cerr << "Error loading font!" << std::endl;
		exit(-1);
	}

	TitleScreen titleScreen(font, scaleX, scaleY);
	titleScreen.display(window);

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

	// NOW UNUSED UPGRADE / SELL BUTTONS
	// MAYBE TURN INTO SETTINGS ETC.
	//Button* upgradeButton = new Button(60 * scaleX, 15 * scaleY, 515 * scaleX, 25 * scaleY, "Upgrade", font, 8 * scaleY);
	//upgradeButton->setColor(sf::Color::Green);
	//buttons.push_back(upgradeButton);

	//Button* sellButton = new Button(60 * scaleX, 15 * scaleY, 515 * scaleX, 50 * scaleY, "Sell", font, 8 * scaleY);
	//sellButton->setColor(sf::Color::Red);
	//buttons.push_back(sellButton);

	std::srand(static_cast<unsigned int>(std::time(nullptr)));  // Random seed for sound pitch randomize
	initialize();   // Init sounds + music

	map.generate();
	originalView = window.getDefaultView(); // Store the original view
	window.setView(originalView); // Set the view to the original view

	displayCharacterSelection();

	// Debug
	debugSystem = new DebugSystem(*player, *this);
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
					healthBar = new HealthBar(20, 300, 12, player->getStatManager().getMaxHealth(), scaleX, scaleY); // Initialize HealthBar for Warrior
					showStats = new ShowStats(*player); // Initialize ShowStats for Warrior
					characterSelected = true;
				}
				else if (event.key.code == sf::Keyboard::Num2) {
					player = createPlayer("Mage");
					healthBar = new HealthBar(20, 300, 12, player->getStatManager().getMaxHealth(), scaleX, scaleY); // Initialize HealthBar for Mage
					showStats = new ShowStats(*player); // Initialize ShowStats for Mage
					characterSelected = true;
				}
				if (player != nullptr) {
					healthText.setString("Health: " + std::to_string(player->getHealth()));
				}
				else {
					std::cerr << "Error: Player creation failed!" << std::endl;
				}
			}
		}
	}

	// Cleanup
	window.clear(sf::Color::Black);
	window.display();
}

void Game::renderUpgradeOrMerchantWindow(const std::string& titleText, const std::vector<Upgrade>& upgrades, bool isMerchantShop) {
	window.clear(sf::Color::Black);
	// Title
	sf::Text title;
	title.setFont(font);
	title.setCharacterSize(12 * scaleY);
	title.setFillColor(sf::Color::White);
	title.setString(titleText);
	title.setPosition(145 * scaleX, 10 * scaleY);
	if (isMerchantShop) {
		title.setPosition(180 * scaleX, 10 * scaleY);
	}
	window.draw(title);

	// Calculate card positions
	float cardWidth = 60 * scaleX;
	float cardHeight = 100 * scaleY;
	float spaceBetweenCards = 80 * scaleX;
	float totalWidth = 3 * cardWidth + 2 * spaceBetweenCards;
	float startX = (window.getSize().x - totalWidth) / 2 - 280;
	float startY = 50 * scaleY - 50;

	// Render ASCII cards
	for (size_t i = 0; i < upgrades.size(); ++i) {
		const std::vector<std::string>& asciiArt = upgrades[i].getAsciiArt();
		sf::Text cardArt;
		sf::Text cardName;
		sf::Text cardPositiveEffects;
		sf::Text cardNegativeEffects;
		sf::Text cardCost;
		cardArt.setFont(font);
		cardArt.setCharacterSize(10 * scaleY);
		cardArt.setFillColor(upgrades[i].getColor());
		cardArt.setLineSpacing(1.6f);
		std::string cardString;
		for (const std::string& line : asciiArt) {
			cardString += line + "\n";
		}
		cardArt.setString(cardString);
		cardArt.setPosition(startX + i * (cardWidth + spaceBetweenCards), startY);

		// Render card names
		cardName.setFont(font);
		cardName.setCharacterSize(10 * scaleY);
		cardName.setFillColor(upgrades[i].getColor());
		cardName.setString(upgrades[i].getName());
		cardName.setPosition(startX + i * (cardWidth + spaceBetweenCards) - 40, startY + cardHeight + 250);

		// Render positive effects
		cardPositiveEffects.setFont(font);
		cardPositiveEffects.setCharacterSize(6 * scaleY);
		cardPositiveEffects.setFillColor(sf::Color::Green);
		cardPositiveEffects.setString(wrapText(upgrades[i].getPositiveEffects(), font, 6 * scaleY, cardWidth));
		cardPositiveEffects.setPosition(startX + i * (cardWidth + spaceBetweenCards) - 40, startY + cardHeight + 300);

		// Render negative effects
		cardNegativeEffects.setFont(font);
		cardNegativeEffects.setCharacterSize(6 * scaleY);
		cardNegativeEffects.setFillColor(sf::Color::Red);
		cardNegativeEffects.setString("\n\n" + wrapText(upgrades[i].getNegativeEffects(), font, 6 * scaleY, cardWidth));
		cardNegativeEffects.setPosition(startX + i * (cardWidth + spaceBetweenCards) - 40, startY + cardHeight + 300);

		if (isMerchantShop) {
			// Render exit shop text
			sf::Text exitShopText;
			exitShopText.setFont(font);
			exitShopText.setCharacterSize(6 * scaleY);
			exitShopText.setFillColor(sf::Color::White);
			exitShopText.setString("Press 'E' to exit shop");
			exitShopText.setPosition(190 * scaleX, 25 * scaleY);
			window.draw(exitShopText);

			// Render card cost
			cardCost.setFont(font);
			cardCost.setCharacterSize(6 * scaleY);
			cardCost.setFillColor(sf::Color::Yellow);
			cardCost.setString("Cost: " + std::to_string(static_cast<int>(upgrades[i].getCost() * player->getStatManager().getMerchantPriceModifier())));
			cardCost.setPosition(startX + i * (cardWidth + spaceBetweenCards) - 40, startY + cardHeight + 300);
		}

		if (upgrades[i].isPurchased()) {
			sf::Color greyColor(128, 128, 128);
			cardArt.setFillColor(greyColor);
			cardName.setFillColor(greyColor);
			cardPositiveEffects.setFillColor(greyColor);
			cardNegativeEffects.setFillColor(greyColor);
			cardCost.setFillColor(greyColor);
		}
		else if (isMerchantShop && player->getGold() < (static_cast<int>(upgrades[i].getCost() * player->getStatManager().getMerchantPriceModifier()))) {
			sf::Color color = cardArt.getFillColor();
			color.a = 50; // Lower opacity
			cardArt.setFillColor(color);
			cardName.setFillColor(color);

			cardPositiveEffects.setFillColor(sf::Color(0, 255, 0, 50));
			cardNegativeEffects.setFillColor(sf::Color(255, 0, 0, 50));
		}

		// Calculate column positions
		float columnX = startX + i * (cardWidth + spaceBetweenCards);
		float columnY = startY;

		alignCards(cardName, cardArt, cardPositiveEffects, cardNegativeEffects, cardCost, 5.0f, columnX);

		window.draw(cardName);
		window.draw(cardArt);
		window.draw(cardPositiveEffects);
		window.draw(cardNegativeEffects);
		window.draw(cardCost);
	}

	window.draw(mainBar);
	window.draw(sideBar);
	window.draw(healthText);

	// NOW UNUSED BUTTONS
	//for (Button* button : buttons) {
	//	button->draw(window);
	//}

	if (healthBar != nullptr) {
		healthBar->render(window);
	}

	if (showStats != nullptr && player != nullptr) {
		showStats->renderPlayerStats(window, *player, 24, scaleX, scaleY, nullptr, "");
	}

	displayCollectedUpgrades();

	// Render stage counter
	showStage->renderStage(window, stageCount, 500, 250, 24, scaleX, scaleY);

	// Render UI total Gold
	showGold->renderGold(window, player->getGold(), 500, 340, 24, scaleX, scaleY);

	window.display();
}

void Game::displayUpgradeSelection() {
	renderUpgradeOrMerchantWindow("Select an upgrade", availableUpgrades, false);

	bool upgradeSelected = false;
	while (!upgradeSelected) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
				return;
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Num1 && availableUpgrades.size() >= 1) {
					std::cout << "Player picked up: " << availableUpgrades[0].getName() << std::endl;
					player->applyUpgrade(availableUpgrades[0]);
					upgradeSelected = true;
					SoundManager::getInstance().playSound("upgrade_choice");
				}
				else if (event.key.code == sf::Keyboard::Num2 && availableUpgrades.size() >= 2) {
					std::cout << "Player picked up: " << availableUpgrades[1].getName() << std::endl;
					player->applyUpgrade(availableUpgrades[1]);
					upgradeSelected = true;
					SoundManager::getInstance().playSound("upgrade_choice");
				}
				else if (event.key.code == sf::Keyboard::Num3 && availableUpgrades.size() >= 3) {
					player->applyUpgrade(availableUpgrades[2]);
					std::cout << "Player picked up: " << availableUpgrades[2].getName() << std::endl;
					upgradeSelected = true;
					SoundManager::getInstance().playSound("upgrade_choice");
				}
			}
		}
	}

	// Cleanup
	window.clear(sf::Color::Black);
	window.display();
}

void Game::displayMerchantWindow(Room& room) {
	bool upgradeSelected = false;
	while (!upgradeSelected) {
		renderUpgradeOrMerchantWindow("Merchant Shop", room.merchantUpgrades, true);

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				return;
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Num1 && room.merchantUpgrades.size() >= 1 && player->getGold() >= room.merchantUpgrades[0].getCost() * player->getStatManager().getMerchantPriceModifier() && !room.merchantUpgrades[0].isPurchased()) {
					player->spendGold(static_cast<int>(room.merchantUpgrades[0].getCost() * player->getStatManager().getMerchantPriceModifier()));
					player->applyUpgrade(room.merchantUpgrades[0]);
					room.merchantUpgrades[0].setPurchased(true);
					SoundManager::getInstance().playSound("upgrade_choice");
				}
				else if (event.key.code == sf::Keyboard::Num2 && room.merchantUpgrades.size() >= 2 && player->getGold() >= room.merchantUpgrades[1].getCost() * player->getStatManager().getMerchantPriceModifier() && !room.merchantUpgrades[1].isPurchased()) {
					player->spendGold(static_cast<int>(room.merchantUpgrades[1].getCost() * player->getStatManager().getMerchantPriceModifier()));
					player->applyUpgrade(room.merchantUpgrades[1]);
					room.merchantUpgrades[1].setPurchased(true);
					SoundManager::getInstance().playSound("upgrade_choice");
				}
				else if (event.key.code == sf::Keyboard::Num3 && room.merchantUpgrades.size() >= 3 && player->getGold() >= room.merchantUpgrades[2].getCost() * player->getStatManager().getMerchantPriceModifier() && !room.merchantUpgrades[2].isPurchased()) {
					player->spendGold(static_cast<int>(room.merchantUpgrades[2].getCost() * player->getStatManager().getMerchantPriceModifier()));
					player->applyUpgrade(room.merchantUpgrades[2]);
					room.merchantUpgrades[2].setPurchased(true);
					SoundManager::getInstance().playSound("upgrade_choice");
				}
				else if (event.key.code == sf::Keyboard::E || event.key.code == sf::Keyboard::Escape) {
					upgradeSelected = true; // Press E to exit shop
				}
			}
		}

		// Update the window to reflect changes
		window.clear(sf::Color::Black);
		renderUpgradeOrMerchantWindow("Merchant Shop", room.merchantUpgrades, true);
		window.display();
	}

	// Cleanup
	window.clear(sf::Color::Black);
	window.display();
	shopCloseClock.restart();
}

Game::~Game() {
	for (Button* button : buttons) {
		delete button;
	}
	delete player;
	delete healthBar; // Clean up healthBar
	delete showStats; // Clean up showStats
	delete showGold;
	delete showStage;
}

void Game::run() {
	sf::Clock clock;
	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		processEvents();
		update(deltaTime);
		handleInput();
		SoundManager::getInstance().updateCrossfade();  // Update crossfade effect
		render();
	}
}

void Game::processEvents() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
			window.close();
		}

		// NOW UNUSED BUTTON CLICK DETECTION
		// REWORK FOR SETTINGS BUTTON
		//if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		//	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		//	for (Button* button : buttons) {
		//		if (button->isClicked(mousePos)) {
		//			std::cout << button->getText().getString().toAnsiString() << " button clicked!" << std::endl;
		//		}
		//	}
		//}

		// Debug keys to modify player gold
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::G) {
				modifyPlayerGold(100); // Add 100 gold
			}
			else if (event.key.code == sf::Keyboard::H) {
				modifyPlayerGold(-100); // Subtract 100 gold
			}
		}
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::V) {
				modifyStat(player->getMutableStats(), "wisdom", 1, *player);
			}
			else if (event.key.code == sf::Keyboard::C) {
				modifyStat(player->getMutableStats(), "constitution", 1, *player);
			}
		}
		debugSystem->handleInput();

	}
}

void Game::handleInput(){
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		Room* currentRoom = getCurrentRoom();
		player->attack(map.getEnemies(), map.getMap(), *currentRoom);
	}
}

// Cheats
void Game::modifyPlayerGold(int amount) {
	if (player != nullptr) {
		player->addGold(amount);
		std::cout << "Player gold modified by " << amount << ". New total: " << player->getGold() << std::endl;
	}
}

void Game::refreshShop() {
	// Logic to refresh the shop
	for (Room& room : map.getRooms()) {
		if (room.isMerchantRoom) {
			room.merchantUpgrades = map.upgradeManager().generateUpgrades(3, player->getClassType());
			availableUpgrades = room.merchantUpgrades;
		}
	}
}

void Game::updateHealth() {
	healthText.setString("Health: " + std::to_string(player->getHealth()));
	healthBar->update(player->getHealth(), player->getMaxHealth());
}

void Game::update(float deltaTime) {
	static sf::Clock clock;
	static sf::Time lastMoveTime = sf::Time::Zero;
	sf::Time moveDelay = sf::milliseconds(100); // Adjust delay as needed

	// Detect player death
	if (player->getHealth() <= 0) {
		DeathScreen deathScreen(font, scaleX, scaleY);
		deathScreen.display(window, *this);
	}

	if (clock.getElapsedTime() - lastMoveTime > moveDelay) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			player->move('q', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			player->move('e', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			player->move('z', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			player->move('c', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			player->move('w', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			player->move('s', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player->move('a', map.getMap(), map.getEnemies(), map.getRooms());
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player->move('d', map.getMap(), map.getEnemies(), map.getRooms());
		}
		lastMoveTime = clock.getElapsedTime();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
		screenShake(5.0f, 0.5f);
	}

	player->update(map.getEnemies(), map.getMap());
	updateHealth();

	// Check for upgrade pickup
	for (Room& room : map.getRooms()) {
		if (room.upgradeSpawned && player->getX() == room.upgradePosition.first && player->getY() == room.upgradePosition.second) {
			room.upgradeSpawned = false;
			room.upgradeCollected = true;
			SoundManager::getInstance().playSound("upgrade_pickup");
			handleUpgradePickup();
			break;
		}
	}

	// Check for event room interaction
	for (Room& room : map.getRooms()) {
		if (room.isEventRoom) {
			EventMaps::checkEventInteraction(room, *player, window, font, 24);

		}
	}

	// Check for merchant room interaction
	for (Room& room : map.getRooms()) {
		if (room.isMerchantRoom && shopCloseClock.getElapsedTime().asSeconds() > 0.25f) {
			if (EventMaps::checkMerchantInteraction(room, *player)) {
				if (room.merchantUpgrades.empty()) {
					room.merchantUpgrades = map.upgradeManager().generateUpgrades(3, player->getClassType());
					availableUpgrades = room.merchantUpgrades;
				}

				displayMerchantWindow(room);
				break;
			}
		}
	}

	// Crossfader for music
	static bool inMerchantRoom = false;
	static bool inBossRoom = false;
	static bool inBossCorridor = false;

	bool currentlyInMerchantRoom = player->isInMerchantRoom(map.getRooms());
	bool currentlyInBossRoom = player->isInBossRoom(map.getRooms());
	bool currentlyInBossCorridor = player->isInBossCorridor(map.getRooms());

	if (currentlyInMerchantRoom && !inMerchantRoom) {
		SoundManager::getInstance().crossfadeMusic("main", "shop", 3.0f);
		inMerchantRoom = true;
		inBossRoom = false;
		inBossCorridor = false;
	}
	else if (currentlyInBossRoom && !inBossRoom) {
		SoundManager::getInstance().crossfadeMusic("main", "boss", 3.0f);
		inBossRoom = true;
		inMerchantRoom = false;
		inBossCorridor = false;
	}
	else if (currentlyInBossCorridor && !inBossCorridor) {
		SoundManager::getInstance().playMusic("boss");
		inBossCorridor = true;
		inMerchantRoom = false;
		inBossRoom = false;
	}
	else if (!currentlyInMerchantRoom && inMerchantRoom) {
		SoundManager::getInstance().crossfadeMusic("shop", "main", 3.0f);
		inMerchantRoom = false;
	}
	else if (!currentlyInBossRoom && inBossRoom) {
		SoundManager::getInstance().crossfadeMusic("boss", "main", 3.0f);
		inBossRoom = false;
	}

	// Check for gold drops
	for (Enemy* enemy : map.getEnemies()) {
		if (enemy->isGoldDropped() && !enemy->isGoldAmountAdded()) {
			goldDrops.push_back(std::make_tuple(enemy->getX(), enemy->getY(), enemy->dropGold()));
			enemy->setGoldAmountAdded(true);
		}
	}

	// Check for gold pickup
	for (auto it = goldDrops.begin(); it != goldDrops.end();) {
		if (player->getX() == std::get<0>(*it) && player->getY() == std::get<1>(*it)) {
			SoundManager::getInstance().playSound("gold_pickup");
			player->addGold(std::get<2>(*it)); // Add gold to player
			it = goldDrops.erase(it); // Remove gold drop
		}
		else {
			++it;
		}
	}


	// Handle boss room
	for (Room& room : map.getRooms()) {
		if (room.isBossRoom && !room.bossRoomEntered && player->getX() == room.startX + 2) {
			std::cout << "Player entered boss room" << std::endl;
			enterBossRoom();
			room.bossRoomEntered = true;
			break;
		}
	}


	// Set bossRoomEntered flag to false when the stage counter is 1 past a boss room (multiple of 5)
	if (stageCount % 1 == 1) {
		for (Room& room : map.getRooms()) {
			if (room.isBossRoom) {
				room.bossRoomEntered = false;
			}
		}
	}

	// Update BossRoom 
	if (!bossRoom.isTextComplete()) {
		bossRoom.update(deltaTime);
	}


	// Check if boss defeated and unblock exit
	if (map.isBossDefeated()) {
		map.unblockExit();
	}
	
	debugSystem->update();

	map.advanceToNextLevel(player, this);
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

	// NOW UNUSED BUTTONS
	//for (Button* button : buttons) {
	//	button->draw(window);
	//}

	if (player != nullptr) {
		map.render(window, player->getX(), player->getY(), 24, player);
		player->render(window, 24); // Render the player

		// Render Mage's projectile
		if (Mage* mage = dynamic_cast<Mage*>(player)) {
			mage->renderProjectile(window, 24);
		}
	}

	// Render enemy projectiles
	for (Enemy* enemy : map.getEnemies()) {
		if (SkeletonArcher* archer = dynamic_cast<SkeletonArcher*>(enemy)) {
			archer->renderProjectiles(window, 24);
		}
	}

	if (healthBar != nullptr) {
		healthBar->render(window);
	}

	if (showStats != nullptr && player != nullptr) {
		showStats->renderPlayerStats(window, *player, 24, scaleX, scaleY, nullptr, "");
	}

	// Render stage counter
	showStage->renderStage(window, stageCount, 500, 250, 24, scaleX, scaleY);

	// Render UI total Gold
	showGold->renderGold(window, player->getGold(), 500, 340, 24, scaleX, scaleY);

	// Render gold drops
	renderGoldDrops(window);

	// Render collected upgrades
	displayCollectedUpgrades();

	// Render boss text
	if (!bossRoom.isTextComplete()) {
		bossRoom.render(window);
	}

	// Render boss health bar if boss is present
	for (Enemy* enemy : map.getEnemies()) {
		if (Boss* boss = dynamic_cast<Boss*>(enemy)) {
			boss->renderBossHealthBar(window, 24, scaleX, scaleY);
		}
	}

	window.display();
}

void Game::handleUpgradePickup() {
	availableUpgrades = map.upgradeManager().generateUpgrades(3, player->getClassType());
	// Display available upgrades and allow player to choose
	// Apply the chosen upgrade to the player

	displayUpgradeSelection();

	// EXAMPLE
	/*if (!availableUpgrades.empty()) {
		player->applyUpgrade(availableUpgrades[0]);
		std::cout << "Player picked up: " << availableUpgrades[0].getName() << std::endl;
	}*/
}

void Game::screenShake(float intensity, float duration) {
	isShaking = true;
	shakeIntensity = intensity;
	shakeClock.restart();
}

void Game::renderGoldDrops(sf::RenderWindow& window) {
	for (const auto& goldDrop : goldDrops) {
		sf::Text goldSymbol;
		goldSymbol.setFont(font);
		goldSymbol.setString(".");
		goldSymbol.setCharacterSize(24);
		goldSymbol.setFillColor(sf::Color::Yellow);
		goldSymbol.setPosition(std::get<0>(goldDrop) * 24, std::get<1>(goldDrop) * 24);
		window.draw(goldSymbol);
	}
}

void Game::incrementStageCount() {
	stageCount++;
}

int Game::getStageCount() const {
	return stageCount;
}

void Game::clearGoldDrops() {
	goldDrops.clear();
}

void Game::displayCollectedUpgrades() {
	if (player == nullptr) {
		return;
	}

	const std::vector<Upgrade>& upgrades = player->getCollectedUpgrades();
	std::map<std::string, int> upgradeCount;

	// Count the number of each upgrade
	for (const Upgrade& upgrade : upgrades) {
		upgradeCount[upgrade.getName()]++;
	}

	float startX = 515 * scaleX;
	float startY = 25 * scaleY;
	float offsetY = 20 * scaleY;
	float maxWidth = 60 * scaleX;

	for (const auto& pair : upgradeCount) {
		const std::string& upgradeName = pair.first;
		int count = pair.second;

		sf::Text upgradeText;
		upgradeText.setFont(font);
		upgradeText.setCharacterSize(8 * scaleY);
		std::string displayText = upgradeName + " x" + std::to_string(count);
		upgradeText.setString(wrapText(displayText, font, 8 * scaleY, maxWidth));

		// Set the color based on rarity
		sf::Color color;
		for (const Upgrade& upgrade : upgrades) {
			if (upgrade.getName() == upgradeName) {
				color = upgrade.getColor();
				break;
			}
		}
		upgradeText.setFillColor(color);

		upgradeText.setPosition(startX, startY);
		window.draw(upgradeText);

		startY += offsetY;
	}
}

void Game::enterBossRoom() {
	bossRoom.enter();
	//player->disableMovement();
}

void Game::initialize() {
	// Load sounds
	SoundManager::getInstance().loadSound("goblin_attack", "sounds/goblin_attack.wav");
	SoundManager::getInstance().loadSound("skeleton_attack", "sounds/skeleton_attack.wav");
	SoundManager::getInstance().loadSound("skeleton_hit", "sounds/skeleton_hit.wav");
	SoundManager::getInstance().loadSound("player_attack", "sounds/player_attack.wav");
	SoundManager::getInstance().loadSound("player_damage", "sounds/player_damage.wav");
	SoundManager::getInstance().loadSound("player_dodge", "sounds/player_dodge.wav");
	SoundManager::getInstance().loadSound("enemy_take_damage", "sounds/enemy_take_damage.ogg");
	SoundManager::getInstance().loadSound("burn_damage", "sounds/burn_damage.ogg");
	SoundManager::getInstance().loadSound("enemy_death", "sounds/enemy_death.wav");
	SoundManager::getInstance().loadSound("room_exit_close", "sounds/room_exit_close.wav");
	SoundManager::getInstance().loadSound("upgrade_pickup", "sounds/upgrade_pickup.wav");
	SoundManager::getInstance().loadSound("upgrade_choice", "sounds/upgrade_choice.wav");
	SoundManager::getInstance().loadSound("gold_pickup", "sounds/gold_pickup.wav");

	SoundManager::getInstance().loadSound("havok_charging", "sounds/havok_charging.wav");
	SoundManager::getInstance().loadSound("havok_stun", "sounds/havok_stun.wav");

	// Set volumes for sounds
	SoundManager::getInstance().setSoundVolume("goblin_attack", 20.0f);
	SoundManager::getInstance().setSoundVolume("skeleton_attack", 20.0f);
	SoundManager::getInstance().setSoundVolume("skeleton_hit", 10.0f);
	SoundManager::getInstance().setSoundVolume("player_attack", 100.0f);
	SoundManager::getInstance().setSoundVolume("player_damage", 50.0f);
	SoundManager::getInstance().setSoundVolume("player_dodge", 100.0f);
	SoundManager::getInstance().setSoundVolume("enemy_take_damage", 10.0f);
	SoundManager::getInstance().setSoundVolume("burn_damage", 100.0f);
	SoundManager::getInstance().setSoundVolume("enemy_death", 100.0f);
	SoundManager::getInstance().setSoundVolume("room_exit_close", 100.0f);
	SoundManager::getInstance().setSoundVolume("upgrade_pickup", 15.0f);
	SoundManager::getInstance().setSoundVolume("upgrade_choice", 10.0f);
	SoundManager::getInstance().setSoundVolume("gold_pickup", 30.0f);

	SoundManager::getInstance().setSoundVolume("havok_charging", 40.0f);
	SoundManager::getInstance().setSoundVolume("havok_stun", 30.0f);

	// Randomize pitch
	SoundManager::getInstance().setRandomizedPitch("goblin_attack", true);
	SoundManager::getInstance().setRandomizedPitch("skeleton_attack", true);
	SoundManager::getInstance().setRandomizedPitch("skeleton_hit", true);
	SoundManager::getInstance().setRandomizedPitch("player_attack", true);
	SoundManager::getInstance().setRandomizedPitch("player_damage", true);
	SoundManager::getInstance().setRandomizedPitch("player_dodge", true);
	SoundManager::getInstance().setRandomizedPitch("enemy_take_damage", true);
	SoundManager::getInstance().setRandomizedPitch("burn_damage", true);
	SoundManager::getInstance().setRandomizedPitch("enemy_death", true);
	SoundManager::getInstance().setRandomizedPitch("room_exit_close", true);
	SoundManager::getInstance().setRandomizedPitch("upgrade_pickup", true);
	SoundManager::getInstance().setRandomizedPitch("upgrade_choice", true);
	SoundManager::getInstance().setRandomizedPitch("gold_pickup", true);

	SoundManager::getInstance().setRandomizedPitch("havok_charging", false);
	SoundManager::getInstance().setRandomizedPitch("havok_stun", true);

	// Load music
	SoundManager::getInstance().loadMusic("main", "music/main.wav");
	SoundManager::getInstance().loadMusic("shop", "music/shop.wav");
	SoundManager::getInstance().loadMusic("boss", "music/boss.ogg");

	// Set volumes for music
	SoundManager::getInstance().setMusicVolume("main", 10.0f);
	SoundManager::getInstance().setMusicVolume("shop", 10.0f);
	SoundManager::getInstance().setMusicVolume("boss", 10.0f);

	// Play main theme
	SoundManager::getInstance().playMusic("main");
}

void Game::restartGame() {
	// Reset game state
	delete player;
	delete healthBar;
	delete showStats;
	delete showGold;
	delete showStage;

	player = nullptr;
	healthBar = nullptr;
	showStats = nullptr;
	showGold = new ShowGold();
	showStage = new ShowStage();
	stageCount = 0;
	player->totalKillCount = 0;
	player->bossKillCount = 0;

	// Reset enemy scaling
	map.getEnemyManager()->resetScaleFactors();

	// Reinitialize game
	initialize();
	map.generate();
	originalView = window.getDefaultView();
	window.setView(originalView);
	displayCharacterSelection();
}

Room* Game::getCurrentRoom() {
	for (Room& room : map.getRooms()) {
		if (player->getX() >= room.startX && player->getX() <= room.endX &&
			player->getY() >= room.startY && player->getY() <= room.endY) {
			return &room;
		}
	}
	return nullptr;
}

std::pair<int, int> Game::getPlayerKillCount() {
	if (player != nullptr) {
		return { player->totalKillCount, player->bossKillCount };
	}
	return { 0, 0 };
}