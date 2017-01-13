#pragma
#include <iostream>
#include <stdint.h>
#include "mmu.h"

class SquareWave {
	public:
		SquareWave(MMU* memory);
		
		~SquareWave();

		void resetWave();

		void advanceWave(const int cycle);

		void evaluateTrigger();

		void advanceSweep();

		void advanceLength();

		void advanceEnvelope();

		int16_t* getSamples();

	private:
		MMU* 	m_memory			= nullptr;

		int 	m_mem_offset		= 0x10;

		bool 	m_on				= false;
		int		m_duty_index		= 0;
		bool 	m_sweep_en			= 0;
		int 	m_sweep_count		= 0;
		int 	m_sweep_shadow 		= 0;
		bool 	m_envelope_en		= 0;
		int 	m_envelope_count	= 0;
		int 	m_freq_count		= 0;	

		int16_t 	m_raw[SAMPLES] 	= {};
};

/******************************************************************************
  Sound Variables
	on:Sound is active
	duty_index: Wrappping index into an array that simulates the duty cycle	
	sweep_en: Set on a trigger if sweep reg has sweeps active.		!does it only set during trigger?
	sweep_count: Running count to determine when to change the frequency
	sweep_shadow: Calculations for sweep uses a shadow reg and therefore can ignore the freq reg
	envelope_en: Set on trigger, cleared once max or min vol is reached		!Is it set if sweep is off?
	envelope_count: Running count to determine when to change volume
	freq_count: Running count to grab a sample to construct a sound buffer ie raw
******************************************************************************/
