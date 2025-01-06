#ifndef MAP_H
#define MAP_H

#include "EnemyManager.h" // Include the EnemyManager class header
#include "Player.h"
#include "UpgradeManager.h"
#include "EventMaps.h"
#include "Boss.h"
#include <vector> // Include vector for storing the map layout
#include <stack> // Include stack for the DFS algorithm
#include <random> // Include random for std::default_random_engine
#include <SFML/Graphics.hpp> // Include SFML for rendering
#include <set>

class EnemyManager;
class Player; // Forward declaration of Player class
class Game; // Forward declaration

// Define the Room struct
struct Room {
	int startX, startY;
	int endX, endY;
	bool enemiesSpawned = false;
	bool upgradeSpawned = false;
	bool upgradeCollected = false;
	std::pair<int, int> upgradePosition;
	bool isMerchantRoom = false;
	bool merchantSpawned = false;
	std::pair<int, int> merchantPosition;
	bool isEventRoom = false;
	bool eventTriggered = false;
	bool eventResolved = false;
	std::string eventText;
	bool eventCharVisible = false;
	std::pair<int, int> eventCharPosition;
	sf::Clock eventTextClock;
	sf::Time eventTextDuration;
	std::vector<Upgrade> merchantUpgrades;  // Retain upgrades in shop for each room merchant
	bool wallOff = true;
	bool isBossRoom = false;
	bool bossRoomEntered = false;
};

class Map {
public:
	Map(int width, int height); // Constructor to initialize the map with given width and height
	~Map();
	void generate(); // Function to generate the map
	double calculatePathPercentage() const; // Function to calculate the percentage of paths in the map
	void debugRender(sf::RenderWindow& window, int charSize); // Function to render the map for debugging
	void floodVisualise(sf::RenderWindow& window, int charSize, const std::vector<std::vector<bool>>& visited);
	void render(sf::RenderWindow& window, int playerX, int playerY, int charSize, Player* player); // Function to render the map
	bool isWalkable(int x, int y) const; // Function to check if a position is walkable
	void reveal(int x, int y); // Function to reveal a specific position on the map
	void revealArea(int x, int y, int radius); // Function to reveal an area around a specific position
	bool isConnected(); // Function to check if all path tiles are connected
	void advanceToNextLevel(Player* player, Game* game); // Function to advance to the next level
	const std::vector<std::vector<char>>& getMap() const;
	const std::vector<Enemy*>& getEnemies() const;
	std::vector<Room>& getRooms();

	void updateEnemies(int playerX, int playerY, Player* player);

	void dropUpgrade(Room& room);
	UpgradeManager& upgradeManager();

	bool successfulGeneration = false; // Flag to indicate if the map was successfully generated

	EnemyManager* getEnemyManager();

	// Boss
	void setBossDefeated(bool defeated);
	bool isBossDefeated() const;
	void unblockExit();

private:
	bool playerAdvanced = false;

	int width; // Width of the map
	int height; // Height of the map
	int roomCount;  // Number of rooms generated
	const int maxRooms = 10;   // Maximum number of rooms to generate
	std::vector<std::vector<char>> map; // 2D vector to store the map layout
	std::vector<std::vector<bool>> revealed; // 2D vector to store which parts of the map are revealed
	std::vector<Room> rooms; // Vector to store all rooms
	std::vector<std::pair<int, int>> entryPoints; // Vector to store entry points
	std::vector<std::pair<int, int>> exitPoints; // Vector to store entry points

	EnemyManager* enemyManager; // Instance of the EnemyManager class
	UpgradeManager upgradeManager_;
	std::vector<Upgrade> roomUpgrades;

	void carve(int x, int y); // Function to carve paths in the map
	void carveRoom(int x, int y, std::default_random_engine& rng); // Function to carve rooms in the map
	void carveCorridor(int cx, int cy, std::default_random_engine& rng, std::stack<std::pair<int, int>>& stack); // Function to carve corridors in the map
	void floodFill(int x, int y, std::vector<std::vector<bool>>& visited);  // Function to perform flood fill
	void connectRegions();  // Function to connect disconnected regions
	void floodFillRegion(int x, int y, int region, std::vector<std::vector<int>>& regions); // Function to perform flood fill for regions
	void connectTwoRegions(int region1, int region2, const std::vector<std::vector<int>>& regionMap, sf::RenderWindow* window); // Function to connect two regions
	void carvePath(int x1, int y1, int x2, int y2, sf::RenderWindow* window); // Function to carve a path between two points
	bool isRoomWall(int x, int y) const; // Function to check if a tile is part of a room wall
	void wallRooms(bool blockEntries = false); // Function to wall off rooms, optionally blocking entries
	bool playerInRoom(int playerX, int playerY) const; // Function to check if the player is inside a room
	void chooseCarveExits(); // Function to choose and carve initial exits
	void carveExits(); // Function to carve exits on condition
	bool firstGeneration = true;

	// Boss rooms
	int generationCount = 0;
	void generateBossCorridor();
	void generateBossRoom();
	bool bossDefeated = false;

	bool debug = false; // Debug flag
};

#endif // MAP_H
