#pragma once
#include "MBC/baseMBC.h"

struct RTC {
	union {
		struct {
			uint8_t seconds;
			uint8_t minutes;
			uint8_t hours;
			uint8_t days;
			uint8_t flags;
		};
		uint8_t ra[5];
	};
};


class MBC3 : public MBC
{
	public:
		MBC3(const char* filename, uint8_t* bank0);
		~MBC3() override;

		void updateMBC(const uint16_t address, const uint8_t data) override;

		uint8_t readExtRam(const uint16_t address) override;
		void writeExtRam(const uint16_t address, const uint8_t data) override;

		void incTimerRTC();

		uint32_t saveToFile(const char* filename, const uint32_t offset) override;
		uint32_t loadFromFile(const char* filename, const uint32_t offset) override;

	private:
		void swapRomBank(const uint8_t bank);
		void swapRamBank(const uint8_t bank);

		bool m_RTC			=	false;
		
		bool m_readRTC		=	false;

		bool m_lastLatch	=	false;

		RTC m_latchReg		=	{};
		RTC m_timerReg		=	{};
};