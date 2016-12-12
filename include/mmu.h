#pragma once
#include <stdint.h>
// #include <string>

#include <MBC/baseMBC.h>
#include <MBC/MBC1.h>
#include <MBC/MBC3.h>

#define LITTLEENDIAN 1

class MMU
{
	friend class linkboy;
	friend class PPU;
	friend class gbz80;
	friend class timer;

	public:
		MMU(const char* filename = nullptr);
		~MMU();

		void loadGame(const char* filename);

		uint8_t rdByteMMU(const uint16_t address);
		void wrByteMMU(const uint16_t address, const uint8_t data);

		uint16_t rdWordMMU(uint16_t address);
		void wrWordMMU(uint16_t address, const uint16_t data);

		void checkJoyPadPress();

		uint32_t saveToFile(const char* filename, const uint32_t offset);
		uint32_t loadFromFile(const char* filename, const uint32_t offset);

	private:
		void startDMA(const uint16_t address);

		bool inBIOS;

		uint16_t JoyPad = 0xFF;

		uint8_t cartROM[0x4000];		//0x0000 - 16KB
		MBC* mbc = nullptr;				//0x4000 - 16KB
		uint8_t VRAM[0x2000];			//0x8000 - 8KB
		//uint8_t ExtRAM[0x2000];		//0xA000 - 8KB
		uint8_t internalRAM[0x2000];	//0xC000,
										//Shadow 0xE000 - 0xFDFF
		uint8_t spriteRAM[0xA0];		//0xFE00 - 0xFE9F
		uint8_t mmIO[0x100];			//0xFF00

		static const uint8_t bios[0x100];
};

enum REGNAME {
	CONTROLLER_REG = 0,
	SERIAL_DATA,
	SERIAL_CONTROL,
	DIVIDER = 0x4,
	TIMA_COUNT,
	TIMA_RELOAD,
	TIMA_CONTROL,
	IF = 0xF,
	SM1_SWEEP,
	SM1_LENGTH,
	SM1_FREQ_LO,
	SM1_FREQ_HI,
	SM2_LENGTH = 0x16,
	SM2_ENVELOP,
	SM2_FREQ_LO,
	SM2_FREQ_HI,
	SM3_CONTROL,
	SM3_LENGTH,
	SM3_SELECT,
	SM3_FREQ_LO,
	SM3_FREQ_HI,
	SM4_LENGTH = 0x20,
	SM4_ENVELOP,
	SM4_POLY,
	SM4_COUNTER,
	S_CHANNEL_CONTROL,
	S_SELECT,
	S_CONTROL,
	S_WAVE_RAM = 0x30,
	LCDC_CONTROL = 0x40,
	LCDC_STAT,
	SCY,
	SCX,
	LY,
	LYC,
	DMA,
	BGP,
	OBP0,
	OBP1,
	WY,
	WX,
	IE = 0xFF
};
