#pragma once
#include <stdint.h>
#include "common.h"

class BaseLogger
{
	public:
		~BaseLogger() {}
		virtual void log(debug& log) {}
		static BaseLogger* getInstance() {
			static BaseLogger log;
			return &log;
		}
	protected:
		BaseLogger() {}
		const char* instr[0x100] = {
			"NOP           ",		//0x00
			"LD BC, nn     ",		//0x01
			"LD (BC), A    ",		//0x02
			"INC BC        ",		//0x03
			"INC B         ",		//0x04
			"DEC B         ",		//0x05
			"LD  B, n      ",		//0x06
			"RLCA          ",		//0x07
			"LD (nn), SP   ",		//0x08
			"ADD HL, BC    ",		//0x09
			"LD A, (BC)    ",		//0x0A
			"DEC BC        ",		//0x0B
			"INC C         ",		//0x0C
			"DEC C         ",		//0x0D
			"LD C, n       ",		//0x0E
			"RRCA          ",		//0x0F
			"STOP          ",		//0x10
			"LD DE, nn     ",		//0x11
			"LD (DE), A    ",		//0x12
			"INC DE        ",		//0x13
			"INC D         ",		//0x14
			"DEC D         ",		//0x15
			"LD D, n       ",		//0x16
			"RLA           ",		//0x17
			"JR n          ",		//0x18
			"ADD HL, BC    ",		//0x19
			"LD A, (DE)    ",		//0x1A
			"DEC DE        ",		//0x1B
			"INC E         ",		//0x1C
			"DEC E         ",		//0x1D
			"LD E, n       ",		//0x1E
			"RRA           ",		//0x1F
			"JR NZ, n      ",		//0x20
			"LD HL, nn     ",		//0x21
			"LDI (HL), A   ",		//0x22
			"INC HL        ",		//0x23
			"INC H         ",		//0x24
			"DEC H         ",		//0x25
			"LD H, n       ",		//0x26
			"DAA           ",		//0x27
			"JR Z, n       ",		//0x28
			"ADD HL, DE    ",		//0x29
			"LDI A, (HL)   ",		//0x2A
			"DEC HL        ",		//0x2B
			"INC L         ",		//0x2C
			"DEC L         ",		//0x2D
			"LD L, n       ",		//0x2E
			"CPL           ",		//0x2F
			"JR NC, n      ",		//0x30
			"LD SP, nn     ",		//0x31
			"LDD (HL), A   ",		//0x32
			"INC SP        ",		//0x33
			"INC (HL)      ",		//0x34
			"DEC (HL)      ",		//0x35
			"LD (HL), n    ",		//0x36
			"SCF           ",		//0x37
			"JR C, n       ",		//0x38
			"ADD HL, SP    ",		//0x39
			"LDD A, (HL)   ",		//0x3A
			"DEC SP        ",		//0x3B
			"INC A         ",		//0x3C
			"DEC A         ",		//0x3D
			"LD A, n       ",		//0x3E
			"CCF           ",		//0x3F
			"LD B, B       ",		//0x40
			"LD B, C       ",		//0x41
			"LD B, D       ",		//0x42
			"LD B, E       ",		//0x43
			"LD B, H       ",		//0x44
			"LD B, L       ",		//0x45
			"LD B, (HL)    ",		//0x46
			"LD B, A       ",		//0x47
			"LD C, B       ",		//0x48
			"LD C, C       ",		//0x49
			"LD C, D       ",		//0x4A
			"LD C, E       ",		//0x4B
			"LD C, H       ",		//0x4C
			"LD C, L       ",		//0x4D
			"LD C, (HL)    ",		//0x4E
			"LD C, A       ",		//0x4F
			"LD D, B       ",		//0x50
			"LD D, C       ",		//0x51
			"LD D, D       ",		//0x52
			"LD D, E       ",		//0x53
			"LD D, H       ",		//0x54
			"LD D, L       ",		//0x55
			"LD D, (HL)    ",		//0x56
			"LD D, A       ",		//0x57
			"LD E, B       ",		//0x58
			"LD E, C       ",		//0x59
			"LD E, D       ",		//0x5A
			"LD E, E       ",		//0x5B
			"LD E, H       ",		//0x5C
			"LD E, L       ",		//0x5D
			"LD E, (HL)    ",		//0x5E
			"LD E, A       ",		//0x5F
			"LD H, B       ",		//0x60
			"LD H, C       ",		//0x61
			"LD H, D       ",		//0x62
			"LD H, E       ",		//0x63
			"LD H, H       ",		//0x64
			"LD H, L       ",		//0x65
			"LD H, (HL)    ",		//0x66
			"LD H, A       ",		//0x67
			"LD L, B       ",		//0x68
			"LD L, C       ",		//0x69
			"LD L, D       ",		//0x6A
			"LD L, E       ",		//0x6B
			"LD L, H       ",		//0x6C
			"LD L, L       ",		//0x6D
			"LD L, (HL)    ",		//0x6E
			"LD (HL), A    ",		//0x6F
			"LD (HL), B    ",		//0x70
			"LD (HL), C    ",		//0x71
			"LD (HL), D    ",		//0x72
			"LD (HL), E    ",		//0x73
			"LD (HL), H    ",		//0x74
			"LD (HL), L    ",		//0x75
			"HALT          ",		//0x76
			"LD (HL), A    ",		//0x77
			"LD A, B       ",		//0x78
			"LD A, C       ",		//0x79
			"LD A, D       ",		//0x7A
			"LD A, E       ",		//0x7B
			"LD A, H       ",		//0x7C
			"LD A, L       ",		//0x7D
			"LD A, (HL)    ",		//0x7E
			"LD A, A       ",		//0x7F
			"ADD A, B      ",		//0x80
			"ADD A, C      ",		//0x81
			"ADD A, D      ",		//0x82
			"ADD A, E      ",		//0x83
			"ADD A, H      ",		//0x84
			"ADD A, L      ",		//0x85
			"ADD A, (HL)   ",		//0x86
			"ADD A, A      ",		//0x87
			"ADDC A, B     ",		//0x88
			"ADDC A, C     ",		//0x89
			"ADDC A, D     ",		//0x8A
			"ADDC A, E     ",		//0x8B
			"ADDC A, H     ",		//0x8C
			"ADDC A, L     ",		//0x8D
			"ADDC A, (HL)  ",		//0x8E
			"ADDC A, A     ",		//0x8F
			"SUB A, B      ",		//0x90
			"SUB A, C      ",		//0x91
			"SUB A, D      ",		//0x92
			"SUB A, E      ",		//0x93
			"SUB A, H      ",		//0x94
			"SUB A, L      ",		//0x95
			"SUB A, (HL)   ",		//0x96
			"SUB A, A      ",		//0x97
			"SUBC A, B     ",		//0x98
			"SUBC A, C     ",		//0x99
			"SUBC A, D     ",		//0x9A
			"SUBC A, E     ",		//0x9B
			"SUBC A, H     ",		//0x9C
			"SUBC A, L     ",		//0x9D
			"SUBC A, (HL)  ",		//0x9E
			"SUBC A, A     ",		//0x9F
			"AND A, B      ",		//0xA0
			"AND A, C      ",		//0xA1
			"AND A, D      ",		//0xA2
			"AND A, E      ",		//0xA3
			"AND A, H      ",		//0xA4
			"AND A, L      ",		//0xA5
			"AND A, (HL)   ",		//0xA6
			"AND A, A      ",		//0xA7
			"XOR A, B      ",		//0xA8
			"XOR A, C      ",		//0xA9
			"XOR A, D      ",		//0xAA
			"XOR A, E      ",		//0xAB
			"XOR A, H      ",		//0xAC
			"XOR A, L      ",		//0xAD
			"XOR A, (HL)   ",		//0xAE
			"XOR A, A      ",		//0xAF
			"OR A, B       ",		//0xB0
			"OR A, C       ",		//0xB1
			"OR A, D       ",		//0xB2
			"OR A, E       ",		//0xB3
			"OR A, H       ",		//0xB4
			"OR A, L       ",		//0xB5
			"OR A, (HL)    ",		//0xB6
			"OR A, A       ",		//0xB7
			"CP B          ",		//0xB8
			"CP C          ",		//0xB9
			"CP D          ",		//0xBA
			"CP E          ",		//0xBB
			"CP H          ",		//0xBC
			"CP L          ",		//0xBD
			"CP (HL)       ",		//0xBE
			"CP A          ",		//0xBF
			"RET NZ        ",		//0xC0
			"POP BC        ",       //0xC1
			"JP NZ, nn     ",       //0xC2
			"JP nn         ",		//0xC3
			"CALL NZ, nn   ",		//0xC4
			"PUSH BC       ",		//0xC5
			"ADD A, n      ",		//0xC6
			"RST 00        ",		//0xC7
			"RET Z         ",		//0xC8
			"RET           ",		//0xC9
			"JP Z, nn      ",		//0xCA
			"EXTENDOP      ",		//0xCB
			"CALL Z, nn    ",		//0xCC
			"CALL nn       ",		//0xCD
			"ADDC A, n     ",		//0xCE
			"RST 08        ",		//0xCF
			"RET NC        ",		//0xD0
			"POP DE        ",		//0xD1
			"JP NC, nn     ",		//0xD2
			"undefined     ",		//0xD3
			"CALL NC, nn   ",		//0xD4
			"PUSH DE       ",		//0xD5
			"SUB A, n      ",		//0xD6
			"RST 10        ",		//0xD7
			"RET C         ",		//0xD8
			"RETI          ",		//0xD9
			"JP C, nn      ",		//0xDA
			"undefined     ",       //0xDB
			"CALL C, nn    ",		//0xDC
			"undefined     ",		//0xDD
			"SUBC A, n     ",		//0xDE
			"RST 18        ",		//0xDF
			"LD (FF00+n), A",		//0xE0
			"POP HL        ",		//0xE1
			"ld (FF00+C), A",		//0xE2
			"undefined     ",		//0xE3
			"undefined     ",		//0xE4
			"PUSH HL       ",		//0xE5
			"AND A, n      ",		//0xE6
			"RST 20        ",		//0xE7
			"ADD SP, n     ",		//0xE8
			"JP (HL)       ",		//0xE9
			"LD (nn), A    ",		//0xEA
			"undefined     ",		//0xEB
			"undefined     ",		//0xEC
			"undefined     ",		//0xED
			"XOR A, n      ",		//0xEE
			"RST 28        ",		//0xEF
			"LD A, (FF00+n)",		//0xF0
			"POP AF        ",		//0xF1
			"LD A, (FF00+C)",		//0xF2
			"DI            ",		//0xF3
			"undefined     ",		//0xF4
			"PUSH AF       ",		//0xF5
			"OR A, n       ",		//0xF6
			"RST 30        ",		//0xF7
			"LDHL SP, n    ",		//0xF8
			"LD SP, HL     ",		//0xF9
			"LD A, (nn)    ",		//0xFA
			"EI            ",		//0xFB
			"undefined     ",		//0xFC
			"undefined     ",		//0xFD
			"CP n          ",		//0xFE
			"RST 38        "        //0xFF
		};
};
