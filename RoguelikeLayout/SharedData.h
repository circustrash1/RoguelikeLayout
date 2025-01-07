#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <string>

class Game;

extern Game game;

struct SharedData {
	std::string relevantStat;
};

#endif // SHAREDDATA_H