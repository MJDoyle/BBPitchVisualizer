#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <list>
#include <regex>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include <Windows.h>

extern sf::Vector2i SCREEN_SIZE;

extern float SCROLL_SPEED;

extern const float DT;

extern const float TILE_SIZE;

const enum TEAMS {TEAM_ONE, TEAM_TWO, NUM_TEAMS};

//Pitch positions

extern sf::IntRect SUBS_ONE;

extern sf::IntRect SUBS_TWO;

extern sf::IntRect PITCH;



//Some overlads for vector operations

sf::Vector2f operator*(double scalar, sf::Vector2f vector);

sf::Vector2f operator*(sf::Vector2f vector, double scalar);

sf::Vector2f operator/(sf::Vector2f vector, double scalar);



//Compare struct for using Vector2i as a map key

struct Vector2iCompare
{
	bool operator() (const sf::Vector2i& lhs, const sf::Vector2i& rhs) const
	{
		if (lhs.x < rhs.x)
			return true;

		if (lhs.x > rhs.x)
			return false;

		if (lhs.y < rhs.y)
			return true;

		if (lhs.y > rhs.y)
			return false;

		return false;
	}
};