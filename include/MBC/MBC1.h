#pragma once
#include "MBC/baseMBC.h"

class MBC1 : public MBC
{
	public:
		MBC1(const char* filename, uint8_t* bank0);
		~MBC1() override;

		void updateMBC(const uint16_t address, const uint8_t data) override;

		uint8_t readExtRam(const uint16_t address) override;
		uint8_t readSwRom(const uint16_t address) override;

		void writeExtRam(const uint16_t address, const uint8_t data) override;

		uint32_t saveToFile(const char* filename, const uint32_t offset) override;
		uint32_t loadFromFile(const char* filename, const uint32_t offset) override;

	private:
		void swapRomBank(const uint8_t bank);
		void swapRamBank(const uint8_t bank);

		bool  m_modeSelect	= false;
};
