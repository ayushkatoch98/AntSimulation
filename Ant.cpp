#include "Ant.h"
#include "Utility.h"
#include <iostream>
#include "WorldSettings.h"
#include <cmath>  
#include "Cell.h"


inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}


inline float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float L = length(v);
    return (L > 1e-6f) ? sf::Vector2f(v.x / L, v.y / L) : sf::Vector2f(0.f, 0.f);
}

inline float normalizeAngle(float angle) {
    while (angle < 0.f) angle += 360.f;
    while (angle >= 360.f) angle -= 360.f;
    return angle;
}

inline sf::Vector2f getUnitVectorFromAngle(float angleDeg) {
    float rad = angleDeg * 3.14159265f / 180.f;
    return sf::Vector2f(std::cos(rad), std::sin(rad));
}


inline float sinWave(float longClock, float frequency, float amplitude) {
    return std::sin(longClock * frequency) * amplitude;
}


inline float calcDistanceWithoutSquareRoot(const sf::Vector2f& from, const sf::Vector2f& to) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    return dx * dx + dy * dy;
}


inline float calcDistanceSquareRoot(const sf::Vector2f& from, const sf::Vector2f& to) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    return std::sqrt(dx * dx + dy * dy);
}

inline sf::Vector2f getNormalizedDirection(const sf::Vector2f& from, const sf::Vector2f& to) {
    sf::Vector2f diff = to - from;

    float length = calcDistanceSquareRoot(from, to);

    if (length == 0.f) // same point, no direction
        return { 0.f, 0.f };

    return diff / length;  // normalized vector
}


const char* antStateToString(AntState state) {
    switch (state) {
    case AntState::FINDING_FOOD: return "FINDING_FOOD";
    case AntState::RETURNING_FOOD: return "RETURNING_FOOD";
    case AntState::RESTING: return "RESTING";
    case AntState::RETURNING_EMPTY: return "RETURNING_EMPTY";
    case AntState::GOING_TO_KNOWN_FOOD: return "GOING_TO_KNOWN_FOOD";
    default: return "UNKNOWN";
    }
}


sf::Vector2f reflectDirection(const sf::Vector2f& dir, const sf::Vector2f& wallNormal) {
    // Make sure normal is normalized
    sf::Vector2f n = wallNormal;
    float len = length(n);
    n /= len;

    // Reflection formula
    float dotProduct = dot(dir, n);
    sf::Vector2f reflected = dir - 2.f * dotProduct * n;

    float rlen = length(reflected);
    if (rlen != 0) reflected /= rlen;

    return reflected;
}


Ant::Ant(const float speed, const float sightLength, sf::Vector2f spawnPosition, sf::Vector2f size, sf::Vector2f priorityDirection, Map& map, const std::vector<Food*>& allFoods, const sf::Texture& tex) : tex(tex), map(map), allFoods(allFoods){
    this->speed = speed;
    this->spawnPosition = spawnPosition;
    this->size = size;
    this->priorityDirection = priorityDirection;
    this->sightLength = sightLength;
    this->clock.start();
    this->lastMoveDir = { 0.0f, 0.0f };
    this->foundFoodCoords = { WorldSettings::windowY, WorldSettings::windowX };

    obj = new sf::Sprite(tex);
    obj->setPosition(spawnPosition);
    obj->setTexture(tex);
    obj->setScale({ 0.04f, 0.04f });

    sf::FloatRect bounds = obj->getLocalBounds();
    obj->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });

    currentState = AntState::FINDING_FOOD;

    sightLengthSquared = sightLength * sightLength;

    holdingFoodObj = Utility::createCircle(3.f, obj->getPosition(), WorldSettings::foodColor);

}


const Cell* Ant::getStrongestCellInRadius(const sf::Vector2i& cellCoords, int radiusAsCellCount) {
    const Cell* bestCell = nullptr;
    float highestStrength = -FLT_MAX;
    sf::Vector2f pointToCandidateCell;
    const Cell* oppDir = nullptr;

    float dotProduct = 0.f;

    sf::Vector2f pointToFood = Utility::getPositionFromCellCoords(foundFoodCoords) - obj->getPosition();
    sf::Vector2f pointToHome = spawnPosition - obj->getPosition();

    std::vector<std::vector<Cell*>>& grid = map.getMap();

    float radiusSquared = radiusAsCellCount * radiusAsCellCount;

    // TODO: Use Cone instead of circle to reduce total iterations and speed the the updates
    for (int dx = -radiusAsCellCount; dx <= radiusAsCellCount; ++dx) {
        
        int indexX = cellCoords.x + dx;
        if (indexX < 0 || indexX >= (int)grid[0].size()) continue;

        for (int dy = -radiusAsCellCount; dy <= radiusAsCellCount; ++dy) {
            
            int indexY = cellCoords.y + dy;
            if (indexY < 0 || indexY >= (int)grid.size()) continue;

            if (dx * dx + dy * dy > radiusSquared) continue; 
                
            const Cell* candidate = grid[indexY][indexX];

            if (cellCoords == candidate->getCellCoords()) continue;

            if (currentState == AntState::RETURNING_FOOD) {

                pointToCandidateCell = candidate->getWorldPosition() - obj->getPosition();

                dotProduct = dot(pointToHome, pointToCandidateCell);
                if (dotProduct <= 0.0f) {
                    oppDir = candidate;
                    continue;
                }

                if (candidate->getHomeTrailStrength() > highestStrength) {
                    highestStrength = candidate->getHomeTrailStrength();
                    bestCell = candidate;
                }
            }
            else if (currentState == AntState::GOING_TO_KNOWN_FOOD) {

                pointToCandidateCell = candidate->getWorldPosition() - obj->getPosition();

                dotProduct = dot(pointToFood, pointToCandidateCell);
                if (dotProduct <= 0.0f) {
                    oppDir = candidate;
                    continue;
                } 

                if (candidate->getFoodTrailStrength() > highestStrength) {
                    highestStrength = candidate->getFoodTrailStrength();
                    bestCell = candidate;
                }
            }
        }
    }

    // Usually do this for these states -> currentState == AntState::GOING_TO_KNOWN_FOOD || currentState == AntState::RETURNING_FOOD
    if (bestCell != nullptr) {
        if ((currentState == AntState::GOING_TO_KNOWN_FOOD && bestCell->getFoodTrailStrength() == 0.f)) {
            stepsSinceTrail++;
            /*std::cout << "Ant State " << antStateToString(currentState) << " Lost For " << stepsSinceTrail << " STRENGTH " << bestCell->getFoodTrailStrength() << " isHoldingFood " << isHoldingFood << " has food found " << foodFound << " dist from food " << calcDistanceWithoutSquareRoot(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords)) << std::endl;*/
        }
        else {
            stepsSinceTrail = 0;
        }
        return bestCell;
    }
    if (oppDir != nullptr) return oppDir;

    return nullptr;
}



const std::pair<const Cell*, float> Ant::getNearestFood(const sf::Vector2i& cellCoords, int radiusAsCellCount)
{
    const Cell* bestCell = nullptr;
    float closestFoodDist = FLT_MAX;

    std::vector<std::vector<Cell*>>& grid = map.getMap();
    const int maxY = (int)grid.size();
    const int maxX = (int)grid[0].size();
    const int radiusSquared = radiusAsCellCount * radiusAsCellCount;

    for (int dx = -radiusAsCellCount; dx <= radiusAsCellCount; ++dx) {
        int indexX = cellCoords.x + dx;
        if (indexX < 0 || indexX >= maxX) continue;

        for (int dy = -radiusAsCellCount; dy <= radiusAsCellCount; ++dy) {
            int indexY = cellCoords.y + dy;
            if (indexY < 0 || indexY >= maxY) continue;

            int distSq = dx * dx + dy * dy;
            if (distSq > radiusSquared) continue; // outside circle

            const Cell* candidate = grid[indexY][indexX];
            if (candidate->getCellHasFood() && distSq < closestFoodDist) {
                closestFoodDist = (float)distSq;
                bestCell = candidate;
                // to optimize this function..... just returning the first 'closes food' 
                return { bestCell, closestFoodDist };
            }
        }
    }

    return { bestCell, closestFoodDist };
}




void Ant::handleTrailMarking(const sf::Vector2i& cellCoords) {
    std::vector<std::vector<Cell*>>& grid = map.getMap();

    if (currentState == AntState::FINDING_FOOD || currentState == AntState::GOING_TO_KNOWN_FOOD) {
        // marking home trail
        if (cellCoords.y >= 0 && cellCoords.y < grid.size() && cellCoords.x >= 0 && cellCoords.x < grid[0].size()) {
            grid[cellCoords.y][cellCoords.x]->changeHomeTrailStrengthBy(IncreaseDecrease::INCREASE);
        }
    }

    else if (currentState == AntState::RETURNING_FOOD) {
        // marking food trail
        if (cellCoords.y >= 0 && cellCoords.y < grid.size() && cellCoords.x >= 0 && cellCoords.x < grid[0].size()) {
            grid[cellCoords.y][cellCoords.x]->changeFoodTrailStrengthBy(IncreaseDecrease::INCREASE);
        }
    }
}

std::pair<const Cell*, float> Ant::getSpawnInformation() {

    float dist = calcDistanceWithoutSquareRoot(obj->getPosition(), spawnPosition);
    sf::Vector2i cellCoords = Utility::getCellCoordsFromPosition(spawnPosition);

    return { map.getMap()[cellCoords.y][cellCoords.x], dist };

}

void Ant::update(const float& dt, sf::RenderWindow& window) {

    if (currentState == AntState::RESTING) return;

    totalFrames++;
    totalTime += dt;

    const sf::Vector2i cellCoords = Utility::getCellCoordsFromPosition(obj->getPosition());
    std::pair<const Cell*, float> closestFood = { nullptr, NULL };
    const std::pair<const Cell*, float> spawnInfo = getSpawnInformation();

    sf::Vector2f randomDirection = sf::Vector2f(0.f, 0.f);
    
    //std::cout << "Ant State " << antStateToString(currentState) << " isHoldingFood " << isHoldingFood << " has food found " << foodFound << " dist from food " << calcDistanceWithoutSquareRoot(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords)) << std::endl;


    // --- Deciding how to travel Random / Trail ---
    // Go directly towards the food when within ant sight
    if (currentState == AntState::GOING_TO_KNOWN_FOOD && calcDistanceWithoutSquareRoot(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords)) <= sightLengthSquared) {
        randomDirection = getNormalizedDirection(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords));
    }
    // Go directly towards the spawn when within ant sight
    else if (currentState == AntState::RETURNING_FOOD && spawnInfo.second <= sightLengthSquared) {
        randomDirection = getNormalizedDirection(obj->getPosition(), spawnPosition);
    }
    // When food location is known or going back home and none of them are within the ant sight ? follow trail
    else if (currentState == AntState::GOING_TO_KNOWN_FOOD || currentState == AntState::RETURNING_FOOD) {
        if (totalFrames >= 5) {
            followTrail(randomDirection, cellCoords, window);
            lastMoveDir2 = randomDirection;
            totalFrames = 0;
        }
        else {
            randomDirection = lastMoveDir2;
        }
       
        if (randomDirection == sf::Vector2f(0.f, 0.f)) {
            std::cout << "TRAIL LOST\n";
            std::cout << "Ant State " << antStateToString(currentState) << " isHoldingFood " << isHoldingFood << " has food found " << foodFound << " dist from food " << calcDistanceWithoutSquareRoot(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords)) << std::endl;
        }
    }

    // move randomly when food location is not known
    else if (currentState == AntState::FINDING_FOOD) {
        closestFood = getNearestFood(cellCoords, sightLength / WorldSettings::cellSize);
        if (closestFood.first != nullptr && closestFood.second <= sightLengthSquared) {
            randomDirection = getNormalizedDirection(obj->getPosition(), closestFood.first->getWorldPosition());
        }
        handleFindingFoodState(randomDirection);
    }

    // --- Changing Ant State START ---
    // when looking for unknown food
    if (currentState == AntState::FINDING_FOOD) {
        if (closestFood.first != nullptr && closestFood.second <= WorldSettings::minDistToInteractWithObjects * WorldSettings::minDistToInteractWithObjects)
            setFoodVariables(closestFood);
    }

    // When looking for known food
    if (currentState == AntState::GOING_TO_KNOWN_FOOD) {

        float dist = calcDistanceWithoutSquareRoot(obj->getPosition(), Utility::getPositionFromCellCoords(foundFoodCoords));

        if (dist <= WorldSettings::minDistToInteractWithObjects * WorldSettings::minDistToInteractWithObjects) {
            const Cell* cell = map.getMap()[foundFoodCoords.y][foundFoodCoords.x];
            setFoodVariables({ cell, dist });
            if (foundFoodCoords != sf::Vector2i(-1, -1)) stepsSinceTrail = 0;
        }

    }

    // if ant is stuck ? unstuck the ant by overriding its state and setting food variables to false
    if (stepsSinceTrail >= 20.f and (currentState == AntState::RETURNING_FOOD or currentState == AntState::GOING_TO_KNOWN_FOOD)) {
        std::cout << "RESETTING STATE\n";
        setFoodFoodEmpty();
        foundFoodCoords = { -1, -1 };
        stepsSinceTrail = 0;

    }

    // when ants are returning to 'spawn'
    if (currentState == AntState::RETURNING_FOOD) {
        // if dist between ant and spawn is <= minDistToInteractWithObjects change the ant state to going to find known food 
        if (calcDistanceWithoutSquareRoot(spawnPosition, obj->getPosition()) <= WorldSettings::minDistToInteractWithObjects * WorldSettings::minDistToInteractWithObjects) {
            if (foundFoodCoords == sf::Vector2i(-1, -1)) {
                currentState = AntState::FINDING_FOOD;
                foundFoodCoords = sf::Vector2i(0, 0);
                foodFound = false;
            }
            else currentState = AntState::GOING_TO_KNOWN_FOOD;
            isHoldingFood = false;
        }
    }
    // --- Changing Ant State END ---

    // --- Marking Trail ---
    handleTrailMarking(cellCoords);

    // --- Sin Wave Movement ---
    float offset = sinWave(clock.getElapsedTime().asSeconds(), 2.0f, 0.2f);
    sf::Vector2f perp(-randomDirection.y, randomDirection.x);
    sf::Vector2f waveOffset = perp * offset;

    randomDirection += waveOffset;

    // --- Rotatation ---
    if (randomDirection != sf::Vector2f(0.f, 0.f)) {
        float newRotation = handleRotation(randomDirection, dt);

        // Move in Ant Facing Direction
        sf::Vector2f facingDir = getUnitVectorFromAngle(newRotation);
        sf::Vector2f nextPos = obj->getPosition() + (facingDir * speed * dt);

        float offset = 30.f;
        if (nextPos.x <= -offset) {
            facingDir = reflectDirection(facingDir, sf::Vector2f({ 1.f, 0.f }));
            priorityDirection = facingDir;
            lastMoveDir = facingDir;
        }
        else if (nextPos.x >= WorldSettings::windowX + offset) {
            facingDir = reflectDirection(facingDir, sf::Vector2f({ -1.f, 0.f }));
            priorityDirection = facingDir;
            lastMoveDir = facingDir;
        }
        else if (nextPos.y <= -offset) {
            facingDir = reflectDirection(facingDir, sf::Vector2f({ 0.f, 1.f }));
            priorityDirection = facingDir;
            lastMoveDir = facingDir;
        }
        else if (nextPos.y >= WorldSettings::windowY + offset) {
            facingDir = reflectDirection(facingDir, sf::Vector2f({ 0.f, -1.f }));
            priorityDirection = facingDir;
            lastMoveDir = facingDir;
        }
        else {
            obj->setRotation(sf::degrees(newRotation));
        }

        if (isHoldingFood)
            holdingFoodObj.setPosition(obj->getPosition() + (facingDir * 15.f));

        obj->move(facingDir * speed * dt);

    }


}

void Ant::setFoodFoodEmpty() {
    foundFoodCoords = { 0, 0 };
    foodFound = false;
    isHoldingFood = false;
    currentState = AntState::FINDING_FOOD;
}

void Ant::setFoodVariables(const std::pair<const Cell*, float>& closestFood) {

    if (closestFood.first->getCellHasFood()) {
        foundFoodCoords = closestFood.first->getCellCoords();
        foodFound = true;
        isHoldingFood = true;
        currentState = AntState::RETURNING_FOOD;
        Food* food = closestFood.first->getFood();
        if (food != nullptr) {
            if (food->reduceHealth() <= 0.f) {
                map.setFood(food, false);
            }
        }
    }
    else {
        setFoodFoodEmpty();
    }

}
void Ant::handleFindingFoodState(sf::Vector2f& randomDirection) {


    // changing ant direction after every 'changeDirectionInterval' seconds
    if (totalTime >= Utility::getRandomValueFloat(WorldSettings::changeDirectionIntervalMin, WorldSettings::changeDirectionIntervalMax)) {
        randomDirection = priorityDirection;
        randomDirection.x += Utility::getRandomValueFloat(-1.0f, 1.0f);
        randomDirection.y += Utility::getRandomValueFloat(-1.0f, 1.0f);

        randomDirection = randomDirection.normalized();

        /*if (dot(randomDirection, lastMoveDir) < -0.8f) {
            randomDirection = lastMoveDir;
        }*/
        lastMoveDir = randomDirection;
        totalTime = 0.0f;
    }
    else randomDirection = lastMoveDir;


}

void Ant::followTrail(sf::Vector2f& randomDirection, const sf::Vector2i& cellCoords, sf::RenderWindow& window) {
    const Cell* cell = getStrongestCellInRadius(cellCoords, sightLength / WorldSettings::cellSize);

    if (cell != nullptr) {
        randomDirection = getNormalizedDirection(obj->getPosition(), cell->getWorldPosition());
    }
}


float Ant::handleRotation(const sf::Vector2f& targetDirection, float dt) {
    float targetAngle = std::atan2(targetDirection.y, targetDirection.x) * 180.f / 3.14159265f;
    float currentAngle = obj->getRotation().asDegrees();

    float delta = targetAngle - currentAngle;

    // Normalize delta to [-180, 180]
    if (delta > 180.f) delta -= 360.f;
    if (delta < -180.f) delta += 360.f;

    float maxRotation = 600.f * dt; // degrees per frame
    if (std::abs(delta) <= maxRotation) currentAngle = targetAngle;
    else currentAngle += (delta > 0.f ? maxRotation : -maxRotation);

    return normalizeAngle(currentAngle);
}

void Ant::render(sf::RenderWindow& window) {
    window.draw(*obj);
    
    if (isHoldingFood)
        window.draw(holdingFoodObj);

}
