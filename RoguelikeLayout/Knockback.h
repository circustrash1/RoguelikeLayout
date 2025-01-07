#ifndef KNOCKBACK_H
#define KNOCKBACK_H

#include <vector>
#include <utility>

class Knockback {
public:
    static void applyKnockback(int& targetX, int& targetY, int attackerX, int attackerY, const std::vector<std::vector<char>>& map, int knockbackDistance, int roomStartX, int roomStartY, int roomEndX, int roomEndY, const std::vector<std::pair<int, int>>& bodyParts);
};

#endif // KNOCKBACK_H
