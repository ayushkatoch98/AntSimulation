#pragma once
#include <SFML/Graphics.hpp>
#include "Food.h"
#include "Cell.h"
#include "Map.h"
#include <stack>


enum class Directions {
	UP = 10,
	DOWN = 10,
	LEFT = 10,
	RIGHT = 10,
	TOP_RIGHT = 10,
	TOP_LEFT = 10,
	BOTTOM_RIGHT = 10,
	BOTTOM_LEFT = 10
};

enum class AntState {
	RESTING,
	FINDING_FOOD,
	RETURNING_EMPTY,
	RETURNING_FOOD,
	GOING_TO_KNOWN_FOOD
};

class Ant{

private:
	float speed;
	float totalTime = 0.0f;
	float sightLength;
	float stepsSinceTrail = 0;
	bool foodFound = false;
	bool isHoldingFood = false;
	bool disabledRotation = false;
	int stuckFrames = 0;
	float sightLengthSquared;
	
	sf::Clock clock;
	Map& map;

	const std::vector<Food*>& allFoods;
	sf::Vector2f spawnPosition;
	sf::Vector2f size;
	sf::Vector2f lastMoveDir;
	sf::Vector2f lastMoveDir2;
	sf::Vector2f priorityDirection;
	sf::Vector2i foundFoodCoords;
	sf::Vector2f lastPosition;

	std::vector<std::pair<sf::Vector2f, int>> allDirectionBias;

	int totalFrames = 0;

	sf::Texture tex;
	sf::Sprite* obj;

	sf::CircleShape holdingFoodObj;


	AntState currentState = AntState::FINDING_FOOD;

public:
	Ant(const float speed, const float sightLength, sf::Vector2f spawnPosition, sf::Vector2f size, sf::Vector2f priorityDirection, Map& map, const std::vector<Food*>& allFoods, const sf::Texture& tex);
	const Cell* getStrongestCellInRadius(const sf::Vector2i& cellCoords, int radiusAsCellCount);
	const AntState& getCurrentState() { return this->currentState; }
	const std::pair<const Cell*, float> getNearestFood(const sf::Vector2i& cellCoords, int radiusAsCellCount);
	void handleTrailMarking(const sf::Vector2i& cellCords);
	void update(const float& dt, sf::RenderWindow& window);
	void setFoodFoodEmpty();
	void setFoodVariables(const std::pair<const Cell*, float>& closestFood);
	void handleFindingFoodState(sf::Vector2f& randomDirection);
	void followTrail(sf::Vector2f& randomDirection, const sf::Vector2i& cellCoords, sf::RenderWindow& window);
	void render(sf::RenderWindow& window);
	float handleRotation(const sf::Vector2f& randomDirection, float dt);
	std::pair<const Cell*, float> getSpawnInformation();
	sf::Vector2f getPosition() { return this->obj->getPosition(); }
};

