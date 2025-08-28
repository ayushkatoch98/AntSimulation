#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string.h>

class WorldSettings{

public:

	// Window Settings
	static const int windowX = 1900;
	static const int windowY = 900;
	static constexpr sf::Vector2u windowSize = sf::Vector2u((unsigned int)windowX, (unsigned int)windowY);

    static constexpr sf::Color bgColor = sf::Color(10, 10, 10);      // Almost black, very dark gray
    static constexpr sf::Color foodColor = sf::Color(170, 120, 30);    // Muted dark gold, still visible
    static constexpr sf::Color foodTrail = sf::Color(50, 90, 150, 128); // Deep blue trail, alpha dynamic
    static constexpr sf::Color homeTrail = sf::Color(40, 150, 70, 128); // Dark green trail, alpha dynamic
    static constexpr sf::Color spawnerColor = sf::Color(120, 40, 160);    // Dark purple, stands out



    // Cell
    static const int cellSize = 5;
    static const int cellCountX = (int)(windowX / cellSize);
    static const int cellCountY = (int)(windowY / cellSize);
    static const int circleSegment = 3;
    static const int verticesPerCircle = circleSegment * 3;


	// Ant Settings 
	static constexpr float antSpeed = 200.0f;
	static constexpr float antScaleX = 8.0;
	static constexpr float antScaleY = 8.0;
    static constexpr float antVisionLength = 100.f;
	static constexpr float changeDirectionIntervalMin = 1.f;
    static constexpr float changeDirectionIntervalMax = 2.f;
    static constexpr float minDistToInteractWithObjects = 10.f;
	static constexpr sf::Vector2f antSize = sf::Vector2f(antScaleX, antScaleY);

    inline static const std::vector<sf::Vector2f> allDirections = {
        { 1.f, 0.f },    // Right
        { 0.f, 1.f },    // Down
        { -1.f, 0.f },   // Left
        { 0.f, -1.f },   // Up

        { 1.f, 1.f },    // Down-Right
        { 1.f, -1.f },   // Up-Right
        { -1.f, 1.f },   // Down-Left
        { -1.f, -1.f }   // Up-Left
    };

	inline static const std::string antTexture = "redant.png";
	virtual void virtualMethod() = 0;

};

