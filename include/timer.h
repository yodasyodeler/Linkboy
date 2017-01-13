#pragma once
#include "mmu.h"
#include <stdint.h>

class timer {
	static const int gbFrequency = 4194304;
	static const int frequencies[4];
	static const int divInterval = 16384;
	static const int serialInterval = 512;

	public:
		timer(MMU* memory);

		~timer();

		void advanceTimer(const int cycle);

		void loadGame();

		uint32_t saveToFile(const char * filename, const uint32_t offset);

		uint32_t loadFromFile(const char * filename, const uint32_t offset);

		//serial
		bool getMessageOut();
		void clrMessageOut();
		bool getMessageIn();
		void clrMessageIn();

		void clrSerialIn();

	private:
		MMU*		m_memory;

		int			m_divCount			= 0;
		int			m_timerCount		= 0;

		

		int	m_serialCount = 0;

		int m_serialInCount = 0;

		//Should this be here?
		bool m_serialActive	= false;
		bool m_messageOut = false;
		bool m_messageIn = false;
};

