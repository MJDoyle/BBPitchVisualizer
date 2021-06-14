//Simple class to hold player info

#pragma once

#include "config.hpp"

class Player
{
public:

	//Constructor
	Player(int team, sf::Vector2i position) : _team(team), _position(position)
	{}

	//Getters
	int GetTeam() { return _team; }
	sf::Vector2i GetPosition() { return _position; }

	//Setters
	void SetTeam(int team) { _team = team; }
	void SetPosition(sf::Vector2i position) { _position = position; }

private:

	//Player team
	int _team;

	//Player position (in tile coordinates)
	sf::Vector2i _position;
};