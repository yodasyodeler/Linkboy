#pragma once
#define USING_SFML_TIME 1

#include "mmu.h"
#include <stdint.h>

#ifdef USING_SDL_TIME
#include <SDL2\SDL.h>
#endif
#ifdef USING_SFML_TIME
#include <SFML\Graphics.hpp>
#endif
#ifdef USING_CHRONO_TIME
#include <chrono>
using std::chrono::nanoseconds;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
#endif


class timer
{
	public:
		timer(MMU* memory);

		~timer();

		void changeSpeed(const int speed);

		void advanceTimer(const int cycle);

		bool advanceCPU();

		bool displayFrame();

		uint32_t saveToFile(const char * filename, const uint32_t offset);

		uint32_t loadFromFile(const char * filename, const uint32_t offset);

		double getFPS();

	private:
		MMU*		m_memory;

		int			m_divCount			= 0;
		int			m_timerCount		= 0;
		int			m_serialCount		= 0;

		double		m_framePerSecond	= 0;
		double		m_frameCount		= 0;

		//Should this be here?
		bool		m_serialActive		= false;

		static const int gbFrequency = 4194304;

		static const double gbperiod;

		static const int frequencies[4];

		static const int divInterval = 16384;
		static const int serialInterval = 512;

#ifdef USING_SDL_TIME
#endif

#ifdef USING_SFML_TIME
		sf::Clock m_clock;
		sf::Time cpuTime;
		sf::Time endTime;
		sf::Time frameTime;
		sf::Time m_frameInterval;
#endif
};

