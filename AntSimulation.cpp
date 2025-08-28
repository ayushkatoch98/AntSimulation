#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC

#include <SFML/Graphics.hpp>
#include "WorldSettings.h"
#include "Ant.h"
#include "Food.h"
#include "Utility.h"
#include "Cell.h"
#include "Map.h"
#include <cstdlib>
#include <crtdbg.h>
#include "AntSpawner.h"


#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    sf::RenderWindow window(sf::VideoMode(WorldSettings::windowSize), "SFML 3");
    window.setFramerateLimit(60);

    sf::Clock deltaClock;
    sf::Clock longClock;
    sf::Clock spawnClock;  // <- clock for spawning ants


    //sf::Vector2f priorityDirection = WorldSettings::allDirections[randomIndex];
    //sf::Vector2f priorityDirection = { 1.f, 0.f };

    sf::Texture tex;

    if (!tex.loadFromFile(WorldSettings::antTexture)) {  // make sure the file is in the working directory
        std::cerr << "Error: Could not load texture!" << std::endl;
        return 0;
    }

    sf::Texture tex2;

    if (!tex2.loadFromFile("ant.png")) {  // make sure the file is in the working directory
        std::cerr << "Error: Could not load texture!" << std::endl;
        return 0;
    }

    Map map;

    int SPAWN_INTERVAL = 1.f;
    int MAX_ANTS_PER_SPAWN = 500;
    int ANTS_SPAWN_COUNT_EVERY_N_SECONDS = 500;

 
        
    float edgeOffset = 30.f;

    std::vector<AntSpawner*> spawners = {
        new AntSpawner({ 50.f, WorldSettings::windowY / 2.f }, tex, MAX_ANTS_PER_SPAWN, map),
        //new AntSpawner({ WorldSettings::windowX - 50.f, 30.f }, tex, MAX_ANTS_PER_SPAWN, map)
    };
    std::vector<Food*> foods = {
        // Left Side
        new Food(Utility::getPositionFromCellCoords(edgeOffset * 3, edgeOffset), 10.f, 300.f),
        //new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountY - edgeOffset), 10.f),

        // Middle (top + bottom)
        new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountX / 2, edgeOffset), 10.f, FLT_MAX),
        new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountX / 2, WorldSettings::cellCountY - edgeOffset), 10.f, FLT_MAX),

        // Right side (top + middle + bottom)
        //new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountX - edgeOffset, edgeOffset), 10.f, FLT_MAX),
        new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountX - edgeOffset * 2, WorldSettings::cellCountY / 2), 10.f, 300.f),
        new Food(Utility::getPositionFromCellCoords(WorldSettings::cellCountX - edgeOffset * 2, WorldSettings::cellCountY - edgeOffset), 10.f, FLT_MAX)
    };
    

    for (int i = 0; i < foods.size(); i++) {
        map.setFood(foods[i], true);
    }
    
   
    while (window.isOpen()) {

        //if (longClock.getElapsedTime().asSeconds() >= 20.f) break;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto& mouse = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouse->button == sf::Mouse::Button::Left) {
                    // Mouse position in window coordinates
                    sf::Vector2i mouseClickPos = mouse->position;
                    std::cout << "Left Click at: " << mouseClickPos.x << ", " << mouseClickPos.y << "\n";
                    foods.push_back(
                        new Food({ (float)mouseClickPos.x, (float)mouseClickPos.y}, 10.f, 300.f)
                    );
                    map.setFood(foods[foods.size() - 1], true);
                }
            }

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Space) {
                    // Do something when space is pressed
                    //food.setPosition({ WorldSettings::windowX - 30.f, WorldSettings::windowY - 30.f });
                }
            }
        }

        float dt = deltaClock.restart().asSeconds();

        // --- SPAWN ANTS EVERY 5 SECONDS ---
        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_INTERVAL) {
            spawnClock.restart();
            for (int i = 0; i < spawners.size(); i++)
                spawners[i]->spawn(ANTS_SPAWN_COUNT_EVERY_N_SECONDS, foods);
        }

        window.clear(WorldSettings::bgColor);

        map.render_and_update(window, dt);
        for (int i = 0; i < foods.size(); i++) {
            foods[i]->render(window);
        }
        for (int i = 0; i < spawners.size(); i++)
            spawners[i]->render_and_update(window, dt);



        window.display();


    }

    _CrtDumpMemoryLeaks();

    return 0;
}
