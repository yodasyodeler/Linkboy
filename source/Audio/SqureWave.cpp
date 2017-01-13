#include "SquareWave.h"

SquareWave::SquareWave(MMU* memory, bool sweep)
	:m_memory(memory)
{
	m_mem_offset = sweep ? 0x10 : 0x15;
}

SquareWave::~SquareWave()
{}

void SquareWave::resetWave()
{
	m_on				= false;
	m_duty_index		= 0;
	m_sweep_en			= 0;
	m_sweep_count		= 0;
	m_sweep_shadow 		= 0;
	m_envelope_en		= 0;
	m_envelope_count	= 0;
	m_freq_count		= 0;	
}

void SquareWave::advanceWave(const int cycle)
{
	uint8_t& freqHi = m_memory[m_mem_offset];
	


	m_count -= cycle;
	while (m_count <= 0) {
		freq = ((m_memory->mmIO[SM1_FREQ_HI] & 7) << 8) | m_memory->mmIO[SM1_FREQ_LO];

		m_ch1_count += (2048 - freq) << 2;
		m_ch1_duty_index = (m_ch1_duty_index + 1) & 7;
	}
}

void SquareWave::evaluateTrigger()
{
}

void SquareWave::advanceSweep()
{
}

void SquareWave::advanceLength()
{
}

void SquareWave::advanceEnvelope()
{
}

int16_t* getSamples()
{
	return m_raw;
}