// main.cpp
#include "Game.h" // Include the Game class header
#include "SharedData.h"

SharedData sharedData;

int main() {
	Game game; // Create an instance of the Game class
	game.run(); // Run the game loop
	return 0; // Exit the program
}