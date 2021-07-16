#include "config.hpp"

sf::Vector2i SCREEN_SIZE = sf::Vector2i(1920, 1080);

float SCROLL_SPEED = 10;

const float DT = 10;

const float TILE_SIZE = 25;



//Pitch positions

sf::IntRect SUBS_ONE = sf::IntRect(-4, 2, 2, 8);

sf::IntRect SUBS_TWO = sf::IntRect(17, 16, 2, 8);

sf::IntRect PITCH = sf::IntRect(0, 0, 15, 26);



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