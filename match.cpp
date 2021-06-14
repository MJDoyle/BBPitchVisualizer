#include "match.hpp"

#include "player.hpp"

//Constructor
Match::Match(std::shared_ptr<sf::RenderWindow> window) : _zoom(2), _zoomSpeed(0), _dragging(false), _dragStart(0, 0)
{
	_window = window;

	//Load textures and set up sprites
	LoadTexturesAndCreateSprites();

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
		view.move(sf::Vector2f(_dragStart - mousePos));

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
		view.move(sf::Vector2f(0, -10));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		sf::View view = _window->getView();
		view.move(sf::Vector2f(0, 10));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		sf::View view = _window->getView();
		view.move(sf::Vector2f(-10, 0));
		_window->setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		sf::View view = _window->getView();
		view.move(sf::Vector2f(10, 0));
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

			//Quit
			else if (event.key.code == sf::Keyboard::Escape)
				_window->close();
		}

		else if (event.type == sf::Event::MouseButtonPressed)
		{
			///////////
			// Select player
			///////////
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//Check if there is a player in that tileposition
				if (_players.count(TilePositionFromScreenPosition(mouseCoords)))
				{
					//Select it if so
					_selectedPlayer = _players[TilePositionFromScreenPosition(mouseCoords)];
				}
			}

			///////////
			// Start camera dragging on middle or right mouse
			///////////
			else if (event.mouseButton.button == sf::Mouse::Middle || event.mouseButton.button == sf::Mouse::Right)
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
			// Release selected player
			///////////
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//If htere is a selected player
				if (_selectedPlayer != nullptr)
				{
					//Save the old pos in order to remove from the player map at the end
					sf::Vector2i oldPos = _selectedPlayer->GetPosition();

					//Get the new pos
					sf::Vector2i newPos = TilePositionFromScreenPosition(mouseCoords);

					//If there isn't another pawn in the ew pos
					if (!_players.count(newPos))
					{
						//Set the newpos elemt of the map to point at the pawn from the old pos
						_players[newPos] = _players[oldPos];

						//Set the new pos of that pawn
						_players[newPos]->SetPosition(newPos);

						//Remove the oldpos element
						_players.erase(oldPos);
					}

					_selectedPlayer.reset();
				}
			}




			///////////
			// Stop camera dragging
			///////////
			else if (event.mouseButton.button == sf::Mouse::Middle || event.mouseButton.button == sf::Mouse::Right)
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

//Draw the current state
void Match::Draw()
{
	///////////
	// Get mouse coords
	///////////

	sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
	sf::Vector2f mouseCoords = _window->mapPixelToCoords(mousePos);

	///////////
	// Clear window
	///////////

	_window->clear(sf::Color::White);

	///////////
	// Draw pitch
	///////////

	_window->draw(*_pitchSprite);

	///////////
	// Draw players
	///////////

	for (auto player = _players.begin(); player != _players.end(); player++)
	{

		//Each player is just a circle, red or blue
		sf::CircleShape circle;
		circle.setRadius(10);
		circle.setOrigin(10, 10);

		//Colour depends on team
		if (player->second->GetTeam() == TEAM_ONE)
			circle.setFillColor(sf::Color(0, 0, 220, 150));

		else if (player->second->GetTeam() == TEAM_TWO)
			circle.setFillColor(sf::Color(220, 0, 0, 150));

		//Non-selected players are drawn at their position
		if (player->second != _selectedPlayer)
		{
			circle.setPosition(ScreenPositionFromTilePosition(player->first));
		}

		//Selected players are drawn at the tile position of the mouse cursor, and given a green highliting rectangle
		else
		{
			//Create highlight rectangle
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
			rect.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
			rect.setFillColor(sf::Color(0, 200, 0, 150));
			rect.setPosition(ScreenPositionFromTilePosition(TilePositionFromScreenPosition(mouseCoords)));

			_window->draw(rect);

			//Set hte position to be the tile corresponding to the mouse cursor
			circle.setPosition(ScreenPositionFromTilePosition(TilePositionFromScreenPosition(mouseCoords)));
		}

		_window->draw(circle);
	}

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