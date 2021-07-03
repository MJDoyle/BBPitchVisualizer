//Everything happens here

#pragma once

#include "config.hpp"

class Player;



class Match
{
public:

	//Constructor
	Match(std::shared_ptr<sf::RenderWindow> window);

	//Setup the players of both teams on the bench
	void AddDefaultStartingPlayers();

	//Run main loop
	void Run();

	//Handle user input
	void HandleEvents();

	//Draw current state
	void Draw();

	//Update
	void Update();


private:

	//VISUALS

	//Game window
	std::shared_ptr<sf::RenderWindow> _window;

	//Font
	std::shared_ptr<sf::Font> _font;

	//Pitch texture
	std::shared_ptr<sf::Texture> _pitchTexture;

	//Pitch sprite
	std::shared_ptr<sf::Sprite> _pitchSprite;

	//Current zoom level
	float _zoom;

	//Curret (signed) zoom speed
	float _zoomSpeed;

	//Is the main view being dragged
	bool _dragging;

	//Start point of the current view drag
	sf::Vector2i _dragStart;

	//Load texture and set up sprites
	void LoadTexturesAndCreateSprites();

	//Load font
	void LoadFont();


	//COMPONENTS

	//Are players being deployed
	bool _deploymentPhase;

	//Map (by tile position) of all players
	std::map<sf::Vector2i, std::shared_ptr<Player>, Vector2iCompare> _players;

	//Player currently selected by user
	std::shared_ptr<Player> _selectedPlayer;

	//Move a player in deployment phase
	void DeployPlayer(int team, sf::Vector2i playerOldPosition, sf::Vector2i playerNewPosition);

	//UTILITY

	//Main update tiemr
	sf::Clock _updateClock;

	//Get the pixel coordinates on the window from the tile position
	sf::Vector2f ScreenPositionFromTilePosition(sf::Vector2i tilePosition);

	//Get the tile coordinates from the pixel position on the window
	sf::Vector2i TilePositionFromScreenPosition(sf::Vector2f screenPosition);
};
