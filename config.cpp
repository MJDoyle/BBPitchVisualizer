#include "config.hpp"

sf::Vector2i SCREEN_SIZE = sf::Vector2i(1920, 1080);

float SCROLL_SPEED = 10;

const float DT = 10;

const float TILE_SIZE = 25;




sf::Vector2f operator*(double scalar, sf::Vector2f vector)
{
	return sf::Vector2f(vector.x * scalar, vector.y * scalar);
}

sf::Vector2f operator*(sf::Vector2f vector, double scalar)
{
	return sf::Vector2f(vector.x * scalar, vector.y * scalar);
}

sf::Vector2f operator/(sf::Vector2f vector, double scalar)
{
	return sf::Vector2f(vector.x / scalar, vector.y / scalar);
}