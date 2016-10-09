#include "MBC/MBC1.h"

MBC1::MBC1(const char* filename, uint8_t* bank0)
	:MBC(filename, bank0)
{
	switch (bank0[0x147]) {
		case 0x01:
		case 0x02:
		case 0x03:
			break;
		default:
			std::cerr << "Invalid MBC1 value: " << ios::hex << bank0[0x147];
			exit(-1);
	};
}

MBC1::~MBC1()
{}

void MBC1::updateMBC(const uint16_t address, const uint8_t data)
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
		case 3:
			swapRomBank(data);
			break;
		case 4:
		case 5:
			swapRamBank(data);
			break;
		case 6:
		case 7:
			m_modeSelect = data & 1;
			break;
		default:
			std::cerr << "MBC1 error: doesn't recognise address:" << ios::hex << address << " data:" << data << std::endl;
			exit(-1);
			break;
	}

}

uint8_t MBC1::readExtRam(const uint16_t address)
{
	uint8_t re = 0xFF;
	uint8_t temp =	(m_modeSelect ? m_RamBank : 0x00);

	if (ExtRAM != nullptr && m_enableRAM)
		re = ExtRAM[(temp*m_ramCap) + (address & 0x1FFF)];

	return re;
}

uint8_t MBC1::readSwRom(const uint16_t address)
{
	uint8_t temp = m_RomBank;
	temp |= (m_modeSelect ? 0 : (m_RamBank<<4));

	return swRom[(temp*0x4000) + (address & 0x3FFF)];
}

void MBC1::writeExtRam(const uint16_t address, const uint8_t data)
{
	uint8_t temp =	(m_modeSelect ? m_RamBank : 0x00);

	if (ExtRAM != nullptr && m_enableRAM)
		ExtRAM[(temp*m_ramCap) + (address & 0x1FFF)] = data;

}

uint32_t MBC1::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::out | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);
		file.write(reinterpret_cast<char*>(&m_RomBank), 1);
		file.write(reinterpret_cast<char*>(&m_RamBank), 1);
		file.write(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		//MBC1 data
		file.write(reinterpret_cast<char*>(&m_modeSelect), sizeof(bool));


		if (ExtRAM != nullptr)
			file.write(reinterpret_cast<char*>(ExtRAM), m_ramSize*m_ramCap);
		file.close();
	}

	return offset + 2 + (sizeof(bool)*2) +  m_ramSize*m_ramCap;
}

uint32_t MBC1::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);
		file.read(reinterpret_cast<char*>(&m_RomBank), 1);
		file.read(reinterpret_cast<char*>(&m_RamBank), 1);
		file.read(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		//MBC1 data
		file.read(reinterpret_cast<char*>(&m_modeSelect), sizeof(bool));

		if (ExtRAM != nullptr)
			file.read(reinterpret_cast<char*>(ExtRAM), m_ramSize*m_ramCap);
		file.close();
	}
	else {
		std::cerr << "File failed to open: " << filename << "\n";
		exit(-1);
	}

	swapRomBank(m_RomBank);
	swapRamBank(m_RamBank);

	return offset + 2 + (sizeof(bool)*2) +  m_ramSize*m_ramCap;
}

void MBC1::swapRomBank(const uint8_t bank)
{
	m_RomBank = bank & 0x1F;

	if (!m_RomBank)
		m_RomBank = 1;
}

void MBC1::swapRamBank(const uint8_t bank)
{
	m_RamBank = bank & 0x3;
}

