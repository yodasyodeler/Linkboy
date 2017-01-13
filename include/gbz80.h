#pragma once
#include <stdint.h>
#include <iostream>
#include "mmu.h"
#include "DebugLogger/BaseLogger.h"


struct opFlag {
	uint8_t operand1;
	uint8_t operand2;
};

class gbz80{
	public:
		gbz80(MMU* memory);
		gbz80(MMU* memory, BaseLogger* log);

		void loadGame();

		int advanceCPU();
		int processInterrupt();

		//getters
		bool getHalt();
		int getPC();

		uint32_t saveToFile(const char* filename, const uint32_t offset);
		uint32_t loadFromFile(const char* filename, const uint32_t offset);

	private:
		enum flags { NZero, Zero, NCarry, Carry };

		//Instructions
		void ldReg8(uint8_t& dest, const uint8_t src);
		void ldReg16(uint16_t& dest, const uint16_t src);

		void ldHL();

		void ldMem(uint8_t& dest, const uint16_t src);
		void wrMem(const uint16_t dest, const uint8_t src);

		void push(const uint16_t src);
		void pop(uint16_t& dest);

		void addReg8(const uint8_t src);
		void addCReg8(const uint8_t src);

		void subReg8(const uint8_t src);
		void subCReg8(const uint8_t src);

		void andReg8(const uint8_t src);
		void orReg8 (const uint8_t src);
		void xorReg8(const uint8_t src);

		void cpReg8(const uint8_t src);

		void incReg8(uint8_t& src);
		void decReg8(uint8_t& src);

		void incMem8(const uint16_t src);
		void decMem8(const uint16_t src);

		void addHL(const uint16_t src);
		void addSP();

		void incReg16(uint16_t& src);
		void decReg16(uint16_t& src);

		void swapReg8(uint8_t& src);
		void DAA();

		void CPL();

		void CCF();
		void SCF();

		void HALT();
		void STOP();

		void DI();
		void EI();

		void RLCA(uint8_t & src);

		void RLA(uint8_t & src);

		void RLC(uint8_t& src);
		void RL(uint8_t& src);

		void RRC(uint8_t& src);
		void RR(uint8_t& src);

		void RRCA(uint8_t & src);

		void RRA(uint8_t & src);

		void SL(uint8_t& src);
		void SRA(uint8_t& src);
		void SRL(uint8_t& src);

		void BIT(const uint8_t src, const uint8_t bit);
		void SET(uint8_t& src, const uint8_t bit);
		void RES(uint8_t& src, const uint8_t bit);

		void JP();
		void JPC(const flags flag);
		void JPHL();
		void JR();
		void JRC(const flags flag);

		void CALL();
		void CALLC(const flags flag);

		void RST(const uint8_t n);

		void RET();
		void RETC(const flags flag);
		void RETI();

		int extendedOP();

		inline uint8_t readZeroFlag(const uint8_t flag)			{ return (flag&0x80) != 0; }
		inline uint8_t readSubtractFlag(const uint8_t flag)		{ return (flag&0x40) != 0; }
		inline uint8_t readHalfCarryFlag(const uint8_t flag)	{ return (flag&0x20) != 0; }
		inline uint8_t readCarryFlag(const uint8_t flag)		{ return (flag&0x10) != 0; }

		//Variables
		bool	enableInterrupt		=		false;

		bool	disableNext			=		false;
		bool	enableNext			=		false;

		bool	m_halt				=		false;
		bool	m_stop				=		false;

		regset reg					=		{};
		uint8_t opcodeIndex;
		uint8_t m_opcodeCycle;

		MMU* m_memory;

		BaseLogger* m_log			=		nullptr;	

		//Const
		static const uint16_t zeroFlag		= 0x80;
		static const uint16_t subtractFlag	= 0x40;
		static const uint16_t halfCarryFlag = 0x20;
		static const uint16_t carryFlag		= 0x10;
};



