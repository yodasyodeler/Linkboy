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
#include "SFMLSound.h"

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
		void handleSound();


		char*	_saveState = nullptr;

		MMU		_mmu;
		gbz80	_cpu;
		PPU		_ppu;
		APU		_apu;
		timer	_timer;
		Client	_client;
		Clock  	_clock;
		Sound   _sound;

		bool	_pause;

		emulatorSettings _settings = { nullptr, NoOperation, false, 0.0, _mmu.JoyPad, {}, 3, {}, {} };
};
