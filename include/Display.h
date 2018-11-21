#pragma once
#include <SFML/Graphics.hpp>
#include "SFML/Network.hpp"
#include "common.h"
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
	JoinLobby,
	ToggleBackground,
	ToggleWindow,
	ToggleSprites
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
	const char* loadGameFile;
	settingOperation operation;
	bool pause;
	double fps;
	uint16_t& joyPad;
	Color color[4];
	int speed;
	networkSettings network;
	lobbySettings lobby;
};

void initScreen(const int winX, const int winY, const uint32_t* pix, const char* dirName = nullptr);

void renderScreen();

bool handleEvents(emulatorSettings& settings);
