#include "MBC/baseMBC.h"

MBC::MBC(const char* filename, const uint8_t* bank0)
{
	int n = lb_strlen(filename);

	m_savefile = new char[n+2];
	lb_strcpy(m_savefile, filename);

	m_savefile[n - 2]	= 's';
	m_savefile[n - 1]	= 'a';
	m_savefile[n]		= 'v';
	m_savefile[n + 1]	= '\0';

	
	if (bank0 == nullptr) {
		std::cerr << "File failed to open file\n";
		exit(-1);
	}

	//Ram Savable
	switch (bank0[0x147]) {
		case 0x03:
		case 0x06:
		case 0x09:
		case 0x0D:
		case 0x0F:
		case 0x10:
		case 0x13:
		case 0x17:
		case 0x1B:
		case 0x1E:
		case 0x22:
		case 0xFF:
			m_saveBattery = true;
			break;
		default:
			break;
	}

	//ROM Size, # Banks
	switch (bank0[0x148]) {
		case 0:
			m_romSize = 2;		//32KB
			break;
		case 1:
			m_romSize = 4;		//64KB
			break;
		case 2:
			m_romSize = 8;		//128KB
			break;
		case 3:
			m_romSize = 16;		//256KB
			break;
		case 4:
			m_romSize = 32;		//512KB
			break;
		case 5:
			m_romSize = 64;		//1MB
			break;
		case 6:
			m_romSize = 128;	//2MB
			break;
		case 7:
			m_romSize = 256;	//4MB
			break;
		case 8:
			m_romSize = 512;	//8MB
			break;
		default:
			std::cerr << "ROM size can not be determined, may not function correctly: " << std::hex << bank0[0x148] << std::endl;
	}

	//RAM Size, # Banks
	switch (bank0[0x149]) {
		case 0:
			m_ramSize = 0;
			m_ramCap = 0;
			break;
		case 1:
			m_ramSize = 1;
			m_ramCap = 0x800;
			break;
		case 2:
			m_ramSize = 1;
			break;
		case 3:
			m_ramSize = 4;
			break;
		case 4:
			m_ramSize = 16;
			break;
		case 5:
			m_ramSize = 8;
		default:
			std::cerr << "RAM size can not be determined, may not function correctly: " << std::hex << bank0[0x148] << std::endl;
	}


	//Load Entire ROM
	std::ifstream romfile(filename, ios::in | ios::binary);
	if (romfile.is_open()) {
		swRom = new uint8_t[0x4000*m_romSize];
		romfile.read(reinterpret_cast<char*>(swRom), 0x4000*m_romSize);
		romfile.close();
		m_RomBank = 1;
	}
	else {
		std::cerr << "File failed to open: " << filename;
		exit(-1);
	}


	//Load Entire RAM
	if (m_ramCap != 0) {
		ExtRAM = new uint8_t[m_ramCap * m_ramSize];

		//If save is non-volatile
		if (m_saveBattery) {
			std::ifstream savefile(m_savefile, ios::in | ios::binary);

			if (savefile.is_open()) {
				savefile.read(reinterpret_cast<char*>(ExtRAM), m_ramCap * m_ramSize);
				savefile.close();
			}
		}
	}
}

MBC::~MBC()
{
	delete [] m_savefile;
	delete [] ExtRAM;
	delete [] swRom;
	m_savefile = nullptr;
	ExtRAM = nullptr;
	swRom = nullptr;
}


void MBC::updateMBC(const uint16_t address, uint8_t data)
{}

uint8_t MBC::readSwRom(const uint16_t address)
{
	return swRom[(m_RomBank*0x4000) + (address & 0x3FFF)];
}

uint8_t MBC::readExtRam(const uint16_t address)
{
	uint8_t re = 0xFF;
	if (ExtRAM != nullptr)
		re = ExtRAM[(m_RamBank*m_ramCap) + (address & 0x1FFF)];

	return re;
}

void MBC::writeExtRam(const uint16_t address, const uint8_t data)
{
	if (ExtRAM != nullptr)
		ExtRAM[address & 0x1FFF] = data;
}

uint32_t MBC::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::app | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);
		file.write(reinterpret_cast<char*>(&m_RomBank), 1);
		file.write(reinterpret_cast<char*>(&m_RamBank), 1);
		file.write(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		if (ExtRAM != nullptr)
			file.write(reinterpret_cast<char*>(ExtRAM), m_ramSize*m_ramCap);
		file.close();
	}

	return offset + 2 + sizeof(bool) + m_ramSize*m_ramCap;
}

uint32_t MBC::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);
		file.read(reinterpret_cast<char*>(&m_RomBank), 1);
		file.read(reinterpret_cast<char*>(&m_RamBank), 1);
		file.read(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		if (ExtRAM != nullptr)
			file.read(reinterpret_cast<char*>(ExtRAM), m_ramSize*m_ramCap);
		file.close();
	}
	else {
		std::cerr << "File failed to open: " << filename;
		exit(-1);
	}

	


	return offset + 2 + sizeof(bool) + m_ramSize*m_ramCap;
}

void MBC::saveRAM()
{
	std::ofstream savefile(m_savefile, ios::out | ios::binary);
	if (savefile.is_open()) {
		savefile.write(reinterpret_cast<char*>(ExtRAM), m_ramCap*m_ramSize);
		savefile.close();
	}
}
