#include "Knockback.h"
#include "Boss.h"

void Knockback::applyKnockback(int& targetX, int& targetY, int attackerX, int attackerY, const std::vector<std::vector<char>>& map, int knockbackDistance, int roomStartX, int roomStartY, int roomEndX, int roomEndY, const std::vector<std::pair<int, int>>& bodyParts) {
    int deltaX = targetX - attackerX;
    int deltaY = targetY - attackerY;

    if (deltaX != 0) {
        deltaX = (deltaX > 0) ? 1 : -1;
    }
    if (deltaY != 0) {
        deltaY = (deltaY > 0) ? 1 : -1;
    }

    int newX = targetX;
    int newY = targetY;

    for (int i = 0; i < knockbackDistance; ++i) {
        int nextX = newX + deltaX;
        int nextY = newY + deltaY;

        bool canMove = true;
        for (const auto& part : bodyParts) {
            int partNextX = part.first + (nextX - targetX);
            int partNextY = part.second + (nextY - targetY);

            if (partNextX < roomStartX || partNextX > roomEndX || partNextY < roomStartY || partNextY > roomEndY || map[partNextY][partNextX] == '#') {
                canMove = false;
                break;
            }
        }

        if (canMove) {
            newX = nextX;
            newY = nextY;
        }
        else {
            break; // Stop if any part of the boss would be out of bounds or collide with a wall
        }
    }

    targetX = newX;
    targetY = newY;
}
