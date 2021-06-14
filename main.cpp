//A simple tool for visualizing blood bowl game states

#include "match.hpp"

int main()
{
	//ShowWindow(::GetConsoleWindow(), SW_HIDE);

	///////////
	// Load settings from config file
	///////////

	bool fullscreen = false;

	//Open the config file
	std::ifstream configFile("config.txt");

	//Read fullscreen bool
	std::string fullscreenLine;
	std::getline(configFile, fullscreenLine);

	//Read x resolution
	std::string resxLine;
	std::getline(configFile, resxLine);

	//Read y resolution
	std::string resyLine;
	std::getline(configFile, resyLine);

	//Determine whether the fullscreen line is true or false
	if (fullscreenLine.find("true") != std::string::npos || fullscreenLine.find("True") != std::string::npos || fullscreenLine.find("TRUE") != std::string::npos)
		fullscreen = true;

	//Extract the x resolution from the string
	std::string resxString = std::regex_replace(resxLine, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));

	//Set the x resolution
	if (resxString.size())
		SCREEN_SIZE.x = std::stoi(resxString);

	//Extract the y resolution from the string
	std::string resyString = std::regex_replace(resyLine, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));

	//Set the y resolution
	if (resyString.size())
		SCREEN_SIZE.y = std::stoi(resyString);


	std::shared_ptr<sf::RenderWindow> window;

	//Create window as fullscreen or windowed
	if (fullscreen)
		window = std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(SCREEN_SIZE.x, SCREEN_SIZE.y), "Pitch Visualizer", sf::Style::Fullscreen));

	else
		window = std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(SCREEN_SIZE.x, SCREEN_SIZE.y), "Pitch Visualizer"));

	//Create the match - everything happens in here
	std::shared_ptr<Match> match = std::shared_ptr<Match>(new Match(window));

	//Run the match
	match->Run();

	return 1;
}