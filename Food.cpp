#include "Food.h"
#include "WorldSettings.h"

Food::Food(sf::Vector2f spawnPosition, float radius, float health) {
	this->spawnPosition = spawnPosition;
	this->radius = radius;
	this->obj = sf::CircleShape(radius);
	this->setPosition(spawnPosition);
	this->health = health;
}

void Food::setPosition(sf::Vector2f pos) {
	this->oldPosition = pos;
	this->obj.setPosition(pos);
	this->obj.setFillColor(WorldSettings::foodColor);
	this->obj.setOrigin({ radius, radius });
}

float Food::reduceHealth() {
	this->health -= 1.f;
	if (this->health < 0.f) this->health = 0.f;

	return this->health;
}

void Food::update(const float& dtTime) {
}

void Food::render(sf::RenderWindow& window) {

	//sf::CircleShape c(100.f);
	//c.setPosition(obj.getPosition());

	//obj.setOrigin({ obj.getSize().x / 2.f, obj.getSize().y / 2.f });
	//c.setOrigin({ c.getRadius(), c.getRadius()});

	//sf::Color color = sf::Color::Red;
	//color.a = 50;
	//c.setFillColor(color);

	//window.draw(c);
	//sf::Color color = this->obj.getFillColor();
	//color.a = this->health;
	//this->obj.setFillColor(color);
	if (this->health > 0.f)
		window.draw(this->obj);
}