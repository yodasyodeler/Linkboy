#include "MBC/MBC3.h"

MBC3::MBC3(const char* filename, uint8_t* bank0)
	:MBC(filename, bank0)
{
	switch (bank0[0x147]) {
		case 0x0F:
		case 0x10:
			m_RTC  = true;
		case 0x11:
		case 0x12:
		case 0x13:
			break;
		default:
			std::cerr << "Invalid MBC3 value: " << ios::hex << bank0[0x147];
			exit(-1);
	}
}

MBC3::~MBC3()
{
	delete [] m_savefile;
	delete [] ExtRAM;
	delete [] swRom;
	m_savefile = nullptr;
	ExtRAM = nullptr;
	swRom = nullptr;
}

void MBC3::updateMBC(const uint16_t address, const uint8_t data)
{
	uint16_t temp = (address >> 12) & 0xF;

	switch (temp) {
	case 0:
	case 1:
		if (m_saveBattery && m_enableRAM && (data & 0xF) != 0xA)
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
		if (m_enableRAM && m_RTC) {
			if (m_lastLatch && data == 0x1)
				m_latchReg = m_timerReg;
			m_lastLatch = data == 0;
		}
		break;	
	default:
		std::cerr << "MBC3 error: doen't recognize address:" << std::hex << address << " data:" << data << std::endl;
		exit(-1);
		break;
	}

}

uint8_t MBC3::readExtRam(const uint16_t address)
{
	uint8_t re = 0xFF;

	if (m_enableRAM) {
		if (m_readRTC)
			re = m_latchReg.ra[m_RamBank-8];
		else if (ExtRAM != nullptr)
			re = ExtRAM[(m_RamBank*m_ramCap) + (address & 0x1FFF)];
	}
	return re;
}

void MBC3::writeExtRam(const uint16_t address, uint8_t data)
{
	if (m_enableRAM && m_RamBank <= 3)
		ExtRAM[(m_RamBank*m_ramCap) + (address & 0x1FFF)] = data;
	else if (m_enableRAM && m_readRTC)
		m_timerReg.ra[m_RamBank - 8] = data;
}

void MBC3::incTimerRTC()
{
	if (!(m_timerReg.flags | (1<<6))) {
		if ((++m_timerReg.seconds) >= 60) {
			m_timerReg.seconds = 0;
			if ((++m_timerReg.minutes) >= 60) {
				m_timerReg.minutes = 0;
				if ((++m_timerReg.hours) >= 24) {
					m_timerReg.hours = 0;
					if ((++m_timerReg.days) == 0) {
						if (m_timerReg.flags | 1) {
							m_timerReg.flags = (1 << 7);
						}
						else
							m_timerReg.flags |= 1;
					}
				}
			}
		}
	}
}

uint32_t MBC3::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::out | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);
		file.write(reinterpret_cast<char*>(&m_RomBank), 1);
		file.write(reinterpret_cast<char*>(&m_RamBank), 1);
		file.write(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		//MBC3 data
		file.write(reinterpret_cast<char*>(&m_readRTC), sizeof(bool));
		file.write(reinterpret_cast<char*>(&m_lastLatch), sizeof(bool));
		file.write(reinterpret_cast<char*>(&m_latchReg), sizeof(RTC));
		file.write(reinterpret_cast<char*>(&m_timerReg), sizeof(RTC));

		if (ExtRAM != nullptr)
			file.write(reinterpret_cast<char*>(ExtRAM), m_ramSize*m_ramCap);
		file.close();
	}

	return offset + 2 + (sizeof(bool)*3) + (sizeof(RTC)*2) + m_ramSize*m_ramCap;
}

uint32_t MBC3::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);
		file.read(reinterpret_cast<char*>(&m_RomBank), 1);
		file.read(reinterpret_cast<char*>(&m_RamBank), 1);
		file.read(reinterpret_cast<char*>(&m_enableRAM), sizeof(bool));

		//MBC3 data
		file.read(reinterpret_cast<char*>(&m_readRTC), sizeof(bool));
		file.read(reinterpret_cast<char*>(&m_lastLatch), sizeof(bool));
		file.read(reinterpret_cast<char*>(&m_latchReg), sizeof(RTC));
		file.read(reinterpret_cast<char*>(&m_timerReg), sizeof(RTC));

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

	return offset + 2 + (sizeof(bool)*3) + (sizeof(RTC)*2) + m_ramSize*m_ramCap;
}

void MBC3::swapRomBank(const uint8_t bank)
{
	m_RomBank = (bank & 0x7F) % m_RomBank;
	if (!m_RomBank)
		m_RomBank = 1;
}

void MBC3::swapRamBank(const uint8_t bank)
{
	m_RamBank = (bank % m_RamBank);

	if (m_RTC && (bank >= 0x8 || bank <= 0xC))
		m_readRTC = true;
}
