// main.cpp
#include "Game.h" // Include the Game class header
#include "SharedData.h"

SharedData sharedData;
Game game;

int main() {
	game.run(); // Run the game loop
	return 0; // Exit the program
}