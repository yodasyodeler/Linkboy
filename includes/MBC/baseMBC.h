#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <iostream>

using std::ios;

class MBC{
	public:
		MBC(const char* filename, const uint8_t* bank0);
		~MBC();

		virtual void updateMBC(const uint16_t address, const uint8_t data);

		virtual uint8_t readSwRom(const uint16_t address);
		virtual uint8_t readExtRam(const uint16_t address);

		virtual void writeExtRam(const uint16_t address, const uint8_t data);

		virtual uint32_t saveToFile(const char* filename, const uint32_t offset);
		virtual uint32_t loadFromFile(const char* filename, const uint32_t offset);
		
		void saveRAM();
	protected:
		char*	 m_savefile		=	nullptr;

		uint8_t  m_RomBank		=	0;
		uint8_t  m_RamBank		=	0;

		uint32_t m_romSize		=	2;
		uint32_t m_ramSize		=	0;

		uint32_t m_ramCap		=	0x2000;


		uint8_t* ExtRAM			=	nullptr;
		//uint8_t  swRom[0x4000]; 
		uint8_t*  swRom			=	nullptr;

		bool	m_saveBattery	=	false;
		bool	m_enableRAM		=	false;
};
