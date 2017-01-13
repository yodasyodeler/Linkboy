#include "mmu.h"

using std::ios;

const uint8_t MMU::bios[0x100] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E, 
	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0, 
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B, 
	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9, 
	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20, 
	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04, 
	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2, 
	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06, 
	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20, 
	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17, 
	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C, 
	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20, 
	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};


MMU::MMU(const char* filename)
	:inBIOS(true)
{
	mmIO[CONTROLLER_REG] = 0xCF;
	if (filename != nullptr)
		loadGame(filename);
}

MMU::~MMU()
{
	delete mbc;
}

void MMU::loadGame(const char * filename)
{
	//Select BIOS & Attempt Opening ROM
	inBIOS = true;
	mmIO[SCX] = 0;
	mmIO[SCY] = 0;
	delete mbc;
	mbc = nullptr;
	if (filename != nullptr) {
		std::ifstream myfile(filename, ios::in | ios::binary);

		if (myfile.is_open()) {
			myfile.read(reinterpret_cast<char*>(cartROM), 0x4000);
			myfile.close();
		}
		else {
			std::cerr << "Cannot find file: " << filename << "\n";
		}

		//Create MBC for ROM
		switch (cartROM[0x147]) {
			case 0x0:
				mbc = new MBC(filename, cartROM);
				break;
			case 0x1:
			case 0x2:
			case 0x3:
				mbc = new MBC1(filename, cartROM);
				break;
			//case 0x05:
			//case 0x06:
			//	mbc = new MBC2(filename, cartROM);
			//	break;
			case 0x08:
			case 0x09:
				mbc = new MBC(filename, cartROM);
			//case 0x0B:
			//case 0x0C:
			//case 0x0D:
			//	mbc = new MMM01(filename, cartRom);
			//	break;
			case 0x0F:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
				mbc = new MBC3(filename, cartROM);
				break;
			//case 0x19:
			//case 0x1A:
			//case 0x1B:
			//case 0x1C:
			//case 0x1D:
			//case 0x1E:
			//	mbc = new MBC5(filename, cartROM);
				//break;
			//case 0x20:
			//	mbc = new MBC6(filename, cartROM);
			//	break;
			//case 0x22:
			//	mbc = new MBC7(filename, cartROM);
			//	break;
			//case 0xFC:
			//	mbc = new POCKETCAMERA(filename, cartROM);
			//	break;
			//case 0xFD:
			//	mbc = new TAMA5(filename, cartROM);
			//	break;
			//case 0xFE:
			//	mbc = new HuC3(filename, cartROM);
			//	break;
			//case 0xFF:
			//	mbc = new HuC1(filename, cartROM);
			//	break;
			default:
				std::cerr << "MBC not supported:" << cartROM[0x147] << "\n";
				break;
		}
	}
}

uint8_t MMU::rdByteMMU(uint16_t address)
{
	uint8_t re = 0;

	switch (address & 0xF000){
		case 0x0000:
			if (inBIOS) {
				if (address < 0x100)
					re = bios[address];
				else if (mbc != nullptr)
					re = cartROM[address];
				else
					re = 0xFF;
			}
			else
				re = cartROM[address];
			break;
		case 0x1000:
		case 0x2000:
		case 0x3000:
			re = cartROM[address];
			break;
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			re = mbc->readSwRom(address);
			break;
		case 0x8000:
		case 0x9000:
			re = VRAM[address & 0x1FFF];
			break;
		case 0xA000:
		case 0xB000:
			re = mbc->readExtRam(address);		//Cartridge RAM
			break;
		case 0xC000:
		case 0xD000:
		
			re = internalRAM[address & 0x1FFF];
			break;
		case 0xE000:
			break;
		case 0xF000:
			switch ((address & 0x0F00)) {
				case 0x000: case 0x100: case 0x200: case 0x300:
				case 0x400: case 0x500: case 0x600: case 0x700:
				case 0x800: case 0x900: case 0xA00: case 0xB00:
				case 0xC00: case 0xD00:
					re = internalRAM[address & 0x1FFF];
					break;
				case 0xE00:
					if (address < 0xFEA0)
						re = spriteRAM[address & 0x7F];
					break;
				case 0xF00:
					address = address &0x00FF;
					switch (address) {
						case CONTROLLER_REG:
							if (!(mmIO[CONTROLLER_REG] & 0x20)) {
								mmIO[CONTROLLER_REG] = (mmIO[CONTROLLER_REG] & 0xF0) | ((JoyPad >> 4) & 0xF);
							}
							else if (!(mmIO[CONTROLLER_REG] & 0x10)) {
								mmIO[CONTROLLER_REG] = (mmIO[CONTROLLER_REG] & 0xF0) | (JoyPad & 0xF);
							}
							re = mmIO[address];
							break;
						case SM1_SWEEP:
							re = mmIO[address] | 0x80;
							break;
						case SM1_LENGTH:
						case SM2_LENGTH:
							re = mmIO[address] | 0x3F;
							break;
						case SM3_CONTROL:
							re = mmIO[address] | 0x7F;
							break;
						case SM1_ENVELOPE:
						case SM2_ENVELOPE:
						case SM4_ENVELOPE:
						case SM4_POLY:
						case S_CHANNEL_CONTROL:
						case S_SELECT:
							re = mmIO[address];
							break;
						case SM3_SELECT:
							re = mmIO[address] | 0x9F;
							break;
						case SM1_FREQ_LO:
						case 0x15:
						case SM2_FREQ_LO:
						case SM3_LENGTH:
						case SM3_FREQ_LO:
						case 0x1F:
						case SM4_LENGTH:
							re = 0xFF;
							break;
						case SM1_FREQ_HI:
						case SM2_FREQ_HI:
						case SM3_FREQ_HI:
						case SM4_COUNTER:
							re = mmIO[address] | 0xBF;
							break;
						case S_CONTROL:
							re = mmIO[address] | 0x70;
							break;
						case 0x27:
						case 0x28:
						case 0x29:
						case 0x2A:
						case 0x2B:
						case 0x2C:
						case 0x2D:
						case 0x2E:
						case 0x2F:
							re = 0xFF;
							break;
						default:
							re = mmIO[address];
							break;
					}
		}
	}
	return re;
}

void MMU::wrByteMMU(uint16_t address, const uint8_t data)
{
	switch (address & 0xF000) {
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			mbc->updateMBC(address, data);
			break;
		case 0x8000:
		case 0x9000:
			VRAM[address & 0x1FFF] = data;
			break;
		case 0xA000:
		case 0xB000:
			mbc->writeExtRam(address, data);	//Cart RAM
			break;
		case 0xC000:
		case 0xD000:
		case 0xE000:
			internalRAM[address & 0x1FFF] = data;
			break;
		case 0xF000:
			switch ((address & 0x0F00)) {
				case 0x000: case 0x100: case 0x200: case 0x300:
				case 0x400: case 0x500: case 0x600: case 0x700:
				case 0x800: case 0x900: case 0xA00: case 0xB00:
				case 0xC00: case 0xD00:
					internalRAM[address & 0x1FFF] = data;
					break;
				case 0xE00:
					if (address < 0xFEA0)
						spriteRAM[address & 0x7F] = data;
					break;
				case 0xF00:
					address = address & 0xFF;
					switch (address) {
						case CONTROLLER_REG:
							mmIO[address] = (data & 0x30) | (mmIO[address]&0xF);
							break;
						case SERIAL_DATA:
							mmIO[address] = data;
							break;
						case SERIAL_CONTROL:
							mmIO[address] = data & 0x81;
							break;
						case DIVIDER:
							mmIO[address] = 0;
							break;
						case TIMA_COUNT:
						case TIMA_RELOAD:
							mmIO[address] = data;
							break;
						case TIMA_CONTROL:
							mmIO[address] = data & 0x7;
							break;
						case IF:
							mmIO[address] = data & 0x1F;
							break;
						case SM1_SWEEP:
							mmIO[address] = data & 0x7F;
							break;
						case SM1_LENGTH:
						case SM1_FREQ_LO:
							mmIO[address] = data;
							break;
						case SM1_FREQ_HI:
							mmIO[address] = data & 0xC7;
							break;
						case SM2_LENGTH:
						case SM2_ENVELOPE:
						case SM2_FREQ_LO:
							mmIO[address] = data;
							break;
						case SM2_FREQ_HI:
							mmIO[address] = data & 0xC7;
							break;
						case SM3_CONTROL:
							mmIO[address] = data & 0x80;
							break;
						case SM3_LENGTH:
						case SM3_SELECT:
							mmIO[address] = data & 0x60;
							break;
						case SM3_FREQ_LO:
							mmIO[address] = data;
							break;
						case SM3_FREQ_HI:
							mmIO[address] = data & 0xC7;
							break;
						case SM4_LENGTH:
							mmIO[address] = data & 0x3F;
							break;
						case SM4_ENVELOPE:
						case SM4_POLY:
							mmIO[address] = data;
							break;
						case SM4_COUNTER:
							mmIO[address] = data & 0xC0;
							break;
						case S_CHANNEL_CONTROL:
						case S_SELECT:
							mmIO[address] = data;
							break;
						case S_CONTROL:
							mmIO[address] = (data & 0x80) | (mmIO[address] & 0x7F);
							break;
						case LCDC_CONTROL:
							mmIO[address] = data;
							break;
						case LCDC_STAT:
							mmIO[address] = data & 0x7F;
							break;
						case SCY:
						case SCX:
							mmIO[address] = data;
							break;
						case LY:
							mmIO[address] = 0;
							break;
						case LYC:
							mmIO[address] = data;
							break;
						case DMA:
							mmIO[address] = data;
							startDMA(data << 8);
							break;
						case BGP:
						case OBP0:
						case OBP1:
						case WY:
						case WX:
							mmIO[address] = data;
							break;
						case 80:
							inBIOS = false;
							mmIO[address] = data;
							break;
						case IE:
							mmIO[address] = (data & 0x1F);
							break;
						default:
							mmIO[address] = data;
							break;
					}
			}
		break;
	}
}

uint16_t MMU::rdWordMMU(const uint16_t address)
{
#ifdef LITTLEENDIAN
	return (rdByteMMU(address) | (rdByteMMU(address + 1)<<8));
#else
	return (rdByteMMU(address)<<8) | rdByteMMU(address + 1);
#endif
}

void MMU::wrWordMMU(const uint16_t address, const uint16_t data)
{
#ifdef LITTLEENDIAN
	wrByteMMU(address, (uint8_t)data);
	wrByteMMU(address + 1, (uint8_t)(data>>8));
#else
	wrByteMMU(address, (uint8_t)(data>>8));
	wrByteMMU(address + 1, (uint8_t)data);
#endif
}

void MMU::checkJoyPadPress()
{
	if (JoyPad & 0x100)
		mmIO[IF] |= 0x10;

	JoyPad &= 0xFF;
}

uint32_t MMU::saveToFile(const char * filename, const uint32_t offset)
{
	uint32_t re = mbc->saveToFile(filename, offset);

	std::ofstream file(filename, ios::app | ios::binary);
	if (file.is_open()) {
		file.seekp(re, ios::beg);
	
		file.write(reinterpret_cast<char*>(&inBIOS), sizeof(bool));
		file.write(reinterpret_cast<char*>(&VRAM), 0x2000);
		file.write(reinterpret_cast<char*>(&internalRAM), 0x2000);
		file.write(reinterpret_cast<char*>(&spriteRAM), 0xA0);
		file.write(reinterpret_cast<char*>(&mmIO), 0x100);

		file.close();
	}

	return re + sizeof(bool) + 0x2000 + 0x2000 + 0xA0 + 0x100;
}

uint32_t MMU::loadFromFile(const char * filename, const uint32_t offset)
{
	uint32_t re = mbc->loadFromFile(filename, offset);

	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(re, ios::beg);

		file.read(reinterpret_cast<char*>(&inBIOS), sizeof(bool));
		file.read(reinterpret_cast<char*>(&VRAM), 0x2000);
		file.read(reinterpret_cast<char*>(&internalRAM), 0x2000);
		file.read(reinterpret_cast<char*>(&spriteRAM), 0xA0);
		file.read(reinterpret_cast<char*>(&mmIO), 0x100);

		file.close();
	}

	return re + sizeof(bool) + 0x2000 + 0x2000 + 0xA0 + 0x100;
}

void MMU::startDMA(const uint16_t address)
{
	for (int i = 0; i < 0xA0; ++i)
		spriteRAM[i] = rdByteMMU(address + i);
}