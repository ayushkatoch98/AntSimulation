#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Utility.h"
#include "WorldSettings.h"
#include "Food.h"
#include "Cell.h"


class Map {
private:
    std::vector<std::vector<Cell*>> map;
    sf::VertexArray vertices;
public:

    Map() {

        
        vertices = sf::VertexArray(
            sf::PrimitiveType::Triangles,
            WorldSettings::cellCountX * WorldSettings::cellCountY * WorldSettings::verticesPerCircle * 2
        );

        map.reserve(WorldSettings::cellCountY);
        for (int y = 0; y < WorldSettings::cellCountY; y++) {
            map.push_back(std::vector<Cell*>());
            map[y].reserve(WorldSettings::cellCountX);

            for (int x = 0; x < WorldSettings::cellCountX; x++) {
                int startIndex = (y * WorldSettings::cellCountX + x) * WorldSettings::verticesPerCircle * 2;
                map[y].push_back(new Cell(x, y, 0.f, 0.f, startIndex, vertices));
            }
        }
    }

    std::vector<std::vector<Cell*>>& getMap() {
        return map;
    }

    void render_and_update(sf::RenderWindow& window, float dt) {
        //auto start = std::chrono::high_resolution_clock::now();

        for (size_t y = 0; y < map.size(); y++) {
            for (size_t x = 0; x < map[y].size(); x++) {
                map[y][x]->update(dt);
            }
        }

        window.draw(vertices);

        //auto end = std::chrono::high_resolution_clock::now();

        //std::chrono::duration<double, std::milli> duration = end - start;
        //std::cout << "Function took " << duration.count() << " ms\n";

    }

    void setFood(Food* food, bool hasFood) {
        sf::Vector2i coords = Utility::getCellCoordsFromPosition(food->getPosition());
        if (hasFood == false) {
            map[coords.y][coords.x]->setCellHasFood(nullptr, hasFood);
            return;
        }

        map[coords.y][coords.x]->setCellHasFood(food, hasFood);
    }
};

