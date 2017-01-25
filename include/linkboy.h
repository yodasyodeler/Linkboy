#pragma once
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

	static const double gbperiod;

	public:
		linkboy(const char* dirName, const char* filename = nullptr);
		linkboy(BaseLogger* log, const char* dirName, const char* filename = nullptr);
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

		char*	m_saveState = nullptr;

		MMU		m_memory;
		gbz80	m_cpu;
		PPU		m_display;
		APU		m_sound;
		timer	m_timer;
		Client	m_client;
		bool	m_pause;

#ifdef USING_SFML_TIME
		sf::Clock m_clock;
		sf::Time  cpuTime;
		sf::Time  endTime;
		sf::Time  frameTime;
		sf::Time  m_frameInterval;
#endif
		double		m_framePerSecond	= 0;
		double		m_frameCount		= 0;

		emulatorSettings m_settings = {false, false, false, 1, 3, nullptr, 0.0, m_memory.JoyPad, NoOperation, {} };
};
