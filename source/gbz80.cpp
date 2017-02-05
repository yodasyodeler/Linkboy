#include "gbz80.h"

static int opcodeMachineCycles[256] = {
	1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,	//0x00
	0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,	//0x10 
	2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,	//0x20
	2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,	//0x30
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0x40
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0x50
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0x60
	2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,	//0x70
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0x80
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0x90
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0xA0
	1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,	//0xB0
	2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,	//0xC0
	2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,	//0xD0
	3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,	//0xE0
	3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4		//0xF0
};
static int extendedOpcodeMachineCycles[256] = {
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x00
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x10 
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x20
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x30
	2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,	//0x40
	2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,	//0x50
	2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,	//0x60
	2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,	//0x70
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x80
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0x90
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0xA0
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0xB0
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0xC0
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0xD0
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,	//0xE0
	2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2		//0xF0
};

/* system functions */
gbz80::gbz80(MMU* memory, BaseLogger* log)
	:_memory(memory), _log(log)
{
	if ( _log == nullptr )
		_log = BaseLogger::getInstance();
}

void gbz80::loadGame()
{
	_reg = {};
	_reg.sp = 0x7FFE;
	_enableInterrupt		=		false;

	_disableNext			=		false;
	_enableNext			=		false;

	_halt				=		false;
	_stop				=		false;
}

int gbz80::advanceCPU()
{
	regset temp = _reg;
	uint8_t index;

	_opcodeIndex = _memory->rdByteMMU(_reg.pc++);
	_opcodeCycle = opcodeMachineCycles[_opcodeIndex];

	index = _opcodeIndex;

	if (_enableNext) {
		_enableNext			=	false;
		_enableInterrupt	=	true;
	}
	if (_disableNext) {
		_disableNext		=	false;
		_enableInterrupt	=	false;
	}
	//DI HALT Bug
	//if ( _halt && !_enableInterrupt ) {
	//	--_reg.pc;
	//	_halt = false;
	//}

	try {
		switch (_opcodeIndex) {
			case 0x00: break;

			case 0x01: ldReg16(_reg.bc, _memory->rdWordMMU(_reg.pc)); _reg.pc += 2; break;
			case 0x11: ldReg16(_reg.de, _memory->rdWordMMU(_reg.pc)); _reg.pc += 2; break;
			case 0x21: ldReg16(_reg.hl, _memory->rdWordMMU(_reg.pc)); _reg.pc += 2; break;
			case 0x31: ldReg16(_reg.sp, _memory->rdWordMMU(_reg.pc)); _reg.pc += 2; break;
		
			case 0x02: wrMem(_reg.bc, _reg.a); break;
			case 0x12: wrMem(_reg.de, _reg.a); break;

			case 0x03: incReg16(_reg.bc); break;
			case 0x13: incReg16(_reg.de); break;
			case 0x23: incReg16(_reg.hl); break;
			case 0x33: incReg16(_reg.sp); break;

			case 0x0B: decReg16(_reg.bc); break;
			case 0x1B: decReg16(_reg.de); break;
			case 0x2B: decReg16(_reg.hl); break;
			case 0x3B: decReg16(_reg.sp); break;

			case 0x04: incReg8(_reg.b); break;
			case 0x0C: incReg8(_reg.c); break;
			case 0x14: incReg8(_reg.d); break;
			case 0x1C: incReg8(_reg.e); break;
			case 0x24: incReg8(_reg.h); break;
			case 0x2C: incReg8(_reg.l); break;
			case 0x34: incMem8(_reg.hl); break;
			case 0x3C: incReg8(_reg.a); break;

			case 0x05: decReg8(_reg.b); break;
			case 0x0D: decReg8(_reg.c); break;
			case 0x15: decReg8(_reg.d); break;
			case 0x1D: decReg8(_reg.e); break;
			case 0x25: decReg8(_reg.h); break;
			case 0x2D: decReg8(_reg.l); break;
			case 0x35: decMem8(_reg.hl); break;
			case 0x3D: decReg8(_reg.a); break;

			case 0x07: RLCA(_reg.a); break;
			case 0x17: RLA(_reg.a); break;
			case 0x0F: RRCA(_reg.a); break;
			case 0x1F: RRA(_reg.a); break;

			case 0x09: addHL(_reg.bc); break;
			case 0x19: addHL(_reg.de); break;
			case 0x29: addHL(_reg.hl); break;
			case 0x39: addHL(_reg.sp); break;

			case 0xE8: addSP(); break;

			case 0x0A: ldMem(_reg.a, _reg.bc); break;
			case 0x1A: ldMem(_reg.a, _reg.de); break;

			case 0x22: wrMem(_reg.hl++, _reg.a); break;
			case 0x2A: ldMem(_reg.a, _reg.hl++); break;

			case 0x32: wrMem(_reg.hl--, _reg.a); break;
			case 0x3A: ldMem(_reg.a, _reg.hl--); break;

			case 0x27: DAA(); break;
			case 0x2F: CPL(); break;

			case 0x37: SCF(); break;
			case 0x3F: CCF(); break;

			case 0x06: ldMem(_reg.b, _reg.pc++); break;
			case 0x0E: ldMem(_reg.c, _reg.pc++); break;
			case 0x16: ldMem(_reg.d, _reg.pc++); break;
			case 0x1E: ldMem(_reg.e, _reg.pc++); break;
			case 0x26: ldMem(_reg.h, _reg.pc++); break;
			case 0x2E: ldMem(_reg.l, _reg.pc++); break;
			case 0x36: wrMem(_reg.hl, _memory->rdByteMMU(_reg.pc++)); break;
			case 0x3E: ldMem(_reg.a,  _reg.pc++); break;

			case 0x08: _memory->wrWordMMU(_memory->rdWordMMU(_reg.pc), _reg.sp); _reg.pc += 2; break;
		
			case 0x78: ldReg8(_reg.a, _reg.b); break;
			case 0x79: ldReg8(_reg.a, _reg.c); break;
			case 0x7A: ldReg8(_reg.a, _reg.d); break;
			case 0x7B: ldReg8(_reg.a, _reg.e); break;
			case 0x7C: ldReg8(_reg.a, _reg.h); break;
			case 0x7D: ldReg8(_reg.a, _reg.l); break;
			case 0x7E: ldMem(_reg.a, _reg.hl); break;
			case 0x7F: ldReg8(_reg.a, _reg.a); break;

			case 0x40: ldReg8(_reg.b, _reg.b); break;
			case 0x41: ldReg8(_reg.b, _reg.c); break;
			case 0x42: ldReg8(_reg.b, _reg.d); break;
			case 0x43: ldReg8(_reg.b, _reg.e); break;
			case 0x44: ldReg8(_reg.b, _reg.h); break;
			case 0x45: ldReg8(_reg.b, _reg.l); break;
			case 0x46: ldMem(_reg.b, _reg.hl); break;
			case 0x47: ldReg8(_reg.b, _reg.a); break;

			case 0x48: ldReg8(_reg.c, _reg.b); break;
			case 0x49: ldReg8(_reg.c, _reg.c); break;
			case 0x4A: ldReg8(_reg.c, _reg.d); break;
			case 0x4B: ldReg8(_reg.c, _reg.e); break;
			case 0x4C: ldReg8(_reg.c, _reg.h); break;
			case 0x4D: ldReg8(_reg.c, _reg.l); break;
			case 0x4E: ldMem(_reg.c, _reg.hl); break;
			case 0x4F: ldReg8(_reg.c, _reg.a); break;

			case 0x50: ldReg8(_reg.d, _reg.b); break;
			case 0x51: ldReg8(_reg.d, _reg.c); break;
			case 0x52: ldReg8(_reg.d, _reg.d); break;
			case 0x53: ldReg8(_reg.d, _reg.e); break;
			case 0x54: ldReg8(_reg.d, _reg.h); break;
			case 0x55: ldReg8(_reg.d, _reg.l); break;
			case 0x56: ldMem(_reg.d, _reg.hl); break;
			case 0x57: ldReg8(_reg.d, _reg.a); break;

			case 0x58: ldReg8(_reg.e, _reg.b); break;
			case 0x59: ldReg8(_reg.e, _reg.c); break;
			case 0x5A: ldReg8(_reg.e, _reg.d); break;
			case 0x5B: ldReg8(_reg.e, _reg.e); break;
			case 0x5C: ldReg8(_reg.e, _reg.h); break;
			case 0x5D: ldReg8(_reg.e, _reg.l); break;
			case 0x5E: ldMem(_reg.e, _reg.hl); break;
			case 0x5F: ldReg8(_reg.e, _reg.a); break;

			case 0x60: ldReg8(_reg.h, _reg.b); break;
			case 0x61: ldReg8(_reg.h, _reg.c); break;
			case 0x62: ldReg8(_reg.h, _reg.d); break;
			case 0x63: ldReg8(_reg.h, _reg.e); break;
			case 0x64: ldReg8(_reg.h, _reg.h); break;
			case 0x65: ldReg8(_reg.h, _reg.l); break;
			case 0x66: ldMem(_reg.h, _reg.hl); break;
			case 0x67: ldReg8(_reg.h, _reg.a); break;
			   
			case 0x68: ldReg8(_reg.l, _reg.b); break;
			case 0x69: ldReg8(_reg.l, _reg.c); break;
			case 0x6A: ldReg8(_reg.l, _reg.d); break;
			case 0x6B: ldReg8(_reg.l, _reg.e); break;
			case 0x6C: ldReg8(_reg.l, _reg.h); break;
			case 0x6D: ldReg8(_reg.l, _reg.l); break;
			case 0x6E: ldMem(_reg.l, _reg.hl); break;
			case 0x6F: ldReg8(_reg.l, _reg.a); break;

			case 0x70: wrMem(_reg.hl, _reg.b); break;
			case 0x71: wrMem(_reg.hl, _reg.c); break;
			case 0x72: wrMem(_reg.hl, _reg.d); break;
			case 0x73: wrMem(_reg.hl, _reg.e); break;
			case 0x74: wrMem(_reg.hl, _reg.h); break;
			case 0x75: wrMem(_reg.hl, _reg.l); break;
			case 0x77: wrMem(_reg.hl, _reg.a); break;
		
			case 0x80: addReg8(_reg.b); break;
			case 0x81: addReg8(_reg.c); break;
			case 0x82: addReg8(_reg.d); break;
			case 0x83: addReg8(_reg.e); break;
			case 0x84: addReg8(_reg.h); break;
			case 0x85: addReg8(_reg.l); break;
			case 0x86: addReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0x87: addReg8(_reg.a); break;

			case 0xC6: addReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0x88: addCReg8(_reg.b); break;
			case 0x89: addCReg8(_reg.c); break;
			case 0x8A: addCReg8(_reg.d); break;
			case 0x8B: addCReg8(_reg.e); break;
			case 0x8C: addCReg8(_reg.h); break;
			case 0x8D: addCReg8(_reg.l); break;
			case 0x8E: addCReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0x8F: addCReg8(_reg.a); break;
			case 0xCE: addCReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0x90: subReg8(_reg.b); break;
			case 0x91: subReg8(_reg.c); break;
			case 0x92: subReg8(_reg.d); break;
			case 0x93: subReg8(_reg.e); break;
			case 0x94: subReg8(_reg.h); break;
			case 0x95: subReg8(_reg.l); break;
			case 0x96: subReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0x97: subReg8(_reg.a); break;

			case 0xD6: subReg8(_memory->rdByteMMU(_reg.pc++)); break;
				   
			case 0x98: subCReg8(_reg.b); break;
			case 0x99: subCReg8(_reg.c); break;
			case 0x9A: subCReg8(_reg.d); break;
			case 0x9B: subCReg8(_reg.e); break;
			case 0x9C: subCReg8(_reg.h); break;
			case 0x9D: subCReg8(_reg.l); break;
			case 0x9E: subCReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0x9F: subCReg8(_reg.a); break;
			case 0xDE: subCReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0xA0: andReg8(_reg.b); break;
			case 0xA1: andReg8(_reg.c); break;
			case 0xA2: andReg8(_reg.d); break;
			case 0xA3: andReg8(_reg.e); break;
			case 0xA4: andReg8(_reg.h); break;
			case 0xA5: andReg8(_reg.l); break;
			case 0xA6: andReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0xA7: andReg8(_reg.a); break;
			case 0xE6: andReg8(_memory->rdByteMMU(_reg.pc++)); break;
			   
			case 0xA8: xorReg8(_reg.b); break;
			case 0xA9: xorReg8(_reg.c); break;
			case 0xAA: xorReg8(_reg.d); break;
			case 0xAB: xorReg8(_reg.e); break;
			case 0xAC: xorReg8(_reg.h); break;
			case 0xAD: xorReg8(_reg.l); break;
			case 0xAE: xorReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0xAF: xorReg8(_reg.a); break;
			case 0xEE: xorReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0xB0: orReg8(_reg.b); break;
			case 0xB1: orReg8(_reg.c); break;
			case 0xB2: orReg8(_reg.d); break;
			case 0xB3: orReg8(_reg.e); break;
			case 0xB4: orReg8(_reg.h); break;
			case 0xB5: orReg8(_reg.l); break;
			case 0xB6: orReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0xB7: orReg8(_reg.a); break;
			case 0xF6: orReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0xB8: cpReg8(_reg.b); break;
			case 0xB9: cpReg8(_reg.c); break;
			case 0xBA: cpReg8(_reg.d); break;
			case 0xBB: cpReg8(_reg.e); break;
			case 0xBC: cpReg8(_reg.h); break;
			case 0xBD: cpReg8(_reg.l); break;
			case 0xBE: cpReg8(_memory->rdByteMMU(_reg.hl)); break;
			case 0xBF: cpReg8(_reg.a); break;
			case 0xFE: cpReg8(_memory->rdByteMMU(_reg.pc++)); break;

			case 0xC3: JP(); break;
			case 0xC2: JPC(NZero); break;
			case 0xCA: JPC(Zero); break;
			case 0xD2: JPC(NCarry); break;
			case 0xDA: JPC(Carry); break;

			case 0xE9: JPHL(); break;

			case 0x18: JR(); break;
			case 0x20: JRC(NZero); break;
			case 0x28: JRC(Zero); break;
			case 0x30: JRC(NCarry); break;
			case 0x38: JRC(Carry); break;
 
			case 0xC5: push(_reg.bc); break;
			case 0xD5: push(_reg.de); break;
			case 0xE5: push(_reg.hl); break;
			case 0xF5: push(_reg.af); break;

			case 0xC1: pop(_reg.bc); break;
			case 0xD1: pop(_reg.de); break;
			case 0xE1: pop(_reg.hl); break;
			case 0xF1: pop(_reg.af); _reg.f &= 0xF0; break;
		
			case 0xC7: RST(0x00); break;
			case 0xCF: RST(0x08); break;
			case 0xD7: RST(0x10); break;
			case 0xDF: RST(0x18); break;
			case 0xE7: RST(0x20); break;
			case 0xEF: RST(0x28); break;
			case 0xF7: RST(0x30); break;
			case 0xFF: RST(0x38); break;

			case 0xC9: RET(); break;
			case 0xC0: RETC(NZero); break;
			case 0xC8: RETC(Zero); break;
			case 0xD0: RETC(NCarry); break;
			case 0xD8: RETC(Carry); break;
			case 0xD9: RETI(); break;

			case 0xCD: CALL(); break;
			case 0xC4: CALLC(NZero); break;
			case 0xCC: CALLC(Zero); break;
			case 0xD4: CALLC(NCarry); break;
			case 0xDC: CALLC(Carry); break;

			case 0xE0: wrMem((_memory->rdByteMMU(_reg.pc++)|0xFF00), _reg.a); break;
			case 0xF0: ldMem(_reg.a, (_memory->rdByteMMU(_reg.pc++) | 0xFF00)); break;

			case 0xE2: wrMem((_reg.c|0xFF00), _reg.a); break;
			case 0xF2: ldMem(_reg.a, (_reg.c|0xFF00)); break;
			case 0xF8: ldHL(); break;
			case 0xF9: ldReg16(_reg.sp, _reg.hl); break;

			case 0xF3: DI(); break;
			case 0xFB: EI(); break;

			case 0xEA: wrMem(_memory->rdWordMMU(_reg.pc), _reg.a); _reg.pc += 2; break;
			case 0xFA: ldMem(_reg.a, _memory->rdWordMMU(_reg.pc)); _reg.pc += 2; break;

			case 0xCB: _opcodeCycle = extendedOP(); break;

			case 0x10: STOP(); break;
			case 0x76: HALT(); break;

			default: throw (uint32_t) _opcodeIndex;
		}
	}
	catch (const uint32_t e) {
		std::cerr << "Instruction not implemented, Opcode:"  << std::hex << e << std::endl;
		exit(-1);
	}

	//Log Info
	if (index != 0x00) {
		debug de = { temp, _reg, index, _opcodeCycle };
		_log->log(de);
	}

	return _opcodeCycle;

}

/* CB Extension Command */
int gbz80::extendedOP()
{
	uint8_t temp;
	_opcodeIndex = _memory->rdByteMMU(_reg.pc++);

	try {
		switch (_opcodeIndex) {
			case 0x00: RLC(_reg.b); break;
			case 0x01: RLC(_reg.c); break;
			case 0x02: RLC(_reg.d); break;
			case 0x03: RLC(_reg.e); break;
			case 0x04: RLC(_reg.h); break;
			case 0x05: RLC(_reg.l); break;
			case 0x06: temp = _memory->rdByteMMU(_reg.hl); RLC(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x07: RLC(_reg.a); break;

			case 0x08: RRC(_reg.b); break;
			case 0x09: RRC(_reg.c); break;
			case 0x0A: RRC(_reg.d); break;
			case 0x0B: RRC(_reg.e); break;
			case 0x0C: RRC(_reg.h); break;
			case 0x0D: RRC(_reg.l); break;
			case 0x0E: temp = _memory->rdByteMMU(_reg.hl); RRC(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x0F: RRC(_reg.a); break;

			case 0x10: RL(_reg.b); break;
			case 0x11: RL(_reg.c); break;
			case 0x12: RL(_reg.d); break;
			case 0x13: RL(_reg.e); break;
			case 0x14: RL(_reg.h); break;
			case 0x15: RL(_reg.l); break;
			case 0x16: temp = _memory->rdByteMMU(_reg.hl); RL(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x17: RL(_reg.a); break;

			case 0x18: RR(_reg.b); break;
			case 0x19: RR(_reg.c); break;
			case 0x1A: RR(_reg.d); break;
			case 0x1B: RR(_reg.e); break;
			case 0x1C: RR(_reg.h); break;
			case 0x1D: RR(_reg.l); break;
			case 0x1E: temp = _memory->rdByteMMU(_reg.hl); RR(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x1F: RR(_reg.a); break;

			case 0x20: SL(_reg.b); break;
			case 0x21: SL(_reg.c); break;
			case 0x22: SL(_reg.d); break;
			case 0x23: SL(_reg.e); break;
			case 0x24: SL(_reg.h); break;
			case 0x25: SL(_reg.l); break;
			case 0x26: temp = _memory->rdByteMMU(_reg.hl); SL(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x27: SL(_reg.a); break;
				   
			case 0x28: SRA(_reg.b); break;
			case 0x29: SRA(_reg.c); break;
			case 0x2A: SRA(_reg.d); break;
			case 0x2B: SRA(_reg.e); break;
			case 0x2C: SRA(_reg.h); break;
			case 0x2D: SRA(_reg.l); break;
			case 0x2E: temp = _memory->rdByteMMU(_reg.hl); SRA(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x2F: SRA(_reg.a); break;
				
			case 0x30: swapReg8(_reg.b); break;
			case 0x31: swapReg8(_reg.c);	break;
			case 0x32: swapReg8(_reg.d);	break;
			case 0x33: swapReg8(_reg.e);	break;
			case 0x34: swapReg8(_reg.h);	break;
			case 0x35: swapReg8(_reg.l);	break;
			case 0x36: temp = _memory->rdByteMMU(_reg.hl); swapReg8(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x37: swapReg8(_reg.a); break;

			case 0x38: SRL(_reg.b); break;
			case 0x39: SRL(_reg.c); break;
			case 0x3A: SRL(_reg.d); break;
			case 0x3B: SRL(_reg.e); break;
			case 0x3C: SRL(_reg.h); break;
			case 0x3D: SRL(_reg.l); break;
			case 0x3E: temp = _memory->rdByteMMU(_reg.hl); SRL(temp); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x3F: SRL(_reg.a); break;


			case 0x40: BIT(_reg.b, 0); break;
			case 0x41: BIT(_reg.c, 0); break;
			case 0x42: BIT(_reg.d, 0); break;
			case 0x43: BIT(_reg.e, 0); break;
			case 0x44: BIT(_reg.h, 0); break;
			case 0x45: BIT(_reg.l, 0); break;
			case 0x46: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 0);  break;
			case 0x47: BIT(_reg.a, 0); break;
				   
			case 0x48: BIT(_reg.b, 1); break;
			case 0x49: BIT(_reg.c, 1); break;
			case 0x4A: BIT(_reg.d, 1); break;
			case 0x4B: BIT(_reg.e, 1); break;
			case 0x4C: BIT(_reg.h, 1); break;
			case 0x4D: BIT(_reg.l, 1); break;
			case 0x4E: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 1); break;
			case 0x4F: BIT(_reg.a, 1); break;

			case 0x50: BIT(_reg.b, 2); break;
			case 0x51: BIT(_reg.c, 2); break;
			case 0x52: BIT(_reg.d, 2); break;
			case 0x53: BIT(_reg.e, 2); break;
			case 0x54: BIT(_reg.h, 2); break;
			case 0x55: BIT(_reg.l, 2); break;
			case 0x56: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 2);  break;
			case 0x57: BIT(_reg.a, 2); break;
				   
			case 0x58: BIT(_reg.b, 3); break;
			case 0x59: BIT(_reg.c, 3); break;
			case 0x5A: BIT(_reg.d, 3); break;
			case 0x5B: BIT(_reg.e, 3); break;
			case 0x5C: BIT(_reg.h, 3); break;
			case 0x5D: BIT(_reg.l, 3); break;
			case 0x5E: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 3); break;
			case 0x5F: BIT(_reg.a, 3); break;

			case 0x60: BIT(_reg.b, 4); break;
			case 0x61: BIT(_reg.c, 4); break;
			case 0x62: BIT(_reg.d, 4); break;
			case 0x63: BIT(_reg.e, 4); break;
			case 0x64: BIT(_reg.h, 4); break;
			case 0x65: BIT(_reg.l, 4); break;
			case 0x66: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 4);  break;
			case 0x67: BIT(_reg.a, 4); break;
				  
			case 0x68: BIT(_reg.b, 5); break;
			case 0x69: BIT(_reg.c, 5); break;
			case 0x6A: BIT(_reg.d, 5); break;
			case 0x6B: BIT(_reg.e, 5); break;
			case 0x6C: BIT(_reg.h, 5); break;
			case 0x6D: BIT(_reg.l, 5); break;
			case 0x6E: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 5); break;
			case 0x6F: BIT(_reg.a, 5); break;

			case 0x70: BIT(_reg.b, 6); break;
			case 0x71: BIT(_reg.c, 6); break;
			case 0x72: BIT(_reg.d, 6); break;
			case 0x73: BIT(_reg.e, 6); break;
			case 0x74: BIT(_reg.h, 6); break;
			case 0x75: BIT(_reg.l, 6); break;
			case 0x76: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 6);  break;
			case 0x77: BIT(_reg.a, 6); break;
				   
			case 0x78: BIT(_reg.b, 7); break;
			case 0x79: BIT(_reg.c, 7); break;
			case 0x7A: BIT(_reg.d, 7); break;
			case 0x7B: BIT(_reg.e, 7); break;
			case 0x7C: BIT(_reg.h, 7); break;
			case 0x7D: BIT(_reg.l, 7); break;
			case 0x7E: temp = _memory->rdByteMMU(_reg.hl); BIT(temp, 7); break;
			case 0x7F: BIT(_reg.a, 7); break;


			case 0x80: RES(_reg.b, 0); break;
			case 0x81: RES(_reg.c, 0); break;
			case 0x82: RES(_reg.d, 0); break;
			case 0x83: RES(_reg.e, 0); break;
			case 0x84: RES(_reg.h, 0); break;
			case 0x85: RES(_reg.l, 0); break;
			case 0x86: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 0); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x87: RES(_reg.a, 0); break;
				   
			case 0x88: RES(_reg.b, 1); break;
			case 0x89: RES(_reg.c, 1); break;
			case 0x8A: RES(_reg.d, 1); break;
			case 0x8B: RES(_reg.e, 1); break;
			case 0x8C: RES(_reg.h, 1); break;
			case 0x8D: RES(_reg.l, 1); break;
			case 0x8E: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 1); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x8F: RES(_reg.a, 1); break;

			case 0x90: RES(_reg.b, 2); break;
			case 0x91: RES(_reg.c, 2); break;
			case 0x92: RES(_reg.d, 2); break;
			case 0x93: RES(_reg.e, 2); break;
			case 0x94: RES(_reg.h, 2); break;
			case 0x95: RES(_reg.l, 2); break;
			case 0x96: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 2); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x97: RES(_reg.a, 2); break;
				   
			case 0x98: RES(_reg.b, 3); break;
			case 0x99: RES(_reg.c, 3); break;
			case 0x9A: RES(_reg.d, 3); break;
			case 0x9B: RES(_reg.e, 3); break;
			case 0x9C: RES(_reg.h, 3); break;
			case 0x9D: RES(_reg.l, 3); break;
			case 0x9E: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 3); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0x9F: RES(_reg.a, 3); break;

			case 0xA0: RES(_reg.b, 4); break;
			case 0xA1: RES(_reg.c, 4); break;
			case 0xA2: RES(_reg.d, 4); break;
			case 0xA3: RES(_reg.e, 4); break;
			case 0xA4: RES(_reg.h, 4); break;
			case 0xA5: RES(_reg.l, 4); break;
			case 0xA6: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 4); _memory->wrByteMMU(_reg.hl, temp);  break;
			case 0xA7: RES(_reg.a, 4); break;
				   
			case 0xA8: RES(_reg.b, 5); break;
			case 0xA9: RES(_reg.c, 5); break;
			case 0xAA: RES(_reg.d, 5); break;
			case 0xAB: RES(_reg.e, 5); break;
			case 0xAC: RES(_reg.h, 5); break;
			case 0xAD: RES(_reg.l, 5); break;
			case 0xAE: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 5); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xAF: RES(_reg.a, 5); break;

			case 0xB0: RES(_reg.b, 6); break;
			case 0xB1: RES(_reg.c, 6); break;
			case 0xB2: RES(_reg.d, 6); break;
			case 0xB3: RES(_reg.e, 6); break;
			case 0xB4: RES(_reg.h, 6); break;
			case 0xB5: RES(_reg.l, 6); break;
			case 0xB6: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 6); _memory->wrByteMMU(_reg.hl, temp);  break;
			case 0xB7: RES(_reg.a, 6); break;
				   
			case 0xB8: RES(_reg.b, 7); break;
			case 0xB9: RES(_reg.c, 7); break;
			case 0xBA: RES(_reg.d, 7); break;
			case 0xBB: RES(_reg.e, 7); break;
			case 0xBC: RES(_reg.h, 7); break;
			case 0xBD: RES(_reg.l, 7); break;
			case 0xBE: temp = _memory->rdByteMMU(_reg.hl); RES(temp, 7); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xBF: RES(_reg.a, 7); break;


			case 0xC0: SET(_reg.b, 0); break;
			case 0xC1: SET(_reg.c, 0); break;
			case 0xC2: SET(_reg.d, 0); break;
			case 0xC3: SET(_reg.e, 0); break;
			case 0xC4: SET(_reg.h, 0); break;
			case 0xC5: SET(_reg.l, 0); break;
			case 0xC6: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 0); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xC7: SET(_reg.a, 0); break;
				   
			case 0xC8: SET(_reg.b, 1); break;
			case 0xC9: SET(_reg.c, 1); break;
			case 0xCA: SET(_reg.d, 1); break;
			case 0xCB: SET(_reg.e, 1); break;
			case 0xCC: SET(_reg.h, 1); break;
			case 0xCD: SET(_reg.l, 1); break;
			case 0xCE: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 1); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xCF: SET(_reg.a, 1); break;

			case 0xD0: SET(_reg.b, 2); break;
			case 0xD1: SET(_reg.c, 2); break;
			case 0xD2: SET(_reg.d, 2); break;
			case 0xD3: SET(_reg.e, 2); break;
			case 0xD4: SET(_reg.h, 2); break;
			case 0xD5: SET(_reg.l, 2); break;
			case 0xD6: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 2); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xD7: SET(_reg.a, 2); break;
				   
			case 0xD8: SET(_reg.b, 3); break;
			case 0xD9: SET(_reg.c, 3); break;
			case 0xDA: SET(_reg.d, 3); break;
			case 0xDB: SET(_reg.e, 3); break;
			case 0xDC: SET(_reg.h, 3); break;
			case 0xDD: SET(_reg.l, 3); break;
			case 0xDE: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 3); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xDF: SET(_reg.a, 3); break;

			case 0xE0: SET(_reg.b, 4); break;
			case 0xE1: SET(_reg.c, 4); break;
			case 0xE2: SET(_reg.d, 4); break;
			case 0xE3: SET(_reg.e, 4); break;
			case 0xE4: SET(_reg.h, 4); break;
			case 0xE5: SET(_reg.l, 4); break;
			case 0xE6: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 4); _memory->wrByteMMU(_reg.hl, temp);  break;
			case 0xE7: SET(_reg.a, 4); break;
				   
			case 0xE8: SET(_reg.b, 5); break;
			case 0xE9: SET(_reg.c, 5); break;
			case 0xEA: SET(_reg.d, 5); break;
			case 0xEB: SET(_reg.e, 5); break;
			case 0xEC: SET(_reg.h, 5); break;
			case 0xED: SET(_reg.l, 5); break;
			case 0xEE: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 5); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xEF: SET(_reg.a, 5); break;

			case 0xF0: SET(_reg.b, 6); break;
			case 0xF1: SET(_reg.c, 6); break;
			case 0xF2: SET(_reg.d, 6); break;
			case 0xF3: SET(_reg.e, 6); break;
			case 0xF4: SET(_reg.h, 6); break;
			case 0xF5: SET(_reg.l, 6); break;
			case 0xF6: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 6); _memory->wrByteMMU(_reg.hl, temp);  break;
			case 0xF7: SET(_reg.a, 6); break;
				   
			case 0xF8: SET(_reg.b, 7); break;
			case 0xF9: SET(_reg.c, 7); break;
			case 0xFA: SET(_reg.d, 7); break;
			case 0xFB: SET(_reg.e, 7); break;
			case 0xFC: SET(_reg.h, 7); break;
			case 0xFD: SET(_reg.l, 7); break;
			case 0xFE: temp = _memory->rdByteMMU(_reg.hl); SET(temp, 7); _memory->wrByteMMU(_reg.hl, temp); break;
			case 0xFF: SET(_reg.a, 7); break;

			default: throw (uint32_t)_opcodeIndex;
		}
	}
	catch (const uint32_t e) {
		std::cerr << "Extended Instruction not implemented, Opcode: CB " << std::hex << e << std::endl;
		exit(-1);
	}

	return extendedOpcodeMachineCycles[_opcodeIndex];
}

int gbz80::processInterrupt()
{
	int i = 4;
	int re = 0;
	int polledInterrupts;

	polledInterrupts = _memory->mmIO[IF] & _memory->mmIO[IE] & 0x1F;
	if (polledInterrupts) {

		_halt = false;
		if (_enableInterrupt) {
			while (!((polledInterrupts >> i) & 1))
				--i;

			push(_reg.pc);

			switch (i) {
				case 0:		//V-blank
					_reg.pc = 0x40;
					break;
				case 1:		//LCD-STAT
					_reg.pc = 0x48;
					break;
				case 2:		//Timer
					_reg.pc = 0x50;
					break;
				case 3:		//Serial Transfer
					_reg.pc = 0x58;
					break;
				case 4:		//Joypad
					_reg.pc = 0x60;
					break;
			}
			_enableInterrupt = false;
			_memory->mmIO[IF] &= ~(1 << i);
			re = 5;
		}
	}

	return re;
}

uint32_t gbz80::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::app | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);
		
		file.write(reinterpret_cast<char*>(&_enableInterrupt), sizeof(bool));
		file.write(reinterpret_cast<char*>(&_disableNext), sizeof(bool));
		file.write(reinterpret_cast<char*>(&_enableNext), sizeof(bool));
		file.write(reinterpret_cast<char*>(&_halt), sizeof(bool));
		file.write(reinterpret_cast<char*>(&_stop), sizeof(bool));

		file.write(reinterpret_cast<char*>(&_reg), sizeof(regset));

		file.close();
	}

	return offset + sizeof(bool)*5 + sizeof(regset);
}

uint32_t gbz80::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);

		file.read(reinterpret_cast<char*>(&_enableInterrupt), sizeof(bool));
		file.read(reinterpret_cast<char*>(&_disableNext), sizeof(bool));
		file.read(reinterpret_cast<char*>(&_enableNext), sizeof(bool));
		file.read(reinterpret_cast<char*>(&_halt), sizeof(bool));
		file.read(reinterpret_cast<char*>(&_stop), sizeof(bool));

		file.read(reinterpret_cast<char*>(&_reg), sizeof(regset));

		file.close();
	}

	return offset + sizeof(bool)*5 + sizeof(regset);
}

bool gbz80::getHalt()
{
	return _halt;
}

int gbz80::getPC()
{
	return _reg.pc;
}

/* opcodes */
void gbz80::ldHL()
{
	int8_t imm = _memory->rdByteMMU(_reg.pc++);
	uint32_t temp = _reg.sp + imm;

	_reg.f = ( (0x100 & (_reg.sp ^ temp ^ imm))			? CARRYFLAG : 0) 		//Carry Flag
	 |		( (0x10 & (_reg.sp ^ temp ^ imm))			? HALFCARRYFLAG: 0);	//Half Carry Flag

	_reg.hl = temp;
}

void gbz80::ldReg8(uint8_t& dest, const uint8_t src)
{
	dest = src;
}
void gbz80::ldReg16(uint16_t& dest, const uint16_t src)
{
	dest = src;
}

void gbz80::ldMem(uint8_t& dest, const uint16_t src)
{
	dest = _memory->rdByteMMU(src);
}
void gbz80::wrMem(const uint16_t dest, const uint8_t src)
{
	_memory->wrByteMMU(dest, src);
}


/* Push & Pop */
void gbz80::push(const uint16_t src)
{
	_reg.sp -= 2;
	_memory->wrWordMMU(_reg.sp, src);
}
void gbz80::pop(uint16_t& dest)
{
	dest = _memory->rdWordMMU(_reg.sp);
	_reg.sp += 2;
}

/* 8-bit ALU instructions */
void gbz80::addReg8(const uint8_t src)
{
	uint32_t temp = _reg.a + src;

	_reg.f = ( (temp > 0xFF)						? CARRYFLAG : 0)			//Carry
	 |		( (temp & 0xFF)						?  0 : ZEROFLAG)			//Zero
	 |		( (0x10 & (_reg.a ^ src ^ temp) )	? HALFCARRYFLAG : 0);		//Half Carry
	

	_reg.a = temp;
}
void gbz80::addCReg8(const uint8_t src)
{
	uint32_t temp = _reg.a + src + readCarryFlag(_reg.f);

	_reg.f = ( (temp > 0xFF)						? CARRYFLAG : 0)		//Carry
	 |		( (temp & 0xFF)						? 0 : ZEROFLAG)			//Zero
	 |		( (0x10 & (_reg.a ^ src ^ temp) )	? HALFCARRYFLAG : 0);	//Half Carry

	_reg.a = temp;
}

void gbz80::subReg8(const uint8_t src)
{
	int32_t temp = _reg.a - src;

	_reg.f = SUBTRACTFLAG
	 |		( (temp < 0)						? CARRYFLAG : 0)	//Carry
	 |		( (temp & 0xFF)						? 0 : ZEROFLAG)			//Zero
	 |		( (0x10 & (_reg.a ^ src ^ temp))		? HALFCARRYFLAG : 0);	//Half Carry

	_reg.a = temp;
}
void gbz80::subCReg8(const uint8_t src)
{
	int32_t temp = _reg.a - src - readCarryFlag(_reg.f);

	_reg.f = SUBTRACTFLAG
	 |		( (temp < 0)						? CARRYFLAG : 0)		//Carry
	 |		( (temp & 0xFF)						? 0 : ZEROFLAG)			//Zero
	 |		( (0x10 & (_reg.a ^ src ^ temp))		? HALFCARRYFLAG : 0);	//Half Carry

	_reg.a = temp;
}

void gbz80::andReg8(const uint8_t src)
{
	_reg.a &= src;
	_reg.f = _reg.a	? HALFCARRYFLAG : (ZEROFLAG | HALFCARRYFLAG);	//Zero
}
void gbz80::orReg8(const uint8_t src)
{
	_reg.a |= src;
	_reg.f = _reg.a 		? 0 : ZEROFLAG;		//Zero
}
void gbz80::xorReg8(const uint8_t src)
{
	_reg.a ^= src;
	_reg.f = _reg.a		? 0 : ZEROFLAG;		//Zero
}

void gbz80::cpReg8(const uint8_t src)
{
	_reg.f = SUBTRACTFLAG
	 |		( (_reg.a < src)					? CARRYFLAG : 0)		//Carry
	 |		( (_reg.a == src)				? ZEROFLAG : 0)			//Zero
	 |		( (_reg.a & 0xF) < (src & 0xF)	? HALFCARRYFLAG : 0);	//Half Carry
}

void gbz80::incReg8(uint8_t& src)
{
	++src;

	_reg.f = (CARRYFLAG & _reg.f)									//Carry
	 |		(  src						? 0 : ZEROFLAG)			//Zero
	 |		( (src & 0xF)				? 0 : HALFCARRYFLAG);	//Half Carry
}

void gbz80::decReg8(uint8_t& src)
{
	--src;

	_reg.f = (CARRYFLAG & _reg.f)										//Carry
	 |		(  src							? 0 : ZEROFLAG)			//Zero
	 |		( ((src & 0xF) == 0x0F)			? HALFCARRYFLAG : 0)	//Half Carry
	 |		SUBTRACTFLAG;											//Subtract
}

void gbz80::incMem8(const uint16_t src)
{
	uint8_t temp = _memory->rdByteMMU(src) + 1;

	_reg.f = (CARRYFLAG & _reg.f)									//Carry
	 |		(  temp 					? 0 : ZEROFLAG)			//Zero
	 |		( (temp & 0xF) 				? 0 : HALFCARRYFLAG);	//Half Carry

	_memory->wrByteMMU(src, temp);
}
void gbz80::decMem8(const uint16_t src)
{
	uint8_t temp = _memory->rdByteMMU(src) - 1;

	_reg.f = (CARRYFLAG & _reg.f)										//Carry
	 |		(  temp							? 0 : ZEROFLAG)			//Zero
	 |		( ((temp & 0xF) == 0x0F)		? HALFCARRYFLAG : 0)	//Half Carry
	 |		SUBTRACTFLAG;											//Subtract

	_memory->wrByteMMU(src, temp);
}

/* 16-bit ALU instructions */
void gbz80::addHL(const uint16_t src)
{
	uint32_t temp = _reg.hl + src;

	_reg.f = (_reg.f & ZEROFLAG)												//Zero
	 |		( (temp > 0xFFFF)						? CARRYFLAG : 0)		//Carry
	 |		( (0x1000&(_reg.hl ^ src ^ temp))		? HALFCARRYFLAG : 0);	//Half Carry

	_reg.hl = temp;
}
void gbz80::addSP()
{
	int8_t imm = _memory->rdByteMMU(_reg.pc++);
	uint32_t temp = _reg.sp + imm;

	_reg.f = ( (0x100 & (_reg.sp ^ imm ^ temp))		? CARRYFLAG : 0)		//Carry
	 |		( (0x10  & (_reg.sp ^ imm ^ temp))		? HALFCARRYFLAG : 0);	//Half Carry

	_reg.sp = temp;
}

void gbz80::incReg16(uint16_t& src)
{
	++src;
}
void gbz80::decReg16(uint16_t& src)
{
	--src;
}

/* Miscellaneous */
void gbz80::swapReg8(uint8_t& src)
{
	src = (src >> 4) | (src << 4);

	_reg.f = src					? 0 : ZEROFLAG;		//Zero
}

void gbz80::DAA()
{
	uint8_t temp = (_reg.f & (SUBTRACTFLAG | CARRYFLAG));

	if (!readSubtractFlag(_reg.f)) {
		if (readCarryFlag(_reg.f) || _reg.a > 0x99) {
			_reg.a += 0x60;
			temp |= CARRYFLAG;
		}
		if (readHalfCarryFlag(_reg.f) || (_reg.a & 0xF) > 0x9) {
			_reg.a += 0x06;
		}
	}
	else if (readHalfCarryFlag(_reg.f) &&readCarryFlag(_reg.f)) 
		_reg.a += 0x9A;
	else if (readCarryFlag(_reg.f)) 
		_reg.a += 0xA0;
	else if (readHalfCarryFlag(_reg.f)) 
		_reg.a += 0xFA;
	

	_reg.f = (temp | (_reg.a 		? 0 : ZEROFLAG) );		//Zero

}

void gbz80::CPL()
{
	_reg.a = ~_reg.a;
	_reg.f |=	SUBTRACTFLAG | HALFCARRYFLAG;
}

void gbz80::CCF()
{
	_reg.f =  (_reg.f^CARRYFLAG)	&	(ZEROFLAG|CARRYFLAG);
}
void gbz80::SCF()
{
	_reg.f = CARRYFLAG | (_reg.f & ZEROFLAG);
}

//stop operations till an interrupt
void gbz80::HALT()
{
	_halt = true;
}
//stop operations and LCD display until a button press
void gbz80::STOP()
{
	_stop = true;
}

void gbz80::DI()
{
	_disableNext = true;
}
void gbz80::EI()
{
	_enableNext = true;
}

/* rotates & shifts */
void gbz80::RLCA(uint8_t& src)
{
	src = (src >> 7) | (src << 1);

	_reg.f = ( (src & 0x01)		? CARRYFLAG : 0);			//Carry
}
void gbz80::RLA(uint8_t& src)
{
	uint8_t temp = (src << 1) | readCarryFlag(_reg.f);

	_reg.f = ( (src & 0x80) >> 3);				//Carry

	src = temp;
}

void gbz80::RRCA(uint8_t& src)
{
	src = (src << 7) | (src >> 1);

	_reg.f = ( (src & 0x80) >> 3);			//Carry
	
}
void gbz80::RRA(uint8_t& src)
{
	uint8_t temp = (src >> 1) | (readCarryFlag(_reg.f) << 7);

	_reg.f = ( (src & 0x01) << 4);			//Carry

	src = temp;
}

void gbz80::SL(uint8_t& src)
{
	_reg.f = ( (src & 0x80)			? CARRYFLAG : 0)	//Carry
	 |		( ((src<<1) & 0xFF)		? 0 : ZEROFLAG);	//Zero

	src <<= 1;
}
void gbz80::SRA(uint8_t& src)
{
	_reg.f = ( (src & 0x01)			? CARRYFLAG : 0)	//Carry
	 |		( (src >> 1)			? 0 : ZEROFLAG);			//Zero

	src = (src & 0x80) | (src >> 1);
}

void gbz80::SRL(uint8_t& src)
{
	_reg.f = ( (src & 0x01)		? CARRYFLAG : 0)
	 |		( (src>>1)			? 0 : ZEROFLAG);

	src = src >> 1;
}

/* Extended Rotates */
void gbz80::RLC(uint8_t& src)
{
	src = (src >> 7) | (src << 1);

	_reg.f = ( (src & 0x01)		? CARRYFLAG : 0)	//Carry
	 |		(  src				? 0 : ZEROFLAG);	//Zero
}
void gbz80::RL(uint8_t& src)
{
	uint8_t temp = (src << 1) | readCarryFlag(_reg.f);

	_reg.f = ( (src & 0x80) >> 3)				//Carry
	 |		(  temp			? 0 : ZEROFLAG);	//Zero

	src = temp;
}

void gbz80::RRC(uint8_t& src)
{
	src = (src << 7) | (src >> 1);

	_reg.f = ( (src & 0x80) >> 3)					//Carry
	 |		(  src				? 0 : ZEROFLAG);	//Zero
}
void gbz80::RR(uint8_t& src)
{
	uint8_t temp = (src >> 1) | (readCarryFlag(_reg.f) << 7);

	_reg.f = ( (src & 0x01) << 4)				//Carry
	 |		(  temp			? 0 : ZEROFLAG);	//Zero

	src = temp;
}

/*extended bitwise instrucitons */
void gbz80::BIT(const uint8_t src, const uint8_t bit)
{
	_reg.f = (_reg.f & CARRYFLAG)
	 |		HALFCARRYFLAG								//Half
	 |		( (src & (1<<bit))		? 0 : ZEROFLAG);	//Zero
}
void gbz80::SET(uint8_t& src, const uint8_t bit)
{
	src |= (1<<bit);
}
void gbz80::RES(uint8_t& src, const uint8_t bit)
{
	src &= ~(1<<bit);
}

/* Jumps */
void gbz80::JP()
{
	_reg.pc = _memory->rdWordMMU(_reg.pc);
}
void gbz80::JPC(const flags flag)
{
	uint16_t addr = _memory->rdWordMMU(_reg.pc);
	_reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(_reg.f)) {
				_reg.pc = addr;
				_opcodeCycle += 1;
			}
			break;
		case Zero:
			if (readZeroFlag(_reg.f)) {
				_reg.pc = addr;
				_opcodeCycle += 1;
			}
			break;
		case NCarry:
			if (!readCarryFlag(_reg.f)) {
				_reg.pc = addr;
				_opcodeCycle += 1;
			}
			break;
		case Carry:
			if (readCarryFlag(_reg.f)) {
				_reg.pc = addr;
				_opcodeCycle += 1;
			}
			break;
	}
}
void gbz80::JPHL()
{
	_reg.pc = _reg.hl;
}
void gbz80::JR()
{
	_reg.pc += ((int8_t)_memory->rdByteMMU(_reg.pc++));
}
void gbz80::JRC(const flags flag)
{
	int8_t offset = _memory->rdByteMMU(_reg.pc++);

	switch (flag) {
		case NZero:
			if (!readZeroFlag(_reg.f)) {
				_reg.pc += offset;
				_opcodeCycle += 1;
			}
			break;
		case Zero:
			if (readZeroFlag(_reg.f)) {
				_reg.pc += offset;
				_opcodeCycle += 1;
			}
			break;
		case NCarry:
			if (!readCarryFlag(_reg.f)) {
				_reg.pc += offset;
				_opcodeCycle += 1;
			}
			break;
		case Carry:
			if (readCarryFlag(_reg.f)) {
				_reg.pc += offset;
				_opcodeCycle += 1;
			}
			break;
	}
}

/* Calls */
void gbz80::CALL()
{
	push(_reg.pc+2);

	_reg.pc = _memory->rdWordMMU(_reg.pc);
}
void gbz80::CALLC(const flags flag)
{
	uint16_t addr = _memory->rdWordMMU(_reg.pc);
	_reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(_reg.f)) {
				push(_reg.pc);
				_reg.pc = addr;
				_opcodeCycle += 3;
			}
			break;
		case Zero:
			if (readZeroFlag(_reg.f)) {
				push(_reg.pc);
				_reg.pc = addr;
				_opcodeCycle += 3;
			}
			break;
		case NCarry:
			if (!readCarryFlag(_reg.f)) {
				push(_reg.pc);
				_reg.pc = addr;
				_opcodeCycle += 3;
			}
			break;
		case Carry:
			if (readCarryFlag(_reg.f)) {
				push(_reg.pc);
				_reg.pc = addr;
				_opcodeCycle += 3;
			}
			break;
	}
}

/* Resets */
void gbz80::RST(const uint8_t n)
{
	push(_reg.pc);
	_reg.pc = n;

}

/* Returns */
void gbz80::RET()
{
	pop(_reg.pc);
}
void gbz80::RETC(const flags flag)
{
	//_reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(_reg.f)) {
				pop(_reg.pc);
				_opcodeCycle += 3;
			}
			break;
		case Zero:
			if (readZeroFlag(_reg.f)) {
				pop(_reg.pc);
				_opcodeCycle += 3;
			}
			break;
		case NCarry:
			if (!readCarryFlag(_reg.f)) {
				pop(_reg.pc);
				_opcodeCycle += 3;
			}
			break;
		case Carry:
			if (readCarryFlag(_reg.f)) {
				pop(_reg.pc);
				_opcodeCycle += 3;
			}
			break;
	}
}
void gbz80::RETI()
{
	pop(_reg.pc);
	_enableInterrupt = true;
}
