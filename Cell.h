#pragma once
#include "Utility.h"
#include "WorldSettings.h"
#include "Food.h"

enum class IncreaseDecrease {
	INCREASE,
	DECREASE
};


class Cell {
private:

	float homeTrailStrength;
	float foodTrailStrength;
	const float changeValue = 40.f;
	sf::RectangleShape objHomeTrail;
	sf::RectangleShape objFoodTrail;
	Food* food = nullptr;
	bool isFoodPresent = false;
	sf::VertexArray& vertices;
	sf::Color foodTrailColor = WorldSettings::foodTrail;
	sf::Color homeTrailColor = WorldSettings::homeTrail;
	int x;
	int y;
	sf::Vector2f worldPosition;
	int vertexStartIndex;

public:


	Cell(int x, int y, float homeTrailStrength, float foodTrailStrength, int vertexStartIndex, sf::VertexArray& vertices) : vertices(vertices) {
		this->homeTrailStrength = homeTrailStrength;
		this->foodTrailStrength = foodTrailStrength;
		this->x = x;
		this->y = y;
		this->objHomeTrail = sf::RectangleShape({ (float)WorldSettings::cellSize, (float)WorldSettings::cellSize });
		this->objFoodTrail = sf::RectangleShape({ (float)WorldSettings::cellSize, (float)WorldSettings::cellSize });

		this->objHomeTrail.setPosition(Utility::getPositionFromCellCoords(x, y));
		this->objFoodTrail.setPosition(Utility::getPositionFromCellCoords(x, y));

		this->vertexStartIndex = vertexStartIndex;

		this->worldPosition = sf::Vector2f({ (float)x, (float)y }) * (float) WorldSettings::cellSize;

		addToVertexArray();
	}

	sf::Vector2f getWorldPosition() const {
		sf::Vector2f v(x, y);
		return v * (float) WorldSettings::cellSize;
	}


	sf::Vector2i getCellCoords() const {
		sf::Vector2i v(x, y);
		return v;
	}

	void addToVertexArray() {
		int cellSize = WorldSettings::cellSize;
		float radius = cellSize / 2.f;
		sf::Vector2f center((x * cellSize) + radius, (y * cellSize) + radius);

		int vertsPerCircle = WorldSettings::verticesPerCircle;

		// First circle (food trail)
		for (int i = 0; i < WorldSettings::circleSegment; i++) {
			float angle1 = i * 2.f * 3.14159265f / WorldSettings::circleSegment;
			float angle2 = (i + 1) * 2.f * 3.14159265f / WorldSettings::circleSegment;

			sf::Vector2f p1(center.x + radius * std::cos(angle1),
				center.y + radius * std::sin(angle1));
			sf::Vector2f p2(center.x + radius * std::cos(angle2),
				center.y + radius * std::sin(angle2));

			int base = vertexStartIndex + i * 3;
			vertices[base] = sf::Vertex(center, sf::Color::Transparent);
			vertices[base + 1] = sf::Vertex(p1, sf::Color::Transparent);
			vertices[base + 2] = sf::Vertex(p2, sf::Color::Transparent);
		}

		// Second circle (home trail)
		int offset = vertsPerCircle;
		for (int i = 0; i < WorldSettings::circleSegment; i++) {
			float angle1 = i * 2.f * 3.14159265f / WorldSettings::circleSegment;
			float angle2 = (i + 1) * 2.f * 3.14159265f / WorldSettings::circleSegment;

			sf::Vector2f p1(center.x + radius * std::cos(angle1),
				center.y + radius * std::sin(angle1));
			sf::Vector2f p2(center.x + radius * std::cos(angle2),
				center.y + radius * std::sin(angle2));

			int base = vertexStartIndex + offset + i * 3;
			vertices[base] = sf::Vertex(center, sf::Color::Transparent);
			vertices[base + 1] = sf::Vertex(p1, sf::Color::Transparent);
			vertices[base + 2] = sf::Vertex(p2, sf::Color::Transparent);
		}
	}

	void setCellHasFood(Food* food, bool hasFood) {
		this->isFoodPresent = hasFood;
		this->food = food;
	}

	Food* getFood() const {
		return this->food;
	}

	float getFoodTrailStrength() const { return foodTrailStrength; }
	float getHomeTrailStrength() const { return homeTrailStrength; }

	bool getCellHasFood() const { return this->isFoodPresent; }

	int getX() { return x; }
	int getY() { return y; }

	void changeHomeTrailStrengthBy(IncreaseDecrease doWhat) {
		changeTrailStrengthBy(homeTrailStrength, doWhat);
	}

	void changeFoodTrailStrengthBy(IncreaseDecrease doWhat) {
		changeTrailStrengthBy(foodTrailStrength, doWhat);
	}

	void changeTrailStrengthBy(float& trailStrength, IncreaseDecrease doWhat) {

		if (doWhat == IncreaseDecrease::INCREASE) {
			trailStrength += changeValue;
		}
		else if (doWhat == IncreaseDecrease::DECREASE) {
			trailStrength -= changeValue;
		}

		if (trailStrength > 100.f) trailStrength = 100.f;
		else if (trailStrength < 0.f) trailStrength = 0.f;
	}


	void update(float dt) {



		foodTrailStrength -= (changeValue * dt * 0.1);
		homeTrailStrength -= (changeValue * dt * 0.1);

		if (foodTrailStrength < 0.f) foodTrailStrength = 0.f;
		if (homeTrailStrength < 0.f) homeTrailStrength = 0.f;

		if (homeTrailStrength <= 0.f && foodTrailStrength <= 0.f) return;

		foodTrailColor.a = (uint8_t)foodTrailStrength;
		homeTrailColor.a = (uint8_t)homeTrailStrength;


		// Food circle
		for (int i = 0; i < WorldSettings::verticesPerCircle; i++) {
			vertices[vertexStartIndex + i].color = homeTrailColor;
			vertices[vertexStartIndex + WorldSettings::verticesPerCircle + i].color = foodTrailColor;
		}

	}

};

