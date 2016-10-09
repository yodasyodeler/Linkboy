#include "MBC/MBC5.h"

MBC5::MBC5(const char * filename, uint8_t * bank0)
	:MBC(filename, bank0)
{
	switch (bank0[0x147]) {
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		break;
	default:
		std::cerr << "Invalid MBC1 value: " << ios::hex << bank0[0x147];
		exit(-1);
	};
}

MBC5::~MBC5()
{
}

void MBC5::updateMBC(const uint16_t address, const uint8_t data)
{
	uint16_t temp = (address & 0xF000) >> 12;

	switch (temp) {
	case 0:
	case 1:
		if (m_saveBattery && m_enableRAM && (data & 0xF) != 0x0A)
			saveRAM();
		m_enableRAM = (data & 0xF) == 0xA;
		break;
	case 2:
		m_RomBank = data | (m_RomBank&0x100);
		break;
	case 3:
		m_RomBank = ((data&1)<<8) | (0xF&m_RomBank);
		break;
	case 4:
	case 5:
		m_RamBank = data % m_ramSize;
		break;
	case 6:
	case 7:
		break;
	default:
		std::cerr << "MBC1 error: doesn't recognise address:" << ios::hex << address << " data:" << data << std::endl;
		exit(-1);
		break;
	}

}
