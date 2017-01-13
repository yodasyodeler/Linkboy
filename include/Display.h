#pragma once
#include <SFML/Graphics.hpp>
#include "SFML/Network.hpp"
#include <iostream>

enum settingOperation {
	NoOperation = 0,
	LoadGame,
	ChangeColor,
	ChangeSpeed,
	SaveGameState,
	LoadGameState,
	ConnectToServer,
	ReadLobby,
	JoinLobby
};

struct networkSettings {
	bool connected;
	sf::IpAddress ip;
	unsigned short port;
	const char* name;
};

struct lobbySettings {
	int lobbyIndex;
	const char* lobbyName[8]; //8 to buffer one up and one down
};

struct emulatorSettings {
	bool pause;
	bool displayHelp;
	bool displayOptions;
	int color;
	int speed;
	const char* loadGameFile;
	double framesPerSecond;
	uint16_t& joyPad;
	settingOperation operation;
	networkSettings network;
	lobbySettings lobby;
};

void initScreen(const int winX, const int winY, const uint32_t* pix, const char* dirName = nullptr);

void renderScreen();

bool handleEvents(emulatorSettings& settings);
