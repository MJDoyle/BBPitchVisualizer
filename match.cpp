#include "match.hpp"

#include "player.hpp"

//Constructor
Match::Match(std::shared_ptr<sf::RenderWindow> window) : _zoom(2), _zoomSpeed(0), _dragging(false), _dragStart(0, 0), _deploymentPhase(true)
{
	_window = window;

	//Load textures and set up sprites
	LoadTexturesAndCreateSprites();

	//Load font 
	LoadFont();

	//Add the starting players
	AddDefaultStartingPlayers();
}

//Main loop
void Match::Run()
{
	while (_window->isOpen())
	{
		//Limit FPS
		if (_updateClock.getElapsedTime().asMilliseconds() > DT)
		{
			HandleEvents();

			Update();

			Draw();
		}
	}

	return;
}

//Handle user input
void Match::HandleEvents()
{
	///////////
	// Get mouse coords
	///////////

	sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
	sf::Vector2f mouseCoords = _window->mapPixelToCoords(mousePos);

	///////////
	// Handle scrolling with mouse
	///////////

	if (_dragging)
	{
		//Get current view
		sf::View view = _window->getView();

		//Move the view towards the drag start (as the mouse moves right, the view moves left)
		sf::Vector2f delta = sf::Vector2f(_dragStart - mousePos);

		//Rotate move direction according to view rotation
		view.move(sf::Vector2f(std::cos(view.getRotation() * std::_Pi / float(180)) * delta.x - std::sin(view.getRotation() * std::_Pi / float(180)) * delta.y, 
			std::sin(view.getRotation() * std::_Pi / float(180)) * delta.x + std::cos(view.getRotation() * std::_Pi / float(180)) * delta.y));

		//Reset the drag start position for the next loop
		_dragStart = mousePos;

		//Set the moved loop
		_window->setView(view);
	}

	///////////
	// Handle scrolling with keyboard
	///////////

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		sf::View view = _window->getView();
		view.move(SCROLL_SPEED * sf::Vector2f(std::sin(view.getRotation() * std::_Pi / float(180)), -std::cos(view.getRotation() * std::_Pi / float(180))));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		sf::View view = _window->getView();
		view.move(SCROLL_SPEED * sf::Vector2f(-std::sin(view.getRotation() * std::_Pi / float(180)), std::cos(view.getRotation() * std::_Pi / float(180))));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		sf::View view = _window->getView();
		view.move(SCROLL_SPEED * sf::Vector2f(-std::cos(view.getRotation() * std::_Pi / float(180)), -std::sin(view.getRotation() * std::_Pi / float(180))));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		sf::View view = _window->getView();
		view.move(SCROLL_SPEED * sf::Vector2f(std::cos(view.getRotation() * std::_Pi / float(180)), std::sin(view.getRotation() * std::_Pi / float(180))));
		_window->setView(view);
	}

	///////////
	// Get discrete events
	///////////

	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			///////////
			// Center camera
			///////////
			if (event.key.code == sf::Keyboard::C)
			{
				//Default zoom and zoomspeed values
				_zoom = 2;
				_zoomSpeed = 0;

				//Default view position
				sf::View view = _window->getView();
				view.setCenter(SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2);
				_window->setView(view);
			}

			///////////
			// Rotate camera
			///////////
			else if (event.key.code == sf::Keyboard::R)
			{
				//Get view
				sf::View view = _window->getView();

				std::cout << "ROTATION: " << view.getRotation() << std::endl;

				//Rotate view left or right
				if (view.getRotation() == 0)
				{
					view.rotate(-90);
				}

				else if (view.getRotation() == 270)
				{
					view.rotate(90);
				}

				//Set rotated view
				_window->setView(view);
			}

			//Start a turn which can be played through
			else if (event.key.code == sf::Keyboard::T)
			{
				if (_deploymentPhase)
					_deploymentPhase = false;

				else
					_deploymentPhase = true;
			}

			//Quit
			else if (event.key.code == sf::Keyboard::Escape)
				_window->close();
		}

		else if (event.type == sf::Event::MouseButtonPressed)
		{
			///////////
			// Start camera dragging on middle or right mouse
			///////////
			if (event.mouseButton.button == sf::Mouse::Middle)
			{
				_dragging = true;
				_dragStart = mousePos;
			}
		}

		else if (event.type == sf::Event::Closed)
			_window->close();

		else if (event.type == sf::Event::MouseButtonReleased)
		{
			///////////
			// Left click
			///////////
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//Select player in that position if there is one

				//Check if there is a player in that tileposition
				if (_players.count(TilePositionFromScreenPosition(mouseCoords)))
				{
					//Select it if so
					_selectedPlayer = _players[TilePositionFromScreenPosition(mouseCoords)];
				}

				//if there is not, and a player is selected, move the player there (setup phase)
				else if (_selectedPlayer != nullptr)
				{
					//Get the new pos
					sf::Vector2i newPos = TilePositionFromScreenPosition(mouseCoords);

					//Check the player can be moved to target location
					if (PITCH.contains(newPos) || SUBS_ONE.contains(newPos) || SUBS_TWO.contains(newPos))
					{

						//Save the old pos in order to remove from the player map at the end
						sf::Vector2i oldPos = _selectedPlayer->GetPosition();

						//Move the player if it's deployment phase
						if (_deploymentPhase)
							DeployPlayer(0, oldPos, newPos);
					}
				}
			}

			///////////
			// Right click
			///////////
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				//If thtere is a selected player, deselct it
				if (_selectedPlayer != nullptr)
				{
					_selectedPlayer.reset();
				}
			}



			///////////
			// Stop camera dragging
			///////////
			else if (event.mouseButton.button == sf::Mouse::Middle)
				_dragging = false;
		}

		///////////
		// Scroll view in and out
		///////////
		else if (event.type == sf::Event::MouseWheelMoved)
		{
			if (event.mouseWheel.delta < 0)
				_zoomSpeed = 0.1;

			else
				_zoomSpeed = -0.1;
		}
	}
}

//Move a player in deployment phase
void Match::DeployPlayer(int team, sf::Vector2i playerOldPosition, sf::Vector2i playerNewPosition)
{
	//Check that the correct team is active

	//Check that there are at most 11 players on the pitch

	//Set the newpos elemt of the map to point at the pawn from the old pos
	_players[playerNewPosition] = _players[playerOldPosition];

	//Set the new pos of that pawn
	_players[playerNewPosition]->SetPosition(playerNewPosition);

	//Remove the oldpos element
	_players.erase(playerOldPosition);
}

//Draw the current state
void Match::Draw()
{
	///////////
	// Get mouse coords
	///////////

	sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
	sf::Vector2f mouseCoords = _window->mapPixelToCoords(mousePos);

	//Get the current window view
	sf::View view = _window->getView();

	///////////
	// Clear window
	///////////

	_window->clear(sf::Color::White);

	///////////
	// Draw pitch
	///////////

	_window->draw(*_pitchSprite);

	///////////
	// Draw grid notation
	///////////

	//Numbers
	for (int i = 1; i != 27; i++)
	{
		//Numbers
		sf::Text numText;
		numText.setFont(*_font);
		numText.setCharacterSize(16);
		numText.setFillColor(sf::Color::Black);
		numText.setPosition(750, 875 - i * TILE_SIZE);

		if (view.getRotation() == 270)
			numText.setRotation(270);

		numText.setString(std::to_string(i));
		numText.setOrigin(numText.getLocalBounds().width / 2, numText.getLocalBounds().height / 2);

		_window->draw(numText);
	}

	//Letters
	char letters[15] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O' };

	for (int i = 0; i != 15; i++)
	{
		//Letters
		sf::Text letText;
		letText.setFont(*_font);
		letText.setCharacterSize(16);
		letText.setFillColor(sf::Color::Black);
		letText.setPosition(785 + i * TILE_SIZE, 875);

		if (view.getRotation() == 270)
			letText.setRotation(270);

		letText.setString(letters[i]);
		letText.setOrigin(letText.getLocalBounds().width / 2, letText.getLocalBounds().height / 2);

		_window->draw(letText);
	}

	///////////
	// Draw players
	///////////

	//Draw selected player move range
	if (_selectedPlayer != nullptr && !_deploymentPhase)
	{
		std::map<sf::Vector2i, int, Vector2iCompare> moveTiles = GetTilesAccessibleFromPos(_selectedPlayer->GetPosition(), _selectedPlayer->GetTeam(), _selectedPlayer->GetRemaningMa());

		//Highlight each moveable tile in green
		for (auto tile = moveTiles.begin(); tile != moveTiles.end(); tile++)
		{
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
			rect.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
			rect.setFillColor(sf::Color(0, 200, 0, 100));
			rect.setPosition(ScreenPositionFromTilePosition(tile->first));

			_window->draw(rect);
		}
	}

	//Draw selected player square
	if (_selectedPlayer != nullptr)
	{
		//Create highlight rectangle
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
		rect.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
		rect.setFillColor(sf::Color(0, 200, 0, 150));
		rect.setPosition(ScreenPositionFromTilePosition(_selectedPlayer->GetPosition()));

		_window->draw(rect);
	}

	//Draw players
	for (auto player = _players.begin(); player != _players.end(); player++)
	{
		//If if turn phase, draw status rings
		if (!_deploymentPhase)
		{
			sf::CircleShape ring;
			ring.setRadius(12);
			ring.setOrigin(12, 12);

			if (player->second == _activePlayer)
				ring.setFillColor(sf::Color::Green);

			else if (player->second->GetTurnComplete())
				ring.setFillColor(sf::Color::Red);

			else
				ring.setFillColor(sf::Color::White);

			ring.setPosition(ScreenPositionFromTilePosition(player->first));

			_window->draw(ring);

		}

		//Each player is just a circle, red or blue
		sf::CircleShape circle;
		circle.setRadius(10);
		circle.setOrigin(10, 10);

		//Colour depends on team
		if (player->second->GetTeam() == TEAM_ONE)
			circle.setFillColor(sf::Color(173, 199, 207));

		else if (player->second->GetTeam() == TEAM_TWO)
			circle.setFillColor(sf::Color(255, 105, 97));

		circle.setPosition(ScreenPositionFromTilePosition(player->first));

		_window->draw(circle);
	}

	//Draw highlight circle for selected player
	if (_selectedPlayer != nullptr)
	{
		sf::CircleShape circle;
		circle.setRadius(10);
		circle.setOrigin(10, 10);
		circle.setFillColor(sf::Color(255, 255, 255, 50));
		circle.setPosition(ScreenPositionFromTilePosition(TilePositionFromScreenPosition(mouseCoords)));
		_window->draw(circle);
	}

	///////////
	// Draw UI
	///////////

	//Reset the window view
	_window->setView(_window->getDefaultView());

	//Draw the UI elements
	sf::Text turnState;
	turnState.setFont(*_font);
	turnState.setCharacterSize(16);
	turnState.setPosition(50, 50);
	turnState.setFillColor(sf::Color::Black);

	if (_deploymentPhase)
		turnState.setString("Your deployment");

	else
		turnState.setString("Your turn");

	_window->draw(turnState);


	//Return the view to it's previous
	_window->setView(view);

	_window->display();
}

void Match::Update()
{
	///////////
	// Handle zooming
	///////////

	_zoom += _zoomSpeed;

	if (_zoom < 0.5)
		_zoom = 0.5;

	else if (_zoom > 8)
		_zoom = 8;

	if (_zoomSpeed > 0.01)
		_zoomSpeed -= 0.01;

	else if (_zoomSpeed < -0.01)
		_zoomSpeed += 0.01;

	else
		_zoomSpeed = 0;

	sf::View view = _window->getView();
	view.setSize(0.5 * _zoom * sf::Vector2f(SCREEN_SIZE));
	_window->setView(view);
}

//Set up the starting players - just add 16 into each substitution box
void Match::AddDefaultStartingPlayers()
{
	//Subs box positions for team two
	for (int i = -4; i != -2; i++)
	{
		for (int j = 2; j != 10; j++)
		{
			_players[sf::Vector2i(i, j)] = std::shared_ptr<Player>(new Player(TEAM_TWO, sf::Vector2i(i, j)));
		}
	}

	//Subs box positions for team one
	for (int i = 17; i != 19; i++)
	{
		for (int j = 16; j != 24; j++)
		{
			_players[sf::Vector2i(i, j)] = std::shared_ptr<Player>(new Player(TEAM_ONE, sf::Vector2i(i, j)));
		}
	}

}

//Convert tile position into pixel position
sf::Vector2f Match::ScreenPositionFromTilePosition(sf::Vector2i playerPosition)
{
	return sf::Vector2f(SCREEN_SIZE / 2) + sf::Vector2f(-175, 312.5) + sf::Vector2f(playerPosition.x * TILE_SIZE, -playerPosition.y * TILE_SIZE);
}

//Convert pixel position into tile position
sf::Vector2i Match::TilePositionFromScreenPosition(sf::Vector2f screenPosition)
{
	int posx;
	int posy;

	posx = (int(screenPosition.x) - SCREEN_SIZE.x / 2 + 175 + 12.5) / TILE_SIZE;

	if (screenPosition.x < float(SCREEN_SIZE.x / 2 - 175 - 12.5))
		posx--;

	posy = (int(screenPosition.y) - SCREEN_SIZE.y / 2 - 312.5 - 12.5) / TILE_SIZE;

	if (screenPosition.y > SCREEN_SIZE.y / 2 + 312.5 + 12.5)
		posy++;


	sf::Vector2i pos = sf::Vector2i(posx, posy);

	pos.y = -pos.y;

	return pos;
}

//From a given point, return a set of all possible tiles that can be reached by a player in the tile without having to roll dice
std::map<sf::Vector2i, int, Vector2iCompare> Match::GetTilesAccessibleFromPos(sf::Vector2i pos, int team, int range)
{
	//Standard breadth-first search

	std::map<sf::Vector2i, int, Vector2iCompare> openSet;

	std::map<sf::Vector2i, int, Vector2iCompare> closedSet;

	//Add start pos to open set
	openSet[pos] = 0;

	while (openSet.size())
	{
		//Get the position and distance of the last tile in the openset
		sf::Vector2i position = std::prev(openSet.end())->first;

		//The distance of the tile from the starting tile
		int distance = std::prev(openSet.end())->second;

		//If the tile is within range, get the neighbours
		if (distance < range)
		{
			//Get the neighbours of this tile
			std::vector<sf::Vector2i> neighbours = GetTileAccessibleNeighbours(position);

			//Iterate through neighbours and check if they are already in a set
			for (auto neighbour = neighbours.begin(); neighbour != neighbours.end(); neighbour++)
			{
				//If the neighbour is already in the closed set, with a shorter or the same distance, move on to the next one
				if (!closedSet.count(*neighbour) || closedSet[*neighbour] > distance + 1)
				{
					//If the neighbour isn't in the open set, add it
					if (!openSet.count(*neighbour))
					{
						openSet[*neighbour] = distance + 1;
					}

					//If the neighbour is already in the open set, but with a larger distance, replace it
					else if (openSet[*neighbour] > distance + 1)
					{
						openSet[*neighbour] = distance + 1;
					}
				}
			}
		}

		//Check if the tile is either not in the closed, or in with a large distance. If so, add it/replace it

		if (!closedSet.count(position) || closedSet[position] > distance + 1)
		{
			closedSet[position] = distance;
		}

		//Remove the tile from the open set
		openSet.erase(position);
	}

	return closedSet;
}

//Get neighbouring tiles of a given tile
std::vector<sf::Vector2i> Match::GetTileAccessibleNeighbours(sf::Vector2i pos)
{
	std::vector<sf::Vector2i> neighbours;

	//Look at all the tiles in a 3x3 square centered on pos. Neglect pos, and all other tiles that are off the pitch
	for (int i = pos.x - 1; i <= pos.x + 1; i++)
	{
		for (int j = pos.y - 1; j <= pos.y + 1; j++)
		{
			if (sf::Vector2i(i, j) != pos && PITCH.contains(sf::Vector2i(i, j)))
			{
				//Neglect tiles that are filled by a player
				if(!_players.count(sf::Vector2i(i, j)))
					neighbours.push_back(sf::Vector2i(i, j));
			}
		}
	}

	return neighbours;
}















//Load textures and create sprites
void Match::LoadTexturesAndCreateSprites()
{
	//create and load texture
	_pitchTexture = std::shared_ptr<sf::Texture>(new sf::Texture());

	_pitchTexture->loadFromFile("pitchTexture.png");

	_pitchTexture->setSmooth(true);

	_pitchSprite = std::shared_ptr<sf::Sprite>(new sf::Sprite());

	//Set up sprite
	_pitchSprite->setTexture(*_pitchTexture);
	_pitchSprite->setTextureRect(sf::IntRect(0, 0, 800, 1080));
	_pitchSprite->setOrigin(400, 540);
	_pitchSprite->setPosition(SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2);
}

//Load font
void Match::LoadFont()
{
	_font = std::shared_ptr<sf::Font>(new sf::Font());

	_font->loadFromFile("arial.ttf");
}