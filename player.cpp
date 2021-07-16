#include "player.hpp"

Player::Player(int team, sf::Vector2i position) : _team(team), _position(position), _turnComplete(false)
{
	_maStat = 6;
	_remainingMa = 6;
}