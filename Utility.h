#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>
#include <random>
#include <iostream>
#include <chrono>
#include "WorldSettings.h"

class Utility{
public:

	static sf::Vector2i getCellCoordsFromPosition(const sf::Vector2f& pos) {
		int x = (int)floor(pos.x / WorldSettings::cellSize);
		int y = (int)floor(pos.y / WorldSettings::cellSize);
		return sf::Vector2i(x, y);
	}

	static sf::Vector2f getPositionFromCellCoords(const sf::Vector2i& cellPos) {
		float x = (float)(cellPos.x * WorldSettings::cellSize);
		float y = (float)(cellPos.y * WorldSettings::cellSize);
		return sf::Vector2f(x, y);
	}

	static sf::Vector2f getPositionFromCellCoords(const int& x, const int& y) {
		float xx = (float)(x * WorldSettings::cellSize);
		float yy = (float)(y * WorldSettings::cellSize);
		return sf::Vector2f(xx, yy);
	}

	static sf::RectangleShape createRectangle(sf::Vector2f size, sf::Vector2f position, sf::Color color = sf::Color::Transparent) {
		sf::RectangleShape r(size);
		r.setPosition(position);
		r.setFillColor(color);
		return r;
	}

	static sf::CircleShape createCircle(float radius, sf::Vector2f position, sf::Color color = sf::Color::Transparent) {
		sf::CircleShape r(radius);
		r.setPosition(position);
		r.setFillColor(color);
		r.setOrigin({ radius, radius });
		return r;
	}

	static int getRandomValue(int min, int max) {
		static std::mt19937 generator(std::random_device{}());
		if (min >= max) {
			std::cout << "MIN AND MAX" << min << " || " << max << std::endl;
		}
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(generator);
	}

	static float getRandomValueFloat(float min, float max) {


		static std::mt19937 generator(std::random_device{}());

		std::uniform_real_distribution<float> distribution(min, max);

		return distribution(generator);
	
	}


	static sf::Vector2f vectorIntToFlow(const sf::Vector2i& vec){
		return sf::Vector2f((float)vec.x, (float)vec.y);
	}

	static sf::Vector2i vectorFloatToInt(const sf::Vector2f& vec){
		return sf::Vector2i((int)floor(vec.x), (int)floor(vec.y));
	}

	virtual void virtualMethod() = 0;

};

