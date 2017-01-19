#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include "mmu.h"
#include <stdint.h>

class APU {
	static const uint32_t SAMPLES = 1024*8;
	static const uint32_t SAMPLERATE = 44100;
	static const uint32_t AMPLITUDE = 250;
	static const int GBFreq = 4194304;
	static const int FramesyncPeriodCount = (GBFreq/512);
	static const uint8_t Duty[4][8]; 
	static const uint8_t Ch4Div[8];

	public:
		APU(MMU* memory);
		~APU();

		void loadGame();

		void changeSpeed(const int speed);

		void advanceSound(const int cycle);

	private:
		inline void checkTrigger();

		inline void advanceSweep();

		inline void advanceLength(); 

		inline void advanceEnvelope();

		int calculateSweep();
	
		int CyclesPerSample = (GBFreq/SAMPLERATE);

		MMU*	m_memory;

		bool		m_sound_on = false;

		int		m_framesync_cycle = 0;
		int		m_framesync_count = 0;

		uint32_t	m_sample_count			= CyclesPerSample;
		uint32_t	m_sample_index			= 0;

		bool	m_ch1_on				= false;
		int		m_ch1_duty_index		= 0;
		int		m_ch1_sweep_count		= 0;
		bool	m_ch1_sweep_en			= false;
		bool	m_ch1_sweep_dir			= false;	// 0:Add / 1:Sub
		int		m_ch1_sweep_shadow		= 0;
		bool	m_ch1_envelope_en		= false;
		int		m_ch1_envelope_count	= 0;
		int		m_ch1_count				= 0;

		bool	m_ch2_on				= false;
		int		m_ch2_duty_index		= 0;
		bool	m_ch2_envelope_en		= false;
		int		m_ch2_envelope_count	= 0;
		int		m_ch2_count				= 0;

		bool	m_ch3_on				= false;
		int		m_ch3_position			= 0;
		int		m_ch3_count				= 0;

		bool	m_ch4_on				= false;
		int		m_ch4_envelope_en		= false;
		int		m_ch4_envelope_count	= 0;
		int		m_ch4_count				= 0;
		int16_t m_ch4_lsfr				= 0;

		int16_t m_ch1_raw[SAMPLES] = {};
		int16_t m_ch2_raw[SAMPLES] = {};
		int16_t m_ch3_raw[SAMPLES] = {};
		int16_t m_ch4_raw[SAMPLES] = {};


		sf::SoundBuffer m_ch1_buffer;
		sf::Sound		m_ch1_sound;

		sf::SoundBuffer m_ch2_buffer;
		sf::Sound		m_ch2_sound;

		sf::SoundBuffer m_ch3_buffer;
		sf::Sound		m_ch3_sound;

		sf::SoundBuffer m_ch4_buffer;
		sf::Sound		m_ch4_sound;
};