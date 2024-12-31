#include "Map.h" // Include the Map class header
#include "EnemyManager.h"
#include <stack> // Include stack for the DFS algorithm
#include <queue> // Include queue for flood fill algo
#include <cstdlib> // Include cstdlib for std::srand and std::rand
#include <ctime> // Include ctime for std::time
#include <algorithm> // Include algorithm for std::shuffle
#include <random> // Include random for std::default_random_engine
#include <iostream>
#include "Game.h"

// TODO: Stored entry/exit points. Use them to carve walls after enemy condition is met

Map::Map(int width, int height) : width(width), height(height), roomCount(0) {
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

	do {
		entryPoints.clear();
		exitPoints.clear(); // Clear exitPoints as well

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

		carve(2, 2); // Start carving paths from position (1, 1)

		wallRooms();

		map[32 / 2][62 / 2] = '.'; // Mark the starting position as a path ('.')
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

	sf::RenderWindow window(sf::VideoMode(width * 10, height * 10), "Room Generation Debug");

	while (!stack.empty()) { // While there are positions on the stack
		auto [cx, cy] = stack.top(); // Get the current position from the top of the stack
		stack.pop(); // Remove the current position from the stack

		if (roomCount < maxRooms && std::rand() % 4 == 0) { // 25% chance to carve a room
			carveRoom(cx, cy, rng); // Attempt to carve a room
		}

		carveCorridor(cx, cy, rng, stack); // Always carve corridors

		// Debug render after each step
		if (debug) {
			debugRender(window, 10);
		}
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
	if (startX <= 0 || startY <= 0 || endX >= width || endY >= height) {
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
	for (const Room& room : rooms) {
		int startX = room.startX;
		int startY = room.startY;
		int endX = room.endX;
		int endY = room.endY;

		// Select entry and exit points on the perimeter
		std::vector<std::pair<int, int>> perimeter;
		for (int nx = startX; nx < endX; ++nx) {
			if (nx != startX && nx != endX - 1) {
				perimeter.push_back({ nx, startY - 1 }); // Top edge
				perimeter.push_back({ nx, endY }); // Bottom edge
			}
		}
		for (int ny = startY; ny < endY; ++ny) {
			if (ny != startY && ny != endY - 1) {
				perimeter.push_back({ startX - 1, ny }); // Left edge
				perimeter.push_back({ endX, ny }); // Right edge
			}
		}

		// Shuffle the perimeter to randomize selection
		std::shuffle(perimeter.begin(), perimeter.end(), std::default_random_engine());

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

		// Find entry and exit points that connect to corridors
		int entryIndex = -1, exitIndex = -1;
		for (size_t i = 0; i < perimeter.size(); ++i) {
			auto [px, py] = perimeter[i];
			std::cout << "Checking perimeter point: (" << px << ", " << py << ")\n"; // Debug output
			if (isCorridorAdjacent(px, py)) {
				if (entryIndex == -1) {
					entryIndex = i;
				}
				else {
					exitIndex = i;
					break;
				}
			}
		}

		// Lambda to check if a point is part of the room wall
		auto isRoomWall = [&](int x, int y) {
			return (x == startX - 1 || x == endX || y == startY - 1 || y == endY);
			};

		// Carve the entry point
		if (entryIndex != -1) {
			auto [ex, ey] = perimeter[entryIndex];
			if (ex > 0 && ey > 0 && ex < width - 1 && ey < height - 1) {
				map[ey][ex] = '.'; // Carve the entry point
				entryPoints.push_back({ ex, ey }); // Store the entry point
			}
			std::cout << "Entry point stored at: (" << ex << ", " << ey << ")\n"; // Debug output

			// Ensure the entry point is connected to a path
			for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
				int adjX = ex + dx;
				int adjY = ey + dy;
				if (adjX >= 0 && adjY >= 0 && adjX < width && adjY < height && map[adjY][adjX] == '#' && !isRoomWall(adjX, adjY)) {
					map[adjY][adjX] = '.'; // Carve the adjacent wall tile
					entryPoints.push_back({ adjX, adjY }); // Store the adjacent point
					std::cout << "Carving adjacent spot at: (" << adjX << ", " << adjY << ")\n"; // Debug output
				}
			}
		}

		// Carve the exit point
		if (exitIndex != -1) {
			auto [ex, ey] = perimeter[exitIndex];
			if (ex > 0 && ey > 0 && ex < width - 1 && ey < height - 1) {
				map[ey][ex] = '.'; // Carve the exit point
				exitPoints.push_back({ ex, ey }); // Store the exit point
			}
			std::cout << "Exit point stored at: (" << ex << ", " << ey << ")\n"; // Debug output

			// Ensure the exit point is connected to a path
			for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {0, -1}, {0, 1}, {-1, 0}, {1, 0} }) {
				int adjX = ex + dx;
				int adjY = ey + dy;
				if (adjX >= 0 && adjY >= 0 && adjX < width && adjY < height && map[adjY][adjX] == '#' && !isRoomWall(adjX, adjY)) {
					map[adjY][adjX] = '.'; // Carve the adjacent wall tile
					exitPoints.push_back({ adjX, adjY }); // Store the adjacent point
					std::cout << "Carving adjacent spot at: (" << adjX << ", " << adjY << ")\n"; // Debug output
				}
			}
		}
	}
	std::cout << "Total entry points stored: " << entryPoints.size() << std::endl; // Debug output
	std::cout << "Total exit points stored: " << exitPoints.size() << std::endl; // Debug output
}

void Map::carveExits() {
	// Define the condition to check (e.g., a specific key press or a game event)
	bool conditionMet = sf::Keyboard::isKeyPressed(sf::Keyboard::E);

	if (conditionMet) {
		std::cout << "Carving exits..." << std::endl;
		if (entryPoints.empty()) {
			std::cout << "No entry points to carve." << std::endl; // Debug output
		}
		else {
			for (const auto& entryPoint : entryPoints) {
				std::cout << "Carving at: (" << entryPoint.first << ", " << entryPoint.second << ")\n"; // Debug output
				map[entryPoint.second][entryPoint.first] = '.'; // Carve the wall at the entry point
			}
			for (const auto& exitPoint : exitPoints) {
				std::cout << "Carving at: (" << exitPoint.first << ", " << exitPoint.second << ")\n"; // Debug output
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
	revealArea(playerX, playerY, 5); // Reveal an area around the player

	sf::Font font; // Create a font object
	if (!font.loadFromFile("fs-min.ttf")) { // Load the font
		return; // If the font fails to load, return
	}

	if (playerInRoom(playerX, playerY)) {
		wallRooms(true); // Wall off rooms, blocking entries
		for (Room& room : rooms) {
			if (playerX >= room.startX && playerX < room.endX && playerY >= room.startY && playerY < room.endY) {
				if (!room.enemiesSpawned) {
					int roomWidth = room.endX - room.startX;
					int roomHeight = room.endY - room.startY;
					enemyManager->spawnEnemies(room.startX, room.startY, roomWidth, roomHeight, map);
					room.enemiesSpawned = true;
				}
				updateEnemies(playerX, playerY);
				break;
			}
		}
	}

	carveExits();

	sf::Text text; // Create a text object
	text.setFont(font); // Set the font for the text
	text.setCharacterSize(charSize); // Set the character size

	const int maxRadius = 5; // Maximum radius for the torch effect
	const int maxBrightness = 255; // Maximum brightness level
	const int minBrightness = 0; // Minimum brightness level for walls
	const float fadeOut = 0.5f; // Fade out factor for the torch effect

	for (int y = 0; y < height; ++y) { // For each row in the map
		for (int x = 0; x < width; ++x) { // For each column in the map
			if (revealed[y][x]) { // If the position is revealed
				int distance = std::abs(playerX - x) + std::abs(playerY - y); // Calculate Manhattan distance from the player

				if (distance <= maxRadius) { // If within the torch radius
					float brightness = maxBrightness * std::exp(-fadeOut * distance); // Calculate exponential brightness
					brightness = std::max(static_cast<float>(minBrightness), brightness); // Ensure brightness doesn't go below minimum

					text.setFillColor(sf::Color(static_cast<sf::Uint8>(brightness), 0, 0)); // Set color with the calculated brightness
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

	enemyManager->renderEnemies(window, charSize, playerX, playerY, player);
}

void Map::updateEnemies(int playerX, int playerY) {
	enemyManager->updateEnemies(map, playerX, playerY);
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
			if (x + dx >= 0 && y + dy >= 0 && x + dx < width && y + dy < height) { // If the position is within bounds
				revealed[y + dy][x + dx] = true; // Mark the position as revealed
			}
		}
	}
}

void Map::floodFill(int x, int y, std::vector<std::vector<bool>>& visited) {
	std::queue<std::pair<int, int>> queue;
	queue.push({ x, y });
	visited[y][x] = true;

	sf::RenderWindow window(sf::VideoMode(width * 10, height * 10), "Flood Fill Debug");

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
			floodVisualise(window, 10, visited);
		}
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