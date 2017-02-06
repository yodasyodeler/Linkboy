#pragma once
#include <string>
#include "gbz80.h"
#include "mmu.h"
#include "ppu.h"
#include "apu.h"
#include "timer.h"
#include "linkboyClient.h"
#include "common.h"

#include "Display.h"
#include "SFMLClock.h"

//#define USING_SFML_TIME 0

class linkboy {
	static const double gbperiod;

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

		char*	_saveState = nullptr;

		MMU		_memory;
		gbz80	_cpu;
		PPU		_display;
		APU		_sound;
		timer	_timer;
		Client	_client;
		Clock  	_clock;

		bool	_pause;

		emulatorSettings _settings = { nullptr, NoOperation, false, 0.0, _memory.JoyPad, {}, 3, {}, {} };
};
