#pragma once
#include <string>
#include "gbz80.h"
#include "mmu.h"
#include "ppu.h"
#include "apu.h"
#include "timer.h"
#include "linkboyClient.h"
#include "Display.h"
#include "common.h"

#define USING_SFML_TIME 1

class linkboy {


	public:
		linkboy( const char* dirName, const char* filename = nullptr, BaseLogger* log = nullptr);
		~linkboy();

		void startEmulation();

	private:
		void saveState();
		void loadState();

		void checkDebug();

		void loadGame(const char* filename);

		void handleNetwork();
		void handleSettings();

		void changeSpeed(const int speed);
		bool checkTime();
		bool checkFrameTime();

		char*	_saveState = nullptr;

		MMU		_memory;
		gbz80	_cpu;
		PPU		_display;
		APU		_sound;
		timer	_timer;
		Client	_client;

		bool	_pause;

	static const double gbperiod;
#ifdef USING_SFML_TIME
		sf::Clock _clock;
		sf::Time  _cpuTime;
		sf::Time  _endTime;
		sf::Time  _frameTime;
		sf::Time  _frameInterval;
#endif
		double		_framePerSecond	= 0;
		double		_frameCount		= 0;

		emulatorSettings _settings = {false, false, false, {}, 3, nullptr, 0.0, _memory.JoyPad, NoOperation, {} };
};
