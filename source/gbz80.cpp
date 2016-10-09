#include "gbz80.h"

static int opcodeMachineCycles[256] = {
	 4, 12,  8,  8,  4,  4,  8,  4, 20, 8,  8, 8,  4,  4,  8,  4,	//0x00
	 4, 12,  8,  8,  4,  4,  8,  4,  8, 8,  8, 8,  4,  4,  8,  4,	//0x10 
	 8, 12,  8,  8,  4,  4,  8,  4,  8, 8,  8, 8,  4,  4,  8,  4,	//0x20
	 8, 12,  8,  8, 12, 12, 12,  4,  8, 8,  8, 8,  4,  4,  8,  4,	//0x30
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0x40
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0x50
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0x60
	 8,  8,  8,  8,  8,  8,  4,  8,  4, 4,  4, 4,  4,  4,  8,  4,	//0x70
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0x80
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0x90
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0xA0
	 4,  4,  4,  4,  4,  4,  8,  4,  4, 4,  4, 4,  4,  4,  8,  4,	//0xB0
	 8, 12, 12, 12, 12, 16,  8, 32,  8, 8, 12, 0, 12, 12,  8, 32,	//0xC0
	 8, 12, 12,  0, 12, 16,  8, 32,  8, 8, 12, 0, 12,  0,  8, 32,	//0xD0
	12, 12,  8,  0,  0, 16,  8, 32, 16, 4, 16, 0,  0,  0,  8, 32,	//0xE0
	12, 12,  8,  4,  0, 16,  8, 32, 12, 8, 16, 4,  0,  0,  8, 32,	//0xF0
};
static int extendedOpcodeMachineCycles[256] = {
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x00
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x10
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x20
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x30
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x40
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x50
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x60
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x70
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x80
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0x90
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0xA0
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0xB0
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0xC0
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0xD0
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8,	//0xE0
	8, 8, 8, 8, 8, 8, 16, 8, 8,	8, 8, 8, 8, 8, 16, 8	//0xF0
};

/* system functions */
gbz80::gbz80(MMU* memory)
	:m_memory(memory), m_log(BaseLogger::getInstance()) 
{}

gbz80::gbz80(MMU* memory, BaseLogger* log)
	:m_memory(memory), m_log(log)
{}

void gbz80::loadGame()
{
	reg = {};
	reg.sp = 0x7FFE;
	enableInterrupt		=		false;

	disableNext			=		false;
	enableNext			=		false;

	m_halt				=		false;
	m_stop				=		false;
}

int gbz80::advanceCPU()
{
	regset temp = reg;
	uint8_t index;

	opcodeIndex = m_memory->rdByteMMU(reg.pc++);
	opcodeCycle = opcodeMachineCycles[opcodeIndex];

	index = opcodeIndex;

	if (enableNext) {
		enableNext			=	false;
		enableInterrupt		=	true;
	}
	if (disableNext) {
		disableNext			=	false;
		enableInterrupt		=	false;
	}
	//DI HALT Bug
	//if ( m_halt && !enableInterrupt ) {
	//	--reg.pc;
	//	m_halt = false;
	//}

	try {
		switch (opcodeIndex) {
			case 0x00: break;

			case 0x01: ldReg16(reg.bc, m_memory->rdWordMMU(reg.pc)); reg.pc += 2; break;
			case 0x11: ldReg16(reg.de, m_memory->rdWordMMU(reg.pc)); reg.pc += 2; break;
			case 0x21: ldReg16(reg.hl, m_memory->rdWordMMU(reg.pc)); reg.pc += 2; break;
			case 0x31: ldReg16(reg.sp, m_memory->rdWordMMU(reg.pc)); reg.pc += 2; break;
		
			case 0x02: wrMem(reg.bc, reg.a); break;
			case 0x12: wrMem(reg.de, reg.a); break;

			case 0x03: incReg16(reg.bc); break;
			case 0x13: incReg16(reg.de); break;
			case 0x23: incReg16(reg.hl); break;
			case 0x33: incReg16(reg.sp); break;

			case 0x0B: decReg16(reg.bc); break;
			case 0x1B: decReg16(reg.de); break;
			case 0x2B: decReg16(reg.hl); break;
			case 0x3B: decReg16(reg.sp); break;

			case 0x04: incReg8(reg.b); break;
			case 0x0C: incReg8(reg.c); break;
			case 0x14: incReg8(reg.d); break;
			case 0x1C: incReg8(reg.e); break;
			case 0x24: incReg8(reg.h); break;
			case 0x2C: incReg8(reg.l); break;
			case 0x34: incMem8(reg.hl); break;
			case 0x3C: incReg8(reg.a); break;

			case 0x05: decReg8(reg.b); break;
			case 0x0D: decReg8(reg.c); break;
			case 0x15: decReg8(reg.d); break;
			case 0x1D: decReg8(reg.e); break;
			case 0x25: decReg8(reg.h); break;
			case 0x2D: decReg8(reg.l); break;
			case 0x35: decMem8(reg.hl); break;
			case 0x3D: decReg8(reg.a); break;

			case 0x07: RLCA(reg.a); break;
			case 0x17: RLA(reg.a); break;
			case 0x0F: RRCA(reg.a); break;
			case 0x1F: RRA(reg.a); break;

			case 0x09: addHL(reg.bc); break;
			case 0x19: addHL(reg.de); break;
			case 0x29: addHL(reg.hl); break;
			case 0x39: addHL(reg.sp); break;

			case 0xE8: addSP(); break;

			case 0x0A: ldMem(reg.a, reg.bc); break;
			case 0x1A: ldMem(reg.a, reg.de); break;

			case 0x22: wrMem(reg.hl++, reg.a); break;
			case 0x2A: ldMem(reg.a, reg.hl++); break;

			case 0x32: wrMem(reg.hl--, reg.a); break;
			case 0x3A: ldMem(reg.a, reg.hl--); break;

			case 0x27: DAA(); break;
			case 0x2F: CPL(); break;

			case 0x37: SCF(); break;
			case 0x3F: CCF(); break;

			case 0x06: ldMem(reg.b, reg.pc++); break;
			case 0x0E: ldMem(reg.c, reg.pc++); break;
			case 0x16: ldMem(reg.d, reg.pc++); break;
			case 0x1E: ldMem(reg.e, reg.pc++); break;
			case 0x26: ldMem(reg.h, reg.pc++); break;
			case 0x2E: ldMem(reg.l, reg.pc++); break;
			case 0x36: wrMem(reg.hl, m_memory->rdByteMMU(reg.pc++)); break;
			case 0x3E: ldMem(reg.a,  reg.pc++); break;

			case 0x08: m_memory->wrWordMMU(m_memory->rdWordMMU(reg.pc), reg.sp); reg.pc += 2; break;
		
			case 0x78: ldReg8(reg.a, reg.b); break;
			case 0x79: ldReg8(reg.a, reg.c); break;
			case 0x7A: ldReg8(reg.a, reg.d); break;
			case 0x7B: ldReg8(reg.a, reg.e); break;
			case 0x7C: ldReg8(reg.a, reg.h); break;
			case 0x7D: ldReg8(reg.a, reg.l); break;
			case 0x7E: ldMem(reg.a, reg.hl); break;
			case 0x7F: ldReg8(reg.a, reg.a); break;

			case 0x40: ldReg8(reg.b, reg.b); break;
			case 0x41: ldReg8(reg.b, reg.c); break;
			case 0x42: ldReg8(reg.b, reg.d); break;
			case 0x43: ldReg8(reg.b, reg.e); break;
			case 0x44: ldReg8(reg.b, reg.h); break;
			case 0x45: ldReg8(reg.b, reg.l); break;
			case 0x46: ldMem(reg.b, reg.hl); break;
			case 0x47: ldReg8(reg.b, reg.a); break;

			case 0x48: ldReg8(reg.c, reg.b); break;
			case 0x49: ldReg8(reg.c, reg.c); break;
			case 0x4A: ldReg8(reg.c, reg.d); break;
			case 0x4B: ldReg8(reg.c, reg.e); break;
			case 0x4C: ldReg8(reg.c, reg.h); break;
			case 0x4D: ldReg8(reg.c, reg.l); break;
			case 0x4E: ldMem(reg.c, reg.hl); break;
			case 0x4F: ldReg8(reg.c, reg.a); break;

			case 0x50: ldReg8(reg.d, reg.b); break;
			case 0x51: ldReg8(reg.d, reg.c); break;
			case 0x52: ldReg8(reg.d, reg.d); break;
			case 0x53: ldReg8(reg.d, reg.e); break;
			case 0x54: ldReg8(reg.d, reg.h); break;
			case 0x55: ldReg8(reg.d, reg.l); break;
			case 0x56: ldMem(reg.d, reg.hl); break;
			case 0x57: ldReg8(reg.d, reg.a); break;

			case 0x58: ldReg8(reg.e, reg.b); break;
			case 0x59: ldReg8(reg.e, reg.c); break;
			case 0x5A: ldReg8(reg.e, reg.d); break;
			case 0x5B: ldReg8(reg.e, reg.e); break;
			case 0x5C: ldReg8(reg.e, reg.h); break;
			case 0x5D: ldReg8(reg.e, reg.l); break;
			case 0x5E: ldMem(reg.e, reg.hl); break;
			case 0x5F: ldReg8(reg.e, reg.a); break;

			case 0x60: ldReg8(reg.h, reg.b); break;
			case 0x61: ldReg8(reg.h, reg.c); break;
			case 0x62: ldReg8(reg.h, reg.d); break;
			case 0x63: ldReg8(reg.h, reg.e); break;
			case 0x64: ldReg8(reg.h, reg.h); break;
			case 0x65: ldReg8(reg.h, reg.l); break;
			case 0x66: ldMem(reg.h, reg.hl); break;
			case 0x67: ldReg8(reg.h, reg.a); break;
			   
			case 0x68: ldReg8(reg.l, reg.b); break;
			case 0x69: ldReg8(reg.l, reg.c); break;
			case 0x6A: ldReg8(reg.l, reg.d); break;
			case 0x6B: ldReg8(reg.l, reg.e); break;
			case 0x6C: ldReg8(reg.l, reg.h); break;
			case 0x6D: ldReg8(reg.l, reg.l); break;
			case 0x6E: ldMem(reg.l, reg.hl); break;
			case 0x6F: ldReg8(reg.l, reg.a); break;

			case 0x70: wrMem(reg.hl, reg.b); break;
			case 0x71: wrMem(reg.hl, reg.c); break;
			case 0x72: wrMem(reg.hl, reg.d); break;
			case 0x73: wrMem(reg.hl, reg.e); break;
			case 0x74: wrMem(reg.hl, reg.h); break;
			case 0x75: wrMem(reg.hl, reg.l); break;
			case 0x77: wrMem(reg.hl, reg.a); break;
		
			case 0x80: addReg8(reg.b); break;
			case 0x81: addReg8(reg.c); break;
			case 0x82: addReg8(reg.d); break;
			case 0x83: addReg8(reg.e); break;
			case 0x84: addReg8(reg.h); break;
			case 0x85: addReg8(reg.l); break;
			case 0x86: addReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0x87: addReg8(reg.a); break;

			case 0xC6: addReg8(m_memory->rdByteMMU(reg.pc++)); break;

			case 0x88: addCReg8(reg.b); break;
			case 0x89: addCReg8(reg.c); break;
			case 0x8A: addCReg8(reg.d); break;
			case 0x8B: addCReg8(reg.e); break;
			case 0x8C: addCReg8(reg.h); break;
			case 0x8D: addCReg8(reg.l); break;
			case 0x8E: addCReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0x8F: addCReg8(reg.a); break;
			case 0xCE: addCReg8(m_memory->rdByteMMU(reg.pc++)); break;

			case 0x90: subReg8(reg.b); break;
			case 0x91: subReg8(reg.c); break;
			case 0x92: subReg8(reg.d); break;
			case 0x93: subReg8(reg.e); break;
			case 0x94: subReg8(reg.h); break;
			case 0x95: subReg8(reg.l); break;
			case 0x96: subReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0x97: subReg8(reg.a); break;

			case 0xD6: subReg8(m_memory->rdByteMMU(reg.pc++)); break;
				   
			case 0x98: subCReg8(reg.b); break;
			case 0x99: subCReg8(reg.c); break;
			case 0x9A: subCReg8(reg.d); break;
			case 0x9B: subCReg8(reg.e); break;
			case 0x9C: subCReg8(reg.h); break;
			case 0x9D: subCReg8(reg.l); break;
			case 0x9E: subCReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0x9F: subCReg8(reg.a); break;
			case 0xDE: subCReg8(m_memory->rdByteMMU(reg.pc++)); break;

			case 0xA0: andReg8(reg.b); break;
			case 0xA1: andReg8(reg.c); break;
			case 0xA2: andReg8(reg.d); break;
			case 0xA3: andReg8(reg.e); break;
			case 0xA4: andReg8(reg.h); break;
			case 0xA5: andReg8(reg.l); break;
			case 0xA6: andReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0xA7: andReg8(reg.a); break;
			case 0xE6: andReg8(m_memory->rdByteMMU(reg.pc++)); break;
			   
			case 0xA8: xorReg8(reg.b); break;
			case 0xA9: xorReg8(reg.c); break;
			case 0xAA: xorReg8(reg.d); break;
			case 0xAB: xorReg8(reg.e); break;
			case 0xAC: xorReg8(reg.h); break;
			case 0xAD: xorReg8(reg.l); break;
			case 0xAE: xorReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0xAF: xorReg8(reg.a); break;
			case 0xEE: xorReg8(m_memory->rdByteMMU(reg.pc++)); break;

			case 0xB0: orReg8(reg.b); break;
			case 0xB1: orReg8(reg.c); break;
			case 0xB2: orReg8(reg.d); break;
			case 0xB3: orReg8(reg.e); break;
			case 0xB4: orReg8(reg.h); break;
			case 0xB5: orReg8(reg.l); break;
			case 0xB6: orReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0xB7: orReg8(reg.a); break;
			case 0xF6: orReg8(m_memory->rdByteMMU(reg.pc++)); break;

			case 0xB8: cpReg8(reg.b); break;
			case 0xB9: cpReg8(reg.c); break;
			case 0xBA: cpReg8(reg.d); break;
			case 0xBB: cpReg8(reg.e); break;
			case 0xBC: cpReg8(reg.h); break;
			case 0xBD: cpReg8(reg.l); break;
			case 0xBE: cpReg8(m_memory->rdByteMMU(reg.hl)); break;
			case 0xBF: cpReg8(reg.a); break;
			case 0xFE: cpReg8(m_memory->rdByteMMU(reg.pc++)); break;

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
 
			case 0xC5: push(reg.bc); break;
			case 0xD5: push(reg.de); break;
			case 0xE5: push(reg.hl); break;
			case 0xF5: push(reg.af); break;

			case 0xC1: pop(reg.bc); break;
			case 0xD1: pop(reg.de); break;
			case 0xE1: pop(reg.hl); break;
			case 0xF1: pop(reg.af); reg.f &= 0xF0; break;
		
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

			case 0xE0: wrMem((m_memory->rdByteMMU(reg.pc++)|0xFF00), reg.a); break;
			case 0xF0: ldMem(reg.a, (m_memory->rdByteMMU(reg.pc++) | 0xFF00)); break;

			case 0xE2: wrMem((reg.c|0xFF00), reg.a); break;
			case 0xF2: ldMem(reg.a, (reg.c|0xFF00)); break;
			case 0xF8: ldHL(); break;
			case 0xF9: ldReg16(reg.sp, reg.hl); break;

			case 0xF3: DI(); break;
			case 0xFB: EI(); break;

			case 0xEA: wrMem(m_memory->rdWordMMU(reg.pc), reg.a); reg.pc += 2; break;
			case 0xFA: ldMem(reg.a, m_memory->rdWordMMU(reg.pc)); reg.pc += 2; break;

			case 0xCB: opcodeCycle = extendedOP(); break;

			case 0x10: STOP(); break;
			case 0x76: HALT(); break;

			default: throw (uint32_t) opcodeIndex;
		}
	}
	catch (const uint32_t e) {
		std::cerr << "Instruction not implemented, Opcode:"  << std::hex << e << std::endl;
		exit(-1);
	}

	//Log Info
	m_log->log(debug{ temp, reg, index });

	return opcodeCycle;

}

/* CB Extension Command */
int gbz80::extendedOP()
{
	uint8_t temp;
	opcodeIndex = m_memory->rdByteMMU(reg.pc++);

	try {
		switch (opcodeIndex) {
			case 0x00: RLC(reg.b); break;
			case 0x01: RLC(reg.c); break;
			case 0x02: RLC(reg.d); break;
			case 0x03: RLC(reg.e); break;
			case 0x04: RLC(reg.h); break;
			case 0x05: RLC(reg.l); break;
			case 0x06: temp = m_memory->rdByteMMU(reg.hl); RLC(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x07: RLC(reg.a); break;

			case 0x08: RRC(reg.b); break;
			case 0x09: RRC(reg.c); break;
			case 0x0A: RRC(reg.d); break;
			case 0x0B: RRC(reg.e); break;
			case 0x0C: RRC(reg.h); break;
			case 0x0D: RRC(reg.l); break;
			case 0x0E: temp = m_memory->rdByteMMU(reg.hl); RRC(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x0F: RRC(reg.a); break;

			case 0x10: RL(reg.b); break;
			case 0x11: RL(reg.c); break;
			case 0x12: RL(reg.d); break;
			case 0x13: RL(reg.e); break;
			case 0x14: RL(reg.h); break;
			case 0x15: RL(reg.l); break;
			case 0x16: temp = m_memory->rdByteMMU(reg.hl); RL(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x17: RL(reg.a); break;

			case 0x18: RR(reg.b); break;
			case 0x19: RR(reg.c); break;
			case 0x1A: RR(reg.d); break;
			case 0x1B: RR(reg.e); break;
			case 0x1C: RR(reg.h); break;
			case 0x1D: RR(reg.l); break;
			case 0x1E: temp = m_memory->rdByteMMU(reg.hl); RR(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x1F: RR(reg.a); break;

			case 0x20: SL(reg.b); break;
			case 0x21: SL(reg.c); break;
			case 0x22: SL(reg.d); break;
			case 0x23: SL(reg.e); break;
			case 0x24: SL(reg.h); break;
			case 0x25: SL(reg.l); break;
			case 0x26: temp = m_memory->rdByteMMU(reg.hl); SL(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x27: SL(reg.a); break;
				   
			case 0x28: SRA(reg.b); break;
			case 0x29: SRA(reg.c); break;
			case 0x2A: SRA(reg.d); break;
			case 0x2B: SRA(reg.e); break;
			case 0x2C: SRA(reg.h); break;
			case 0x2D: SRA(reg.l); break;
			case 0x2E: temp = m_memory->rdByteMMU(reg.hl); SRA(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x2F: SRA(reg.a); break;
				
			case 0x30: swapReg8(reg.b); break;
			case 0x31: swapReg8(reg.c);	break;
			case 0x32: swapReg8(reg.d);	break;
			case 0x33: swapReg8(reg.e);	break;
			case 0x34: swapReg8(reg.h);	break;
			case 0x35: swapReg8(reg.l);	break;
			case 0x36: temp = m_memory->rdByteMMU(reg.hl); swapReg8(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x37: swapReg8(reg.a); break;

			case 0x38: SRL(reg.b); break;
			case 0x39: SRL(reg.c); break;
			case 0x3A: SRL(reg.d); break;
			case 0x3B: SRL(reg.e); break;
			case 0x3C: SRL(reg.h); break;
			case 0x3D: SRL(reg.l); break;
			case 0x3E: temp = m_memory->rdByteMMU(reg.hl); SRL(temp); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x3F: SRL(reg.a); break;


			case 0x40: BIT(reg.b, 0); break;
			case 0x41: BIT(reg.c, 0); break;
			case 0x42: BIT(reg.d, 0); break;
			case 0x43: BIT(reg.e, 0); break;
			case 0x44: BIT(reg.h, 0); break;
			case 0x45: BIT(reg.l, 0); break;
			case 0x46: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 0);  break;
			case 0x47: BIT(reg.a, 0); break;
				   
			case 0x48: BIT(reg.b, 1); break;
			case 0x49: BIT(reg.c, 1); break;
			case 0x4A: BIT(reg.d, 1); break;
			case 0x4B: BIT(reg.e, 1); break;
			case 0x4C: BIT(reg.h, 1); break;
			case 0x4D: BIT(reg.l, 1); break;
			case 0x4E: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 1); break;
			case 0x4F: BIT(reg.a, 1); break;

			case 0x50: BIT(reg.b, 2); break;
			case 0x51: BIT(reg.c, 2); break;
			case 0x52: BIT(reg.d, 2); break;
			case 0x53: BIT(reg.e, 2); break;
			case 0x54: BIT(reg.h, 2); break;
			case 0x55: BIT(reg.l, 2); break;
			case 0x56: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 2);  break;
			case 0x57: BIT(reg.a, 2); break;
				   
			case 0x58: BIT(reg.b, 3); break;
			case 0x59: BIT(reg.c, 3); break;
			case 0x5A: BIT(reg.d, 3); break;
			case 0x5B: BIT(reg.e, 3); break;
			case 0x5C: BIT(reg.h, 3); break;
			case 0x5D: BIT(reg.l, 3); break;
			case 0x5E: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 3); break;
			case 0x5F: BIT(reg.a, 3); break;

			case 0x60: BIT(reg.b, 4); break;
			case 0x61: BIT(reg.c, 4); break;
			case 0x62: BIT(reg.d, 4); break;
			case 0x63: BIT(reg.e, 4); break;
			case 0x64: BIT(reg.h, 4); break;
			case 0x65: BIT(reg.l, 4); break;
			case 0x66: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 4);  break;
			case 0x67: BIT(reg.a, 4); break;
				  
			case 0x68: BIT(reg.b, 5); break;
			case 0x69: BIT(reg.c, 5); break;
			case 0x6A: BIT(reg.d, 5); break;
			case 0x6B: BIT(reg.e, 5); break;
			case 0x6C: BIT(reg.h, 5); break;
			case 0x6D: BIT(reg.l, 5); break;
			case 0x6E: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 5); break;
			case 0x6F: BIT(reg.a, 5); break;

			case 0x70: BIT(reg.b, 6); break;
			case 0x71: BIT(reg.c, 6); break;
			case 0x72: BIT(reg.d, 6); break;
			case 0x73: BIT(reg.e, 6); break;
			case 0x74: BIT(reg.h, 6); break;
			case 0x75: BIT(reg.l, 6); break;
			case 0x76: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 6);  break;
			case 0x77: BIT(reg.a, 6); break;
				   
			case 0x78: BIT(reg.b, 7); break;
			case 0x79: BIT(reg.c, 7); break;
			case 0x7A: BIT(reg.d, 7); break;
			case 0x7B: BIT(reg.e, 7); break;
			case 0x7C: BIT(reg.h, 7); break;
			case 0x7D: BIT(reg.l, 7); break;
			case 0x7E: temp = m_memory->rdByteMMU(reg.hl); BIT(temp, 7); break;
			case 0x7F: BIT(reg.a, 7); break;


			case 0x80: RES(reg.b, 0); break;
			case 0x81: RES(reg.c, 0); break;
			case 0x82: RES(reg.d, 0); break;
			case 0x83: RES(reg.e, 0); break;
			case 0x84: RES(reg.h, 0); break;
			case 0x85: RES(reg.l, 0); break;
			case 0x86: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 0); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x87: RES(reg.a, 0); break;
				   
			case 0x88: RES(reg.b, 1); break;
			case 0x89: RES(reg.c, 1); break;
			case 0x8A: RES(reg.d, 1); break;
			case 0x8B: RES(reg.e, 1); break;
			case 0x8C: RES(reg.h, 1); break;
			case 0x8D: RES(reg.l, 1); break;
			case 0x8E: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 1); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x8F: RES(reg.a, 1); break;

			case 0x90: RES(reg.b, 2); break;
			case 0x91: RES(reg.c, 2); break;
			case 0x92: RES(reg.d, 2); break;
			case 0x93: RES(reg.e, 2); break;
			case 0x94: RES(reg.h, 2); break;
			case 0x95: RES(reg.l, 2); break;
			case 0x96: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 2); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x97: RES(reg.a, 2); break;
				   
			case 0x98: RES(reg.b, 3); break;
			case 0x99: RES(reg.c, 3); break;
			case 0x9A: RES(reg.d, 3); break;
			case 0x9B: RES(reg.e, 3); break;
			case 0x9C: RES(reg.h, 3); break;
			case 0x9D: RES(reg.l, 3); break;
			case 0x9E: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 3); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0x9F: RES(reg.a, 3); break;

			case 0xA0: RES(reg.b, 4); break;
			case 0xA1: RES(reg.c, 4); break;
			case 0xA2: RES(reg.d, 4); break;
			case 0xA3: RES(reg.e, 4); break;
			case 0xA4: RES(reg.h, 4); break;
			case 0xA5: RES(reg.l, 4); break;
			case 0xA6: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 4); m_memory->wrByteMMU(reg.hl, temp);  break;
			case 0xA7: RES(reg.a, 4); break;
				   
			case 0xA8: RES(reg.b, 5); break;
			case 0xA9: RES(reg.c, 5); break;
			case 0xAA: RES(reg.d, 5); break;
			case 0xAB: RES(reg.e, 5); break;
			case 0xAC: RES(reg.h, 5); break;
			case 0xAD: RES(reg.l, 5); break;
			case 0xAE: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 5); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xAF: RES(reg.a, 5); break;

			case 0xB0: RES(reg.b, 6); break;
			case 0xB1: RES(reg.c, 6); break;
			case 0xB2: RES(reg.d, 6); break;
			case 0xB3: RES(reg.e, 6); break;
			case 0xB4: RES(reg.h, 6); break;
			case 0xB5: RES(reg.l, 6); break;
			case 0xB6: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 6); m_memory->wrByteMMU(reg.hl, temp);  break;
			case 0xB7: RES(reg.a, 6); break;
				   
			case 0xB8: RES(reg.b, 7); break;
			case 0xB9: RES(reg.c, 7); break;
			case 0xBA: RES(reg.d, 7); break;
			case 0xBB: RES(reg.e, 7); break;
			case 0xBC: RES(reg.h, 7); break;
			case 0xBD: RES(reg.l, 7); break;
			case 0xBE: temp = m_memory->rdByteMMU(reg.hl); RES(temp, 7); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xBF: RES(reg.a, 7); break;


			case 0xC0: SET(reg.b, 0); break;
			case 0xC1: SET(reg.c, 0); break;
			case 0xC2: SET(reg.d, 0); break;
			case 0xC3: SET(reg.e, 0); break;
			case 0xC4: SET(reg.h, 0); break;
			case 0xC5: SET(reg.l, 0); break;
			case 0xC6: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 0); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xC7: SET(reg.a, 0); break;
				   
			case 0xC8: SET(reg.b, 1); break;
			case 0xC9: SET(reg.c, 1); break;
			case 0xCA: SET(reg.d, 1); break;
			case 0xCB: SET(reg.e, 1); break;
			case 0xCC: SET(reg.h, 1); break;
			case 0xCD: SET(reg.l, 1); break;
			case 0xCE: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 1); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xCF: SET(reg.a, 1); break;

			case 0xD0: SET(reg.b, 2); break;
			case 0xD1: SET(reg.c, 2); break;
			case 0xD2: SET(reg.d, 2); break;
			case 0xD3: SET(reg.e, 2); break;
			case 0xD4: SET(reg.h, 2); break;
			case 0xD5: SET(reg.l, 2); break;
			case 0xD6: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 2); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xD7: SET(reg.a, 2); break;
				   
			case 0xD8: SET(reg.b, 3); break;
			case 0xD9: SET(reg.c, 3); break;
			case 0xDA: SET(reg.d, 3); break;
			case 0xDB: SET(reg.e, 3); break;
			case 0xDC: SET(reg.h, 3); break;
			case 0xDD: SET(reg.l, 3); break;
			case 0xDE: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 3); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xDF: SET(reg.a, 3); break;

			case 0xE0: SET(reg.b, 4); break;
			case 0xE1: SET(reg.c, 4); break;
			case 0xE2: SET(reg.d, 4); break;
			case 0xE3: SET(reg.e, 4); break;
			case 0xE4: SET(reg.h, 4); break;
			case 0xE5: SET(reg.l, 4); break;
			case 0xE6: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 4); m_memory->wrByteMMU(reg.hl, temp);  break;
			case 0xE7: SET(reg.a, 4); break;
				   
			case 0xE8: SET(reg.b, 5); break;
			case 0xE9: SET(reg.c, 5); break;
			case 0xEA: SET(reg.d, 5); break;
			case 0xEB: SET(reg.e, 5); break;
			case 0xEC: SET(reg.h, 5); break;
			case 0xED: SET(reg.l, 5); break;
			case 0xEE: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 5); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xEF: SET(reg.a, 5); break;

			case 0xF0: SET(reg.b, 6); break;
			case 0xF1: SET(reg.c, 6); break;
			case 0xF2: SET(reg.d, 6); break;
			case 0xF3: SET(reg.e, 6); break;
			case 0xF4: SET(reg.h, 6); break;
			case 0xF5: SET(reg.l, 6); break;
			case 0xF6: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 6); m_memory->wrByteMMU(reg.hl, temp);  break;
			case 0xF7: SET(reg.a, 6); break;
				   
			case 0xF8: SET(reg.b, 7); break;
			case 0xF9: SET(reg.c, 7); break;
			case 0xFA: SET(reg.d, 7); break;
			case 0xFB: SET(reg.e, 7); break;
			case 0xFC: SET(reg.h, 7); break;
			case 0xFD: SET(reg.l, 7); break;
			case 0xFE: temp = m_memory->rdByteMMU(reg.hl); SET(temp, 7); m_memory->wrByteMMU(reg.hl, temp); break;
			case 0xFF: SET(reg.a, 7); break;

			default: throw (uint32_t)opcodeIndex;
		}
	}
	catch (const uint32_t e) {
		std::cerr << "Extended Instruction not implemented, Opcode: CB " << std::hex << e << std::endl;
		exit(-1);
	}

	return extendedOpcodeMachineCycles[opcodeIndex];
}

void gbz80::processInterrupt()
{
	int i = 4;
	int polledInterrupts;

	polledInterrupts = m_memory->mmIO[IF] & m_memory->mmIO[IE] & 0x1F;
	if (polledInterrupts) {
		m_halt = false;
		if (enableInterrupt) {
		
			while (!((polledInterrupts >> i) & 1))
				--i;

			push(reg.pc);

			switch (i) {
				case 0:		//V-blank
					reg.pc = 0x40;
					break;
				case 1:		//LCD-STAT
					reg.pc = 0x48;
					break;
				case 2:		//Timer
					reg.pc = 0x50;
					break;
				case 3:		//Serial Transfer
					reg.pc = 0x58;
					break;
				case 4:		//Joypad
					reg.pc = 0x60;
					break;
			}
			enableInterrupt = false;
			m_memory->mmIO[IF] &= ~(1 << i);
		}
	}
}

bool gbz80::getHalt()
{
	return m_halt;
}

int gbz80::getPC()
{
	return reg.pc;
}

uint32_t gbz80::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::app | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);
		
		file.write(reinterpret_cast<char*>(&enableInterrupt), sizeof(bool));
		file.write(reinterpret_cast<char*>(&disableNext), sizeof(bool));
		file.write(reinterpret_cast<char*>(&enableNext), sizeof(bool));
		file.write(reinterpret_cast<char*>(&m_halt), sizeof(bool));
		file.write(reinterpret_cast<char*>(&m_stop), sizeof(bool));

		file.write(reinterpret_cast<char*>(&reg), sizeof(regset));

		file.close();
	}

	return offset + sizeof(bool)*5 + sizeof(regset);
}

uint32_t gbz80::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);

		file.read(reinterpret_cast<char*>(&enableInterrupt), sizeof(bool));
		file.read(reinterpret_cast<char*>(&disableNext), sizeof(bool));
		file.read(reinterpret_cast<char*>(&enableNext), sizeof(bool));
		file.read(reinterpret_cast<char*>(&m_halt), sizeof(bool));
		file.read(reinterpret_cast<char*>(&m_stop), sizeof(bool));

		file.read(reinterpret_cast<char*>(&reg), sizeof(regset));

		file.close();
	}

	return offset + sizeof(bool)*5 + sizeof(regset);
}


/* opcodes */
void gbz80::ldHL()
{
	int8_t imm = m_memory->rdByteMMU(reg.pc++);
	uint32_t temp = reg.sp + imm;

	reg.f = ( (0x100 & (reg.sp ^ temp ^ imm))			? carryFlag : 0) 		//Carry Flag
	 |		( (0x10 & (reg.sp ^ temp ^ imm))			? halfCarryFlag: 0);	//Half Carry Flag

	reg.hl = temp;
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
	dest = m_memory->rdByteMMU(src);
}
void gbz80::wrMem(const uint16_t dest, const uint8_t src)
{
	m_memory->wrByteMMU(dest, src);
}


/* Push & Pop */
void gbz80::push(const uint16_t src)
{
	reg.sp -= 2;
	m_memory->wrWordMMU(reg.sp, src);
}
void gbz80::pop(uint16_t& dest)
{
	dest = m_memory->rdWordMMU(reg.sp);
	reg.sp += 2;
}

/* 8-bit ALU instructions */
void gbz80::addReg8(const uint8_t src)
{
	uint32_t temp = reg.a + src;

	reg.f = ( (temp > 0xFF)						? carryFlag : 0)			//Carry
	 |		( (temp & 0xFF)						?  0 : zeroFlag)			//Zero
	 |		( (0x10 & (reg.a ^ src ^ temp) )	? halfCarryFlag : 0);		//Half Carry
	

	reg.a = temp;
}
void gbz80::addCReg8(const uint8_t src)
{
	uint32_t temp = reg.a + src + readCarryFlag(reg.f);

	reg.f = ( (temp > 0xFF)						? carryFlag : 0)		//Carry
	 |		( (temp & 0xFF)						? 0 : zeroFlag)			//Zero
	 |		( (0x10 & (reg.a ^ src ^ temp) )	? halfCarryFlag : 0);	//Half Carry

	reg.a = temp;
}

void gbz80::subReg8(const uint8_t src)
{
	int32_t temp = reg.a - src;

	reg.f = subtractFlag
	 |		( (temp < 0)						? carryFlag : 0)	//Carry
	 |		( (temp & 0xFF)						? 0 : zeroFlag)			//Zero
	 |		( (0x10 & (reg.a ^ src ^ temp))		? halfCarryFlag : 0);	//Half Carry

	reg.a = temp;
}
void gbz80::subCReg8(const uint8_t src)
{
	int32_t temp = reg.a - src - readCarryFlag(reg.f);

	reg.f = subtractFlag
	 |		( (temp < 0)						? carryFlag : 0)		//Carry
	 |		( (temp & 0xFF)						? 0 : zeroFlag)			//Zero
	 |		( (0x10 & (reg.a ^ src ^ temp))		? halfCarryFlag : 0);	//Half Carry

	reg.a = temp;
}

void gbz80::andReg8(const uint8_t src)
{
	reg.a &= src;
	reg.f = reg.a		? halfCarryFlag : (zeroFlag | halfCarryFlag);	//Zero
}
void gbz80::orReg8(const uint8_t src)
{
	reg.a |= src;
	reg.f = reg.a 		? 0 : zeroFlag;		//Zero
}
void gbz80::xorReg8(const uint8_t src)
{
	reg.a ^= src;
	reg.f = reg.a		? 0 : zeroFlag;		//Zero
}

void gbz80::cpReg8(const uint8_t src)
{
	reg.f = subtractFlag
	 |		( (reg.a < src)					? carryFlag : 0)		//Carry
	 |		( (reg.a == src)				? zeroFlag : 0)			//Zero
	 |		( (reg.a & 0xF) < (src & 0xF)	? halfCarryFlag : 0);	//Half Carry
}

void gbz80::incReg8(uint8_t& src)
{
	++src;

	reg.f = (carryFlag & reg.f)									//Carry
	 |		(  src						? 0 : zeroFlag)			//Zero
	 |		( (src & 0xF)				? 0 : halfCarryFlag);	//Half Carry
}

void gbz80::decReg8(uint8_t& src)
{
	--src;

	reg.f = (carryFlag & reg.f)										//Carry
	 |		(  src							? 0 : zeroFlag)			//Zero
	 |		( ((src & 0xF) == 0x0F)			? halfCarryFlag : 0)	//Half Carry
	 |		subtractFlag;											//Subtract
}

void gbz80::incMem8(const uint16_t src)
{
	uint8_t temp = m_memory->rdByteMMU(src) + 1;

	reg.f = (carryFlag & reg.f)									//Carry
	 |		(  temp 					? 0 : zeroFlag)			//Zero
	 |		( (temp & 0xF) 				? 0 : halfCarryFlag);	//Half Carry

	m_memory->wrByteMMU(src, temp);
}
void gbz80::decMem8(const uint16_t src)
{
	uint8_t temp = m_memory->rdByteMMU(src) - 1;

	reg.f = (carryFlag & reg.f)										//Carry
	 |		(  temp							? 0 : zeroFlag)			//Zero
	 |		( ((temp & 0xF) == 0x0F)		? halfCarryFlag : 0)	//Half Carry
	 |		subtractFlag;											//Subtract

	m_memory->wrByteMMU(src, temp);
}

/* 16-bit ALU instructions */
void gbz80::addHL(const uint16_t src)
{
	uint32_t temp = reg.hl + src;

	reg.f = (reg.f & zeroFlag)												//Zero
	 |		( (temp > 0xFFFF)						? carryFlag : 0)		//Carry
	 |		( (0x1000&(reg.hl ^ src ^ temp))		? halfCarryFlag : 0);	//Half Carry

	reg.hl = temp;
}
void gbz80::addSP()
{
	int8_t imm = m_memory->rdByteMMU(reg.pc++);
	uint32_t temp = reg.sp + imm;

	reg.f = ( (0x100 & (reg.sp ^ imm ^ temp))		? carryFlag : 0)		//Carry
	 |		( (0x10  & (reg.sp ^ imm ^ temp))		? halfCarryFlag : 0);	//Half Carry

	reg.sp = temp;
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

	reg.f = src					? 0 : zeroFlag;		//Zero
}

void gbz80::DAA()
{
	uint8_t temp = (reg.f & (subtractFlag | carryFlag));

	if (!readSubtractFlag(reg.f)) {
		if (readCarryFlag(reg.f) || reg.a > 0x99) {
			reg.a += 0x60;
			temp |= carryFlag;
		}
		if (readHalfCarryFlag(reg.f) || (reg.a & 0xF) > 0x9) {
			reg.a += 0x06;
		}
	}
	else if (readHalfCarryFlag(reg.f) &&readCarryFlag(reg.f)) 
		reg.a += 0x9A;
	else if (readCarryFlag(reg.f)) 
		reg.a += 0xA0;
	else if (readHalfCarryFlag(reg.f)) 
		reg.a += 0xFA;
	

	reg.f = (temp | (reg.a 		? 0 : zeroFlag) );		//Zero

}

void gbz80::CPL()
{
	reg.a = ~reg.a;
	reg.f |=	subtractFlag | halfCarryFlag;
}

void gbz80::CCF()
{
	reg.f =  (reg.f^carryFlag)	&	(zeroFlag|carryFlag);
}
void gbz80::SCF()
{
	reg.f = carryFlag | (reg.f & zeroFlag);
}

//stop operations till an interrupt
void gbz80::HALT()
{
	m_halt = true;
}
//stop operations and LCD display until a button press
void gbz80::STOP()
{
	m_stop = true;
}

void gbz80::DI()
{
	disableNext = true;
}
void gbz80::EI()
{
	enableNext = true;
}

/* rotates & shifts */
void gbz80::RLCA(uint8_t& src)
{
	src = (src >> 7) | (src << 1);

	reg.f = ( (src & 0x01)		? carryFlag : 0);			//Carry
}
void gbz80::RLA(uint8_t& src)
{
	uint8_t temp = (src << 1) | readCarryFlag(reg.f);

	reg.f = ( (src & 0x80) >> 3);				//Carry

	src = temp;
}

void gbz80::RRCA(uint8_t& src)
{
	src = (src << 7) | (src >> 1);

	reg.f = ( (src & 0x80) >> 3);			//Carry
	
}
void gbz80::RRA(uint8_t& src)
{
	uint8_t temp = (src >> 1) | (readCarryFlag(reg.f) << 7);

	reg.f = ( (src & 0x01) << 4);			//Carry

	src = temp;
}

void gbz80::SL(uint8_t& src)
{
	reg.f = ( (src & 0x80)			? carryFlag : 0)	//Carry
	 |		( ((src<<1) & 0xFF)		? 0 : zeroFlag);	//Zero

	src <<= 1;
}
void gbz80::SRA(uint8_t& src)
{
	reg.f = ( (src & 0x01)			? carryFlag : 0)	//Carry
	 |		( (src >> 1)			? 0 : zeroFlag);			//Zero

	src = (src & 0x80) | (src >> 1);
}

void gbz80::SRL(uint8_t& src)
{
	reg.f = ( (src & 0x01)		? carryFlag : 0)
	 |		( (src>>1)			? 0 : zeroFlag);

	src = src >> 1;
}

/* Extended Rotates */
void gbz80::RLC(uint8_t& src)
{
	src = (src >> 7) | (src << 1);

	reg.f = ( (src & 0x01)		? carryFlag : 0)	//Carry
	 |		(  src				? 0 : zeroFlag);	//Zero
}
void gbz80::RL(uint8_t& src)
{
	uint8_t temp = (src << 1) | readCarryFlag(reg.f);

	reg.f = ( (src & 0x80) >> 3)				//Carry
	 |		(  temp			? 0 : zeroFlag);	//Zero

	src = temp;
}

void gbz80::RRC(uint8_t& src)
{
	src = (src << 7) | (src >> 1);

	reg.f = ( (src & 0x80) >> 3)					//Carry
	 |		(  src				? 0 : zeroFlag);	//Zero
}
void gbz80::RR(uint8_t& src)
{
	uint8_t temp = (src >> 1) | (readCarryFlag(reg.f) << 7);

	reg.f = ( (src & 0x01) << 4)				//Carry
	 |		(  temp			? 0 : zeroFlag);	//Zero

	src = temp;
}

/*extended bitwise instrucitons */
void gbz80::BIT(const uint8_t src, const uint8_t bit)
{
	reg.f = (reg.f & carryFlag)
	 |		halfCarryFlag								//Half
	 |		( (src & (1<<bit))		? 0 : zeroFlag);	//Zero
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
	reg.pc = m_memory->rdWordMMU(reg.pc);
}
void gbz80::JPC(const flags flag)
{
	uint16_t addr = m_memory->rdWordMMU(reg.pc);
	reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(reg.f))
				reg.pc = addr;
			break;
		case Zero:
			if (readZeroFlag(reg.f))
				reg.pc = addr;
			break;
		case NCarry:
			if (!readCarryFlag(reg.f))
				reg.pc = addr;
			break;
		case Carry:
			if (readCarryFlag(reg.f))
				reg.pc = addr;
			break;
	}
}
void gbz80::JPHL()
{
	reg.pc = reg.hl;
}
void gbz80::JR()
{
	reg.pc += ((int8_t)m_memory->rdByteMMU(reg.pc++));
}
void gbz80::JRC(const flags flag)
{
	int8_t offset = m_memory->rdByteMMU(reg.pc++);

	switch (flag) {
		case NZero:
			if (!readZeroFlag(reg.f))
				reg.pc += offset;
			break;
		case Zero:
			if (readZeroFlag(reg.f))
				reg.pc += offset;
			break;
		case NCarry:
			if (!readCarryFlag(reg.f))
				reg.pc += offset;
			break;
		case Carry:
			if (readCarryFlag(reg.f))
				reg.pc += offset;
			break;
	}
}

/* Calls */
void gbz80::CALL()
{
	push(reg.pc+2);

	reg.pc = m_memory->rdWordMMU(reg.pc);
}
void gbz80::CALLC(const flags flag)
{
	uint16_t addr = m_memory->rdWordMMU(reg.pc);
	reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(reg.f)) {
				push(reg.pc);
				reg.pc = addr;
			}
			break;
		case Zero:
			if (readZeroFlag(reg.f)) {
				push(reg.pc);
				reg.pc = addr;
			}
			break;
		case NCarry:
			if (!readCarryFlag(reg.f)) {
				push(reg.pc);
				reg.pc = addr;
			}
			break;
		case Carry:
			if (readCarryFlag(reg.f)) {
				push(reg.pc);
				reg.pc = addr;
			}
			break;
	}
}

/* Resets */
void gbz80::RST(const uint8_t n)
{
	push(reg.pc);
	reg.pc = n;

}

/* Returns */
void gbz80::RET()
{
	pop(reg.pc);
}
void gbz80::RETC(const flags flag)
{
	//reg.pc += 2;

	switch (flag) {
		case NZero:
			if (!readZeroFlag(reg.f)) {
				pop(reg.pc);
			}
			break;
		case Zero:
			if (readZeroFlag(reg.f)) {
				pop(reg.pc);
			}
			break;
		case NCarry:
			if (!readCarryFlag(reg.f)) {
				pop(reg.pc);
			}
			break;
		case Carry:
			if (readCarryFlag(reg.f)) {
				pop(reg.pc);
			}
			break;
	}
}
void gbz80::RETI()
{
	pop(reg.pc);
	enableInterrupt = true;
}
