#include "MBC/MBC2.h"

MBC2::MBC2(const char* filename, uint8_t* bank0)
	:MBC(filename, bank0)
{
	switch (bank0[0x147]) {
		case 5:
		case 6:
			break;
		default:
			std::cerr << "Invalid MBC2 value: " << ios::hex << bank0[0x147];
			exit(-1);
	}
	if (ExtRAM != nullptr) {
		std::cerr << "Invalid MBC2 Ram \n";
		exit(-2);
	}
	ExtRAM = new uint8_t[512];
}		

MBC2::~MBC2()
{
	delete [] m_savefile;
	delete [] ExtRAM;
	delete [] swRom;
	m_savefile = nullptr;
	ExtRAM = nullptr;
	swRom = nullptr;
}

void MBC2::updateMBC(const uint16_t address, const uint8_t data)
{
	uint16_t temp = (address>>12) & 0xF;

	switch (temp) {
		case 0:
		case 1:
			//if (address & 0x100) {
				if (m_saveBattery && m_enableRAM && (data & 0xF) != 0x0A)
					saveRAM();
				m_enableRAM = (data & 0xF) == 0xA;
			//}
			break;
		case 2:
		case 3:
			if (address & 0x100) {
				m_RomBank = data&0xF;
				if (!m_RomBank)
					m_RomBank = 1;
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		default:
			std::cerr << "MBC2 error: doesn't recognize address:" << std::hex << address << " data:" << data << std::endl;
			exit(-1);
			break;

	}
}

uint8_t MBC2::readExtRam(const uint16_t address)
{
	uint8_t re = 0;

	if (m_enableRAM)
		re = ExtRAM[address & 0x1FF];

	return re;
}

void MBC2::writeExtRam(const uint16_t address, const uint8_t data)
{
	if (m_enableRAM) 
		ExtRAM[address&0x1FF] = data & 0xF;
}
