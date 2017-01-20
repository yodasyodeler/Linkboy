#pragma once
#include "MBC/baseMBC.h"

class MBC2 : public MBC
{
	public:
		MBC2(const char* filename, uint8_t* bank0);		
		~MBC2() override;

		void updateMBC(const uint16_t address, const uint8_t data) override;

		uint8_t readExtRam(const uint16_t address) override;
		void writeExtRam(const uint16_t address, const uint8_t data) override;
};