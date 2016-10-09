#pragma once
#include <SFML\Graphics.hpp>
#include <iostream>

struct emulatorSettings {
	bool pause;
	bool displayHelp;
	bool displayOptions;
	int color;
	int speed;
	int breakpoint;
	int saveState;
	const char* loadGameFile;
	double framesPerSecond;
	uint16_t& joyPad;
};

void initScreen(const int winX, const int winY, const uint32_t* pix, const char* dirName = nullptr);

void renderScreen();

bool handleEvents(emulatorSettings& settings);
