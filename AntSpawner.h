#pragma once
#include <SFML/Graphics.hpp>
#include "Ant.h"
#include "WorldSettings.h"
#include <chrono>

class AntSpawner{

public:
	sf::Vector2f spawnPosition;
	std::vector<Ant*> ants;
	sf::Texture& antTex;
	int maxAnts;
	Map& map;
	sf::CircleShape obj;


	AntSpawner(sf::Vector2f spawnPosition, sf::Texture& antTex, int maxAnts, Map& map): antTex(antTex), map(map) {
		this->spawnPosition = spawnPosition;
		this->maxAnts = maxAnts;

		float radius = 50.f;


		obj = sf::CircleShape(radius);
		obj.setFillColor(WorldSettings::spawnerColor);
		obj.setPosition(spawnPosition);
		obj.setOrigin({ radius, radius });
	}

	void addNewAnt(const std::vector<Food*>& foods) {
		int randomIndex = Utility::getRandomValue(0, WorldSettings::allDirections.size() - 1);
		ants.push_back(new Ant(WorldSettings::antSpeed, WorldSettings::antVisionLength, spawnPosition,
			WorldSettings::antSize, WorldSettings::allDirections[0], map, foods, antTex)
		);
	}


	void spawn(int quantity, const std::vector<Food*>& foods) {
		for (int i = 0; i < quantity && ants.size() < maxAnts; i++) {
			addNewAnt(foods);
		}
	}


	void render_and_update(sf::RenderWindow& window, float dt) {

		auto start = std::chrono::high_resolution_clock::now();


		int size = ants.size();

		for (int i = 0; i < size; i++) {

			ants[i]->update(dt, window);
			ants[i]->render(window);

		}


		window.draw(obj);


		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = end - start;
		std::cout << "Ant Render took " << duration.count() << " ms\n";

	}

};

