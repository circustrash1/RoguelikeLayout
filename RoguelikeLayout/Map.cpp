#include "Map.h" // Include the Map class header
#include "EnemyManager.h"
#include "Player.h"
#include "EventMaps.h"
#include "SoundManager.h"
#include "Enemies.h"
#include "StatManager.h"
#include <stack> // Include stack for the DFS algorithm
#include <queue> // Include queue for flood fill algo
#include <cstdlib> // Include cstdlib for std::srand and std::rand
#include <ctime> // Include ctime for std::time
#include <algorithm> // Include algorithm for std::shuffle
#include <random> // Include random for std::default_random_engine
#include <iostream>
#include "Game.h"

Map::Map(int width, int height) : width(width), height(height), roomCount(0), firstGeneration(true), enemyManager(new EnemyManager()) {
	map.resize(height, std::vector<char>(width, '#')); // Initialize the map with walls ('#')
	revealed.resize(height, std::vector<bool>(width, false)); // Initialize the revealed array with false
	enemyManager = new EnemyManager();
}

Map::~Map() {
	delete enemyManager;
}

void Map::generate() {
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random number generator
	std::default_random_engine rng(std::rand());

	bool successfulGeneration = false;

	generationCount++;

	if (generationCount % 5 == 0) {
		generateBossCorridor();
		return;
	}

	do {
		entryPoints.clear();
		exitPoints.clear(); // Clear exitPoints as well
		rooms.clear();
		roomUpgrades.clear();
		roomUpgrades.clear();
		enemyManager->clearEnemies();
		revealed.assign(height, std::vector<bool>(width, false)); // Reset the revealed array

		// Reset the map to walls
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				map[y][x] = '#';
			}
		}

		roomCount = 0;  // Reset room count

		int roomAttempts = 0;
		int maxRoomAttempts = 100;
		while (roomAttempts < maxRoomAttempts && roomCount < maxRooms) {
			// Try placing rooms
			int x = std::rand() % (width - 6) + 3; // Random X within bounds
			int y = std::rand() % (height - 6) + 3; // Random Y within bounds

			carveRoom(x, y, rng); // Attempt to carve a room
			roomAttempts++;
		}

		// Convert pre-existing rooms to merchant and event rooms
		for (size_t i = 0; i < rooms.size(); ++i) {
			int roomType = std::rand() % 15;
			if (roomType == 0) {
				EventMaps::convertToMerchantRoom(rooms[i]);
			}
			else if (roomType == 1) {
				EventMaps::convertToEventRoom(rooms[i]);
			}
		}

		carve(2, 2); // Start carving paths from position (1, 1)

		wallRooms();

		map[32 / 2][62 / 2] = '.'; // Mark the starting position as a path ('.')

		// Create a 2-wall border around the map
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (y < 2 || y >= height - 2 || x < 2 || x >= width - 2) {
					map[y][x] = '#';
				}
			}
		}

		chooseCarveExits();

		connectRegions();

		if (isConnected() && calculatePathPercentage() >= 35.0) {
			successfulGeneration = true;
			debug = false;
			break; // Successful generation, exit the loop
		}

		rooms.clear(); // Clear the rooms vector on unsuccessful generation
	} while (true); // Repeat until a successful generation
}

double Map::calculatePathPercentage() const {
	int pathCount = 0;
	int totalCells = width * height;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == '.') {
				pathCount++;
			}
		}
	}

	return (static_cast<double>(pathCount) / totalCells) * 100;
}

void Map::debugRender(sf::RenderWindow& window, int charSize) {
	window.clear(sf::Color::Black);

	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == '.') {
				text.setFillColor(sf::Color::White);
			}
			else {
				text.setFillColor(sf::Color::Red);
			}
			text.setString(map[y][x]);
			text.setPosition(x * charSize, y * charSize);
			window.draw(text);
		}
	}

	window.display();
}

void Map::floodVisualise(sf::RenderWindow& window, int charSize, const std::vector<std::vector<bool>>& visited) {
	window.clear(sf::Color::Black);

	sf::Font font;
	if (!font.loadFromFile("fs-min.ttf")) {
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (visited[y][x]) {
				text.setFillColor(sf::Color::Green);        // Visited
			}
			else if (map[y][x] == '#') {
				text.setFillColor(sf::Color::Red);          // Unvisited
			}
			text.setString(map[y][x]);
			text.setPosition(x * charSize, y * charSize);
			window.draw(text);
		}
	}

	window.display();
}

void Map::carve(int x, int y) {
	std::stack<std::pair<int, int>> stack; // Create a stack for the DFS algorithm
	stack.push({ x, y }); // Push the starting position onto the stack
	map[y][x] = '.'; // Mark the starting position as a path ('.')

	std::default_random_engine rng(static_cast<unsigned int>(std::time(nullptr))); // Create a random number generator

	sf::RenderWindow* window = nullptr;
	if (debug) {
		window = new sf::RenderWindow(sf::VideoMode(width * 10, height * 10), "Room Generation Debug");
	}

	while (!stack.empty()) { // While there are positions on the stack
		auto [cx, cy] = stack.top(); // Get the current position from the top of the stack
		stack.pop(); // Remove the current position from the stack

		if (roomCount < maxRooms && std::rand() % 4 == 0) { // 25% chance to carve a room
			carveRoom(cx, cy, rng); // Attempt to carve a room
		}

		carveCorridor(cx, cy, rng, stack); // Always carve corridors

		// Debug render after each step
		if (debug) {
			debugRender(*window, 10);
		}
	}

	// Close and delete the debug window if it was created
	if (debug && window) {
		window->close();
		delete window;
	}
}

void Map::carveRoom(int x, int y, std::default_random_engine& rng) {
	std::uniform_int_distribution<int> roomWidthDist(10, 15);  // Room width range
	std::uniform_int_distribution<int> roomHeightDist(10, 15); // Room height range

	int roomWidth = roomWidthDist(rng);  // Generate a random room width
	int roomHeight = roomHeightDist(rng); // Generate a random room height

	// Calculate the bounds of the room
	int startX = x - roomWidth / 2;
	int startY = y - roomHeight / 2;
	int endX = startX + roomWidth;
	int endY = startY + roomHeight;

	// Ensure room is within bounds
	if (startX <= 3 || startY <= 2 || endX >= width - 3 || endY >= height - 2) {
		return; // If out of bounds, do not carve the room
	}

	// Try to place the room and ensure no overlap
	bool canPlaceRoom = true;
	for (int ny = startY; ny < endY; ++ny) {
		for (int nx = startX; nx < endX; ++nx) {
			if (map[ny][nx] == '.') {
				canPlaceRoom = false; // If any space is already a path, the room can't be placed
				break;
			}
		}
		if (!canPlaceRoom) break;
	}

	if (!canPlaceRoom) {
		return; // Exit if room overlaps existing paths
	}

	// Carve the room if valid
	for (int ny = startY; ny < endY; ++ny) {
		for (int nx = startX; nx < endX; ++nx) {
			if (map[ny][nx] == '#') { // Only carve if it's a wall
				map[ny][nx] = '.'; // Carve the room
			}
		}
	}

	rooms.push_back({ startX, startY, endX, endY });

	roomCount++;
}

void Map::wallRooms(bool blockEntries) {
	sf::RenderWindow* window = nullptr;
	if (debug) {
		window = new sf::RenderWindow(sf::VideoMode(width * 10, height * 10), "Wall Rooms Debug");
	}

	// Iterate through each room
	for (const Room& room : rooms) {
		if (!room.wallOff || room.isBossRoom) {
			continue;	// Skip if the room is not to be walled off or a boss room
		}

		int startX = room.startX;
		int startY = room.startY;
		int endX = room.endX;
		int endY = room.endY;

		// Wall off the perimeter of the room
		for (int nx = startX - 1; nx <= endX; ++nx) {
			map[startY - 1][nx] = '#'; // Top wall
			if (debug) {
				debugRender(*window, 10);
				sf::sleep(sf::milliseconds(10)); // Delay for visualization
			}
			map[endY][nx] = '#'; // Bottom wall
			if (debug) {
				debugRender(*window, 10);
				sf::sleep(sf::milliseconds(10)); // Delay for visualization
			}
		}
		for (int ny = startY - 1; ny <= endY; ++ny) {
			map[ny][startX - 1] = '#'; // Left wall
			if (debug) {
				debugRender(*window, 10);
				sf::sleep(sf::milliseconds(10)); // Delay for visualization
			}
			map[ny][endX] = '#'; // Right wall
			if (debug) {
				debugRender(*window, 10);
				sf::sleep(sf::milliseconds(10)); // Delay for visualization
			}
		}
	}
	if (debug) {
		window->close();
		delete window;
	}
}

void Map::chooseCarveExits() {
	// Iterate through each room
	for (Room& room : rooms) {
		if (room.isBossRoom) {
			continue; // Skip if the room is a boss room
		}
		int startX = room.startX;
		int startY = room.startY;
		int endX = room.endX;
		int endY = room.endY;

		// Select entry and exit points on the perimeter
		std::vector<std::pair<int, int>> perimeter;
		for (int nx = startX; nx < endX; ++nx) {
			int adjustedStartY = startY - 2;
			int adjustedEndY = endY;
			while (adjustedStartY <= 0) adjustedStartY++;
			while (adjustedEndY >= height - 2) adjustedEndY--;
			if (nx != startX && nx != endX - 1) {
				perimeter.push_back({ nx, adjustedStartY }); // Top edge
				perimeter.push_back({ nx, adjustedEndY }); // Bottom edge
			}
		}
		for (int ny = startY; ny < endY; ++ny) {
			int adjustedStartX = startX - 2;
			int adjustedEndX = endX;
			while (adjustedStartX <= 0) adjustedStartX++;
			while (adjustedEndX >= width - 2) adjustedEndX--;
			if (ny != startY && ny != endY - 1) {
				perimeter.push_back({ adjustedStartX, ny }); // Left edge
				perimeter.push_back({ adjustedEndX, ny }); // Right edge
			}
		}

		// Lambda to check if the perimeter point is adjacent to a corridor ('.')
		auto isCorridorAdjacent = [&](int px, int py) {
			for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
				int adjX = px + dx;
				int adjY = py + dy;
				if (adjX >= 0 && adjY >= 0 && adjX < width && adjY < height && map[adjY][adjX] == '.') {
					return true; // Adjacent to a corridor
				}
			}
			return false;
			};

		// Lambda to check if a point is part of the room wall
		auto isRoomWall = [&](int x, int y) {
			return (x == startX - 1 || x == endX || y == startY - 1 || y == endY);
			};

		// Keep looping until at least 2 entry and 2 exit points are found for the current room
		int entryCount = 0;
		int exitCount = 0;
		while (entryCount < 2 || exitCount < 2) {
			std::shuffle(perimeter.begin(), perimeter.end(), std::default_random_engine());

			for (size_t i = 0; i < perimeter.size(); ++i) {
				auto [px, py] = perimeter[i];
				if (isCorridorAdjacent(px, py)) {
					if (entryCount < 2) {
						if (px > 0 && py > 0 && px < width - 1 && py < height - 1) {
							map[py][px] = '.'; // Carve the entry point
							entryPoints.push_back({ px, py }); // Store the entry point
							entryCount++;
						}
					}
					else if (exitCount < 2) {
						if (px > 0 && py > 0 && px < width - 1 && py < height - 1) {
							map[py][px] = '.'; // Carve the exit point
							exitPoints.push_back({ px, py }); // Store the exit point
							exitCount++;
						}
					}

					// Ensure the entry/exit point is connected to a path
					for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
						int adjX = px + dx;
						int adjY = py + dy;
						if (adjX >= 0 && adjY >= 0 && adjX < width && adjY < height && map[adjY][adjX] == '#' && !isRoomWall(adjX, adjY)) {
							map[adjY][adjX] = '.'; // Carve the adjacent wall tile
							if (entryCount < 2) {
								entryPoints.push_back({ adjX, adjY }); // Store the adjacent point
							}
							else if (exitCount < 2) {
								exitPoints.push_back({ adjX, adjY }); // Store the adjacent point
							}
						}
					}
				}

				if (entryCount >= 2 && exitCount >= 2) {
					break;
				}
			}
		}
	}

	// Create an exit room at the border of the map - right side
	int exitX = width - 3;
	int exitY = height / 2;
	for (int i = -1; i <= 1; ++i) {
		map[exitY + i][exitX] = '.'; // Create a large opening in the border
		map[exitY + i][exitX + 1] = '.';
		map[exitY + i][exitX + 2] = '.';
	}

	for (Room& room : rooms) {		// Wall off exit if boss room
		if (room.isBossRoom) {
			for (int i = -1; i <= 1; ++i) {
				map[exitY + i][exitX + 1] = '#';
				map[exitY + i][exitX + 2] = '#';
			}
		}
	}

	// Create a symmetrical exit on the left side of the map
	if (!firstGeneration) {
		int symExitX = 2;
		for (int i = -1; i <= 1; ++i) {
			map[exitY + i][symExitX] = '.'; // Create a large opening in the border
			map[exitY + i][symExitX - 1] = '.';
			map[exitY + i][symExitX - 2] = '.';
		}
	}
}

void Map::carveExits() {
	// Define the condition to check (e.g., a specific key press or a game event)

	if (enemyManager->allEnemiesDead() || (sf::Keyboard::isKeyPressed(sf::Keyboard::R))) {
		if (entryPoints.empty()) {
			std::cout << "No entry points to carve." << std::endl; // Debug output
		}
		else {
			for (const auto& entryPoint : entryPoints) {
				map[entryPoint.second][entryPoint.first] = '.'; // Carve the wall at the entry point
			}
			for (const auto& exitPoint : exitPoints) {
				map[exitPoint.second][exitPoint.first] = '.'; // Carve the wall at the entry point
			}
		}
	}
}

void Map::carveCorridor(int cx, int cy, std::default_random_engine& rng, std::stack<std::pair<int, int>>& stack) {
	std::vector<std::pair<int, int>> directions = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} }; // Define possible directions to move
	std::shuffle(directions.begin(), directions.end(), rng); // Shuffle the directions to randomize the path carving

	for (auto [dx, dy] : directions) { // For each direction
		int nx = cx + dx; // Calculate the new x position
		int ny = cy + dy; // Calculate the new y position
		int mx = cx + dx / 2; // Calculate the x position between the current and new positions
		int my = cy + dy / 2; // Calculate the y position between the current and new positions

		if (nx > 1 && ny > 1 && nx < width - 1 && ny < height - 1 && map[ny][nx] == '#') { // If the new position is within bounds and is a wall
			map[ny][nx] = '.'; // Mark the new position as a path
			map[my][mx] = '.'; // Mark the position between as a path
			stack.push({ nx, ny }); // Push the new position onto the stack
		}
	}
}

void Map::render(sf::RenderWindow& window, int playerX, int playerY, int charSize, Player* player) {
	revealArea(playerX, playerY, player->getStatManager().getViewDistance() - (player->getStats().wisdom * 1.75) + 5); // Reveal an area around the player

	sf::Font font; // Create a font object
	if (!font.loadFromFile("fs-min.ttf")) { // Load the font
		return; // If the font fails to load, return
	}

	if (playerInRoom(playerX, playerY)) {
		wallRooms(true); // Wall off rooms, blocking entries
		for (Room& room : rooms) {
			if (playerX >= room.startX && playerX < room.endX && playerY >= room.startY && playerY < room.endY) {
				if (room.isMerchantRoom) {
					EventMaps::handleMerchantRoom(room, *player);
				}
				else if (room.isEventRoom) {
					EventMaps::handleEventRoom(room, *player, window, font, charSize);
				}
				else if (!room.enemiesSpawned && !room.isBossRoom) {
					int roomWidth = room.endX - room.startX;
					int roomHeight = room.endY - room.startY;

					enemyManager->spawnEnemies(room.startX, room.startY, roomWidth, roomHeight, map, room);

					enemyManager->spawnEnemies(room.startX, room.startY, roomWidth, roomHeight, map, room);

					room.enemiesSpawned = true;
				}
				updateEnemies(playerX, playerY, player);
				break;
			}
		}
	}

	carveExits();

	sf::Text text; // Create a text object
	text.setFont(font); // Set the font for the text
	text.setCharacterSize(charSize); // Set the character size

	const int maxRadius = player->getStatManager().getViewDistance() - (player->getStats().wisdom * 1.75) + 5; // Maximum radius for the torch effect
	const int maxBrightness = 255; // Maximum brightness level
	const int minBrightness = 50; // Minimum brightness level for walls
	const float fadeOut = 0.25f; // Fade out factor for the torch effect

	for (int y = 0; y < height; ++y) { // For each row in the map
		for (int x = 0; x < width; ++x) { // For each column in the map
			if (revealed[y][x]) { // If the position is revealed
				// Calculate Euclidean distance from the player to create a circular radius effect
				float distance = std::sqrt(std::pow(playerX - x, 2) + std::pow(playerY - y, 2)); // Euclidean distance

				if (distance <= maxRadius) { // If within the torch radius
					// Calculate exponential brightness fading based on distance from the player
					float brightness = maxBrightness * std::exp(-fadeOut * distance);
					brightness = std::max(static_cast<float>(minBrightness), brightness); // Ensure brightness doesn't go below minimum

					// Calculate the red intensity based on distance
					float redIntensity = brightness;  // Directly use brightness for red

					// Calculate the green and blue intensity, which will be lower to give the red "torch" effect
					float greenBlueIntensity = minBrightness + (brightness - minBrightness) * 0.2f; // Slightly higher green/blue values as it fades

					// Red stays dominant, while green and blue components fade out gradually
					sf::Uint8 red = static_cast<sf::Uint8>(redIntensity);
					sf::Uint8 greenBlue = static_cast<sf::Uint8>(greenBlueIntensity);

					text.setFillColor(sf::Color(red, greenBlue, greenBlue)); // Set color for torchlight effect
				}
				else {
					text.setFillColor(sf::Color(50, 50, 50, 80)); // Set color to grey for characters outside the max radius
				}

				text.setString(map[y][x]); // Set the text to the map character
				text.setPosition(x * charSize, y * charSize); // Adjust position based on charSize
				window.draw(text); // Draw the text
			}
		}
	}

	// Render the player character separately to ensure its color is not affected by the torch effect
	text.setString('@'); // Set the text to the player character
	text.setFillColor(sf::Color::White); // Set the player color to white
	text.setPosition(playerX * charSize, playerY * charSize); // Adjust position based on charSize
	window.draw(text); // Draw the player

	// Render the upgrade pickups
	for (Room& room : rooms) {
		if (room.upgradeSpawned) {
			text.setString("U"); // Set the text to the upgrade character
			text.setFillColor(sf::Color::Color(218, 165, 32));
			text.setPosition(room.upgradePosition.first * charSize, room.upgradePosition.second * charSize);
			window.draw(text);
		}
	}

	// Render the merchant
	for (Room& room : rooms) {
		if (room.isMerchantRoom && room.merchantSpawned) {
			text.setString('$');
			text.setFillColor(sf::Color::Yellow);
			text.setPosition(room.merchantPosition.first * charSize, room.merchantPosition.second * charSize);
			window.draw(text);
		}
	}

	// Render the event character
	for (Room& room : rooms) {
		if (room.eventCharVisible) {
			text.setString('?');
			text.setFillColor(sf::Color::Magenta);
			text.setPosition(room.eventCharPosition.first * charSize, room.eventCharPosition.second * charSize);
			window.draw(text);
		}

		if (room.eventTextClock.getElapsedTime() < room.eventTextDuration) {
			text.setString(room.eventText);
			text.setFillColor(sf::Color::White);
			text.setCharacterSize(24);
			text.setPosition(450, 400);
			window.draw(text);
		}

		EventMaps::renderSkillCheckText(room, *player, window, font, charSize);
	}

	enemyManager->renderEnemies(window, charSize, playerX, playerY, player, map);
}

void Map::updateEnemies(int playerX, int playerY, Player* player) {
	enemyManager->updateEnemies(map, playerX, playerY, player);
	for (Room& room : rooms) {
		if (playerX >= room.startX && playerX < room.endX && playerY >= room.startY && playerY < room.endY) {
			if (!room.upgradeSpawned && !room.upgradeCollected && enemyManager->allEnemiesDead() && !room.isMerchantRoom && !room.isEventRoom) {
				dropUpgrade(room);
				room.upgradeSpawned = true;
			}
			break;
		}
	}

	// Check if the boss is defeated
	for (Enemy* enemy : enemyManager->getEnemies()) {
		if (Boss* boss = dynamic_cast<Boss*>(enemy)) {
			if (boss->getHealth() <= 0) {
				setBossDefeated(true);
			}
		}
	}
}

bool Map::playerInRoom(int playerX, int playerY) const {
	for (const Room& room : rooms) {
		if (playerX >= room.startX && playerX < room.endX && playerY >= room.startY && playerY < room.endY) {
			return true;
		}
	}
	return false;
}

bool Map::isWalkable(int x, int y) const {
	return x >= 0 && y >= 0 && x < width && y < height && map[y][x] == '.'; // Check if the position is within bounds and is a path
}

void Map::reveal(int x, int y) {
	if (x >= 0 && y >= 0 && x < width && y < height) { // If the position is within bounds
		revealed[y][x] = true; // Mark the position as revealed
	}
}

void Map::revealArea(int x, int y, int radius) {
	for (int dy = -radius; dy <= radius; ++dy) { // For each row in the radius
		for (int dx = -radius; dx <= radius; ++dx) { // For each column in the radius
			// Calculate the Euclidean distance from the center (x, y) to (x+dx, y+dy)
			float distance = std::sqrt(dx * dx + dy * dy);

			if (distance <= radius) { // If the point is within the circle's radius
				if (x + dx >= 0 && y + dy >= 0 && x + dx < width && y + dy < height) { // Ensure within bounds
					revealed[y + dy][x + dx] = true; // Mark the position as revealed
				}
			}
		}
	}
}

void Map::floodFill(int x, int y, std::vector<std::vector<bool>>& visited) {
	std::queue<std::pair<int, int>> queue;
	queue.push({ x, y });
	visited[y][x] = true;

	sf::RenderWindow* window = nullptr;
	if (debug) {
		window = new sf::RenderWindow(sf::VideoMode(width * 10, height * 10), "Flood Fill Debug");
	}

	while (!queue.empty()) {
		auto [cx, cy] = queue.front();
		queue.pop();

		// Directions for exploring up, down, left, right
		for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
			int nx = cx + dx;
			int ny = cy + dy;

			// If the neighbor is within bounds and is a path tile and hasn't been visited
			if (nx >= 0 && ny >= 0 && nx < width && ny < height && map[ny][nx] == '.' && !visited[ny][nx]) {
				visited[ny][nx] = true;
				queue.push({ nx, ny });
			}
		}

		if (debug) {
			floodVisualise(*window, 10, visited);
		}
	}
	// Close and delete the debug window if it was created
	if (debug && window) {
		window->close();
		delete window;
	}
}

bool Map::isConnected() {
	std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

	// Find the first path tile ('.') to start flood fill
	bool foundStart = false;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == '.') {
				floodFill(x, y, visited);  // Perform flood fill from this tile
				foundStart = true;
				break;
			}
		}
		if (foundStart) break;
	}

	// Check if all path tiles are visited
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == '.' && !visited[y][x]) {
				return false;  // If any path tile is not visited, return false
			}
		}
	}

	return true;  // All path tiles are connected
}

void Map::connectRegions() {
	sf::RenderWindow* window = nullptr;
	if (debug) {
		window = new sf::RenderWindow(sf::VideoMode(width * 10, height * 10), "Connect Regions Debug");
	}
	std::vector<std::vector<int>> regionMap(height, std::vector<int>(width, -1));
	int regionCount = 0;

	// Identify regions using flood fill
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == '.' && regionMap[y][x] == -1) {
				floodFillRegion(x, y, regionCount, regionMap);
				regionCount++;
			}
		}
	}

	// Connect regions
	for (int i = 0; i < regionCount - 1; ++i) {
		for (int j = i + 1; j < regionCount; ++j) {
			connectTwoRegions(i, j, regionMap, window);
		}
	}

	// Close and delete the debug window if it was created
	if (debug && window) {
		window->close();
		delete window;
	}
}

void Map::floodFillRegion(int x, int y, int region, std::vector<std::vector<int>>& regionMap) {
	std::queue<std::pair<int, int>> queue;
	queue.push({ x, y });
	regionMap[y][x] = region;

	sf::RenderWindow* window = nullptr;
	if (debug) {
		window = new sf::RenderWindow(sf::VideoMode(width * 10, height * 10), "Flood Fill Region Debug");
	}

	while (!queue.empty()) {
		auto [cx, cy] = queue.front();
		queue.pop();

		for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
			int nx = cx + dx;
			int ny = cy + dy;

			if (nx >= 0 && ny >= 0 && nx < width && ny < height && map[ny][nx] == '.' && regionMap[ny][nx] == -1) {
				regionMap[ny][nx] = region;
				queue.push({ nx, ny });

				if (debug) {
					// Create a visited array for visualization
					std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
					for (int y = 0; y < height; ++y) {
						for (int x = 0; x < width; ++x) {
							if (regionMap[y][x] == region) {
								visited[y][x] = true;
							}
						}
					}
					floodVisualise(*window, 10, visited);
				}
			}
		}
	}

	if (debug) {
		window->close();
		delete window;
	}
}

void Map::connectTwoRegions(int region1, int region2, const std::vector<std::vector<int>>& regionMap, sf::RenderWindow* window) {
	std::vector<std::pair<int, int>> region1Cells;
	std::vector<std::pair<int, int>> region2Cells;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (regionMap[y][x] == region1) {
				region1Cells.push_back({ x, y });
			}
			else if (regionMap[y][x] == region2) {
				region2Cells.push_back({ x, y });
			}
		}
	}

	std::pair<int, int> bestCell1, bestCell2;
	int bestDistance = std::numeric_limits<int>::max();

	for (const auto& cell1 : region1Cells) {
		for (const auto& cell2 : region2Cells) {
			// Exclude cells near the map border
			if (cell1.first <= 1 || cell1.first >= width - 2 || cell1.second <= 1 || cell1.second >= height - 2) {
				continue;
			}

			int distance = std::abs(cell1.first - cell2.first) + std::abs(cell1.second - cell2.second);
			if (distance < bestDistance) {
				bestDistance = distance;
				bestCell1 = cell1;
				bestCell2 = cell2;
			}
		}
	}

	carvePath(bestCell1.first, bestCell1.second, bestCell2.first, bestCell2.second, window);
}

void Map::carvePath(int x1, int y1, int x2, int y2, sf::RenderWindow* window) {
	while (x1 != x2 || y1 != y2) {
		if (x1 != x2) {
			x1 += (x2 > x1) ? 1 : -1;
		}
		if (y1 != y2) {
			y1 += (y2 > y1) ? 1 : -1;
		}
		if (map[y1][x1] == '#' && !isRoomWall(x1, y1)) {
			map[y1][x1] = '.';
			if (debug) {
				debugRender(*window, 10);
				sf::sleep(sf::milliseconds(50)); // Delay for visualization
			}
		}
	}
}

bool Map::isRoomWall(int x, int y) const {
	for (const Room& room : rooms) {
		if ((x == room.startX - 1 || x == room.endX || y == room.startY - 1 || y == room.endY) &&
			(x >= room.startX - 1 && x <= room.endX && y >= room.startY - 1 && y <= room.endY)) {
			return true;
		}
	}
	return false;
}

const std::vector<std::vector<char>>& Map::getMap() const {
	return map;
}

const std::vector<Enemy*>& Map::getEnemies() const {
	return enemyManager->getEnemies(); // Assuming EnemyManager has a getEnemies method
}

void Map::dropUpgrade(Room& room) {
	if (!room.isMerchantRoom && !room.isEventRoom) {
		int centerX = (room.startX + room.endX) / 2;
		int centerY = (room.startY + room.endY) / 2;	// Drop an upgrade in the center of the room
		std::cout << "Dropped upgrade: at (" << centerX << ", " << centerY << ")\n";
		room.upgradePosition = { centerX, centerY };
	}
}

UpgradeManager& Map::upgradeManager() {
	return upgradeManager_;
}

std::vector<Room>& Map::getRooms() {
	return rooms;
}

void Map::generateBossCorridor() {
	// Reset relevant variables
	entryPoints.clear();
	exitPoints.clear();
	rooms.clear();
	roomUpgrades.clear();
	enemyManager->clearEnemies();
	revealed.assign(height, std::vector<bool>(width, false)); // Reset the revealed array

	// Reset the map to walls
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			map[y][x] = '#';
		}
	}

	// Set the map dimensions for the corridor
	width = 62;
	height = 32;

	// Define the corridor boundaries
	int corridorY = height / 2;

	// Create walls and path tiles
	for (int x = 0; x < width; ++x) {
		for (int y = corridorY - 1; y <= corridorY + 1; ++y) {
			map[y][x] = '.'; // Path tile
		}
	}

	// Create entry and exit points
	entryPoints.push_back({ 0, corridorY });
	exitPoints.push_back({ width - 1, corridorY });

	// Create a merchant room in the corridor
	int merchantX = width / 2;
	int merchantY = corridorY;
	Room merchantRoom = { merchantX - 6, merchantY - 1, merchantX + 6, merchantY + 2 };
	merchantRoom.isMerchantRoom = true;
	merchantRoom.merchantSpawned = true;
	merchantRoom.merchantPosition = { merchantX, merchantY };
	merchantRoom.wallOff = false;
	rooms.push_back(merchantRoom);

	// Mark the merchant position on the map
	map[merchantY][merchantX] = '$';
}

void Map::generateBossRoom() {
	// Reset relevant variables
	entryPoints.clear();
	exitPoints.clear();
	rooms.clear();
	roomUpgrades.clear();
	enemyManager->clearEnemies();
	revealed.assign(height, std::vector<bool>(width, false)); // Reset the revealed array

	// Reset the map to paths
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			map[y][x] = '.';
		}
	}

	// Create a 2-wall border around the map
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (y < 2 || y >= height - 2 || x < 2 || x >= width - 2) {
				map[y][x] = '#';
			}
		}
	}

	// Define the boss room boundaries
	int bossRoomWidth = width;
	int bossRoomHeight = height;

	// Create boss room
	Room bossRoom = { 0, 0, bossRoomWidth, bossRoomHeight };
	bossRoom.isBossRoom = true;
	rooms.push_back(bossRoom);

	enemyManager->spawnRandomBoss(bossRoomWidth / 2, bossRoomHeight / 2);
}

EnemyManager* Map::getEnemyManager() {
	return enemyManager;
}

void Map::setBossDefeated(bool defeated) {
	bossDefeated = defeated;
}

bool Map::isBossDefeated() const {
	return bossDefeated;
}

void Map::unblockExit() {
	int exitX = width - 3;
	int exitY = height / 2;
	for (int i = -1; i <= 1; ++i) {
		map[exitY + i][exitX + 1] = '.';
		map[exitY + i][exitX + 2] = '.';
	}
}

void Map::advanceToNextLevel(Player* player, Game* game) {
	// Check if the player is at the exit
	if (player->getX() == 61) {
		firstGeneration = false;
		if (generationCount % 5 == 0) {
			generateBossRoom();
			chooseCarveExits();
		}
		else {
			generate();
		}

		player->setPosition(0, player->getY()); // Move the player to the center of the new map
		game->clearGoldDrops();
		game->incrementStageCount();

		playerAdvanced = true;

		enemyManager->incrementScaleFactors();
	}
}