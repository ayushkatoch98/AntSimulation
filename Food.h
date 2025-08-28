#pragma once
#include <SFML/Graphics.hpp>

class Food {
private:
	sf::Vector2f spawnPosition;
	sf::Vector2f oldPosition;
	sf::CircleShape obj;
	float radius;
	float health;

public:
	void update(const float& dtTime);
	void render(sf::RenderWindow& window);
	float reduceHealth();
	const sf::Vector2f& getPosition() { return obj.getPosition(); }
	Food(sf::Vector2f spawnPosition, float radius, float health);
	void setPosition(sf::Vector2f pos);
};

