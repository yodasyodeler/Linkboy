#include "apu.h"

const uint8_t APU::Duty[4][8] = {
	0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,1,1,1,1,
	0,1,1,1,1,1,1,0
};

const uint8_t APU::Ch4Div[8] = {
	 8, 16, 32, 48,
	64, 80, 96, 112
};

APU::APU(MMU* memory)
	:m_memory(memory)
{}

APU::~APU()
{}

void APU::loadGame()
{
	m_framesync_cycle = 0;
	m_framesync_count = 0;

	m_sample_count			= CyclesPerSample;
	m_sample_index			= 0;

	m_ch1_on				= false;
	m_ch1_duty_index		= 0;
	m_ch1_sweep_count		= 0;
	m_ch1_sweep_en			= false;
	m_ch1_sweep_dir			= false;	// 0:Add / 1:Sub
	m_ch1_sweep_shadow		= 0;
	m_ch1_envelope_en		= false;
	m_ch1_envelope_count	= 0;
	m_ch1_count				= 0;

	m_ch2_on				= false;
	m_ch2_duty_index		= 0;
	m_ch2_envelope_en		= false;
	m_ch2_envelope_count	= 0;
	m_ch2_count				= 0;

	m_ch3_position			= 0;
	m_ch3_count				= 0;

	m_ch4_on				= false;
	m_ch4_envelope_en		= false;
	m_ch4_envelope_count	= 0;
	m_ch4_count				= 0;
	m_ch4_lsfr				= 0;

	m_memory->mmIO[S_CONTROL] = 0;
}

void APU::changeSpeed(const int speed)
{
	//switch (speed) {
	//case 0:
	//	CyclesPerSample = (GBFreq / 4)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 1:
	//	CyclesPerSample = (GBFreq / 2)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 2:
	//	CyclesPerSample = (GBFreq / .75)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 3:
	//	CyclesPerSample = GBFreq/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 4:
	//	CyclesPerSample = (GBFreq * 1.5)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 5:
	//	CyclesPerSample = (GBFreq * 2)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 6:
	//	CyclesPerSample = (GBFreq * 3)/SAMPLERATE;
	//	//m_soundOn = true;
	//	break;
	//case 7:
	//	CyclesPerSample = (GBFreq * 4)/SAMPLERATE;
	//	//m_soundOn = false;
	//	break;
	//case 8:
	//	CyclesPerSample = (GBFreq * 5)/SAMPLERATE;
	//	//m_soundOn = false;
	//	break;
	//case 9:
	//	CyclesPerSample = (GBFreq * 7.5)/SAMPLERATE;
	//	//m_soundOn = false;
	//	break;
	//case 10:
	//	CyclesPerSample = (GBFreq * 10)/SAMPLERATE;
	//	//m_soundOn = false;
	//	break;
	//default:
	//	CyclesPerSample = GBFreq/SAMPLERATE;
	//	//m_soundOn = false;
	//	break;
	//}
}

void APU::advanceSound(const int cycle)
{
	int freq;
	int duty;
	int vol;

	//Turn On sound
	if ((m_memory->mmIO[S_CONTROL] & 0x80) && !m_sound_on) {
		m_sound_on = true;
		
	}
	//Turn Off Sound
	else if (!(m_memory->mmIO[S_CONTROL] & 0x80) && m_sound_on) {
		m_ch1_on = false;
		m_ch2_on = false;
		m_ch3_on = false;
		m_sound_on = false;

		m_framesync_cycle = 0;
		
		for (int i=0x10; i<0x30; ++i)
			m_memory->mmIO[i] = 0;
	}
	if (!m_sound_on) {
		for (int i=0x10; i<0x27; ++i)
			m_memory->mmIO[i] = 0;
	}

	//Record value on buffer
	if (m_sound_on) {
		//Channel 1 - Square
		m_ch1_count -= cycle;
		while (m_ch1_count <= 0) {
			freq = ((m_memory->mmIO[SM1_FREQ_HI] & 7) << 8) | m_memory->mmIO[SM1_FREQ_LO];

			m_ch1_count += (2048 - freq) << 2;
			m_ch1_duty_index = (m_ch1_duty_index + 1) & 7;
		}

		//Channel 2 -Square
		m_ch2_count -= cycle;
		while (m_ch2_count <= 0) {
			freq = ((m_memory->mmIO[SM2_FREQ_HI] & 7) << 8) | m_memory->mmIO[SM2_FREQ_LO];

			m_ch2_count += (2048 - freq) << 2;
			m_ch2_duty_index = (m_ch2_duty_index + 1) & 7;
		}

		//Channel 3 - Wave
		m_ch3_count -= cycle;
		while (m_ch3_count <= 0) {
			freq = ((m_memory->mmIO[SM3_FREQ_HI] & 7) << 8) | m_memory->mmIO[SM3_FREQ_LO];

			m_ch3_count += (2048 - freq) << 2;
			m_ch3_position = (m_ch3_position + 1) & 0x1F;
		}

		//Channel 4 - Noise
		m_ch4_count -= cycle;
		while (m_ch4_count <= 0) {
			freq = m_memory->mmIO[SM4_POLY];
			freq = Ch4Div[freq&0x7] << ((freq>>4) & 0xF);

			m_ch4_count += freq;
			duty = (((m_ch4_lsfr>>1) ^ m_ch4_lsfr) & 1);
			m_ch4_lsfr = (m_ch4_lsfr>>1) | (duty<<14);
			if (freq & 8)
				m_ch4_lsfr = (duty<<6) | (m_ch4_lsfr&0xFB);
		}

		m_sample_count -= cycle;
		while (m_sample_count <= 0) {
			uint8_t soundControl = m_memory->mmIO[S_CONTROL];
			m_sample_count += CyclesPerSample;


			if (soundControl & 0x80) {
				//Channel 1 - Square
				if (m_ch1_on) {
					duty = (m_memory->mmIO[SM1_LENGTH] >> 6) & 3;
					vol = (m_memory->mmIO[SM1_ENVELOPE] >> 4);
					m_ch1_raw[m_sample_index] = vol*Duty[duty][m_ch1_duty_index] * AMPLITUDE;
				}
				else
					m_ch1_raw[m_sample_index] = 0;

				//Channel 2 - Square
				if (m_ch2_on) {
					duty = (m_memory->mmIO[SM2_LENGTH] >> 6) & 3;
					vol = (m_memory->mmIO[SM2_ENVELOPE] >> 4);
					m_ch2_raw[m_sample_index] = vol*Duty[duty][m_ch2_duty_index] * AMPLITUDE;
				}
				else
					m_ch2_raw[m_sample_index] = 0;

				//Channel 3 - Wave
				if (m_ch3_on) {
					duty = m_memory->mmIO[0x30 + (m_ch3_position >> 1)];
					vol = ((m_memory->mmIO[SM3_SELECT] >> 5) - 1) & 0x3;

					duty = ((duty >> (m_ch3_position & 1)) & 0xF) >> vol;

					m_ch3_raw[m_sample_index] = duty;
				}
				else
					m_ch3_raw[m_sample_index] = 0;

				//Channel 4 - Noise
				if (m_ch4_on) {
					vol = ((m_memory->mmIO[SM4_ENVELOPE]>>4)&0xF);
					m_ch4_raw[m_sample_index] = (m_ch4_lsfr & 1)*vol; 
				}
				else
					m_ch4_raw[m_sample_index] = 0;
			}

			m_sample_index += 1;
			//used to play sounds here, can be optimized
			if (m_sample_index >= SAMPLES) {
				m_sample_index = 0;
			}
		}
	}


	//FrameSync
	m_framesync_cycle += cycle;
	if (m_framesync_cycle > FramesyncPeriodCount) {

		checkTrigger();
		m_framesync_cycle -= FramesyncPeriodCount;
		switch (m_framesync_count) {
		case 0:
			advanceLength();
			break;
		case 2:
			advanceLength();
			advanceSweep();
			break;
		case 4:
			advanceLength();
			break;
		case 6:
			advanceLength();
			advanceSweep();
			break;
		case 7:
			advanceEnvelope();
			break;
		default:
			break;
		}
		m_framesync_count = (m_framesync_count + 1) & 7;
	}
}

const int16_t* APU::getBuffer(int n)
{
	int16_t* re = nullptr;
	switch (n) {
		case 0:
			re = m_ch1_raw;
			break;
		case 1:
			re = m_ch2_raw;
			break;
		case 2:
			re = m_ch3_raw;
			break;
		case 3:
			re = m_ch4_raw;
			break;
		default:
			break;
	}

	return re;
}

uint32_t APU::getNumSamples()
{
	return SAMPLES;
}

inline void APU::checkTrigger()
{
	uint8_t sweep;
	uint8_t length;
	uint8_t envelope;
	uint8_t lowFreq;
	uint8_t highFreq;

	//Channel 1 - Square
	highFreq = m_memory->mmIO[SM1_FREQ_HI];
	if (highFreq & 0x80) {
		sweep		= m_memory->mmIO[SM1_SWEEP];
		length		= m_memory->mmIO[SM1_LENGTH] & 0x3F;
		envelope	= m_memory->mmIO[SM1_ENVELOPE];
		lowFreq		= m_memory->mmIO[SM1_FREQ_LO];

		m_ch1_on				= true;
		m_ch1_duty_index		= 0;
		m_ch1_sweep_count		= (sweep>>4)&7;
		m_ch1_sweep_en			= (sweep&0x77) != 0;
		m_ch1_sweep_shadow		= ((highFreq&7)<<8) | lowFreq;
		m_ch1_envelope_en		= true;
		m_ch1_envelope_count	= envelope&0x7;
		m_ch1_count				= (2048 - m_ch1_sweep_shadow)<<2;
		
		if ((highFreq & 0x40) == 0 && length == 0) {
			m_memory->mmIO[SM1_LENGTH] |= 0x3F;
		}
		if ((sweep & 7) != 0 && (calculateSweep() > 2047)) {
			m_ch1_on = false;
			m_memory->mmIO[S_CONTROL] &= 0xFE;
		}
		else
			m_memory->mmIO[S_CONTROL] |= 0x01;

		m_memory->mmIO[SM1_FREQ_HI] = highFreq & 0x7F;
	}

	//Channel 2 - Square
	highFreq = m_memory->mmIO[SM2_FREQ_HI];
	if (highFreq & 0x80) {
		length		= m_memory->mmIO[SM2_LENGTH] & 0x3F;
		envelope	= m_memory->mmIO[SM2_ENVELOPE];
		lowFreq		= m_memory->mmIO[SM2_FREQ_LO];

		m_ch2_on				= true;
		m_ch2_envelope_en		= true;
		m_ch2_envelope_count	= envelope&0x7;
		m_ch2_count				= (2048 - (((highFreq&7)<<8) | lowFreq))<<2;
		m_memory->mmIO[SM2_FREQ_HI] = highFreq & 0x7F;
		m_memory->mmIO[S_CONTROL] |= 0x02;

		if ((highFreq & 0x40) == 0 && length == 0) {
			m_memory->mmIO[SM2_LENGTH] |= 0x3F;
		}
	}

	//Channel 3 - Wave
	highFreq = m_memory->mmIO[SM3_FREQ_HI];
	if (highFreq & 0x80) {
		length		= m_memory->mmIO[SM3_LENGTH];
		lowFreq		= m_memory->mmIO[SM3_FREQ_LO];

		m_ch3_on		= true;
		m_ch3_position	= 0;
		m_ch3_count		= (2048 - (((highFreq&7)<<8) | lowFreq))<<2;
		m_memory->mmIO[SM3_FREQ_HI] = highFreq & 0x7F;
		m_memory->mmIO[S_CONTROL] |= 0x04;
		if ((highFreq & 0x40) == 0 && length == 0)
			m_memory->mmIO[SM3_LENGTH] = 0xFF;
	}

	//Channel 4 - Noise
	highFreq = m_memory->mmIO[SM4_COUNTER];
	if (highFreq & 0x80) {
		length		= m_memory->mmIO[SM4_LENGTH] & 0x3F;
		envelope	= m_memory->mmIO[SM4_ENVELOPE];
		lowFreq		= m_memory->mmIO[SM4_POLY];

		m_ch4_on				= true;
		m_ch4_envelope_en		= true;
		m_ch4_envelope_count	= envelope&0x7;
		m_ch4_count				= Ch4Div[lowFreq&0x7] << ((lowFreq>>4) & 0xF);
		m_ch4_lsfr				= 0x7FFF;
		m_memory->mmIO[SM4_COUNTER] = highFreq & 0x7F;
		m_memory->mmIO[S_CONTROL] |= 0x08;

		if ((highFreq & 0x40) == 0 && length == 0) {
			m_memory->mmIO[SM4_LENGTH] |= 0x3F;
		}
	}
}

inline void APU::advanceSweep()
{
	uint8_t sweepReg;
	int load;
	int sweep;

	//Channel 1
	m_ch1_sweep_count = (m_ch1_sweep_count-1) & 0x7;
	if (m_ch1_sweep_count <= 0) {

		sweepReg	= m_memory->mmIO[SM1_SWEEP];
		load		= (sweepReg>>4)&7;
		sweep		= (sweepReg&7);

		m_ch1_sweep_count = load;
		if (m_ch1_sweep_en && load) {

			int newFreq = calculateSweep();
			if (newFreq > 2047) {
				m_ch1_on = false;
				m_memory->mmIO[S_CONTROL] &= 0xFE;
			}
			else if (sweep > 0) { 
				m_ch1_sweep_shadow = newFreq;
				m_memory->mmIO[SM1_FREQ_LO] = m_ch1_sweep_shadow & 0xFF;
				m_memory->mmIO[SM1_FREQ_HI] = ((m_ch1_sweep_shadow >> 8) & 0x7) | (m_memory->mmIO[SM1_FREQ_HI]&0xF8);
			}

			if (calculateSweep() > 2047) {
				m_ch1_on = false;
				m_memory->mmIO[S_CONTROL] &= 0xFE;
			}
		}
	}
}

inline void APU::advanceLength()
{
	int length;
	int reload; 

	//Channel 1 - Square
	length		= (m_memory->mmIO[SM1_LENGTH]  & 0x3F);
	reload		= (m_memory->mmIO[SM1_FREQ_HI] & 0x40);
	if (reload) {
		length = (length + 1);
		if (length >= 64) {
			m_ch1_on = false;
			m_memory->mmIO[S_CONTROL] &= 0xFE;
		}
		m_memory->mmIO[SM1_LENGTH] = (m_memory->mmIO[SM1_LENGTH]&0xC0) | (length & 0x3F);
	}
	
	//Channel 2 - Square
	length		= (m_memory->mmIO[SM2_LENGTH]  & 0x3F);
	reload		= (m_memory->mmIO[SM2_FREQ_HI] & 0x40);
	if (reload) {
		length = (length + 1);
		if (length >= 64 ) {
			m_ch2_on = false;
			m_memory->mmIO[S_CONTROL] &= 0xFD;
		}
		m_memory->mmIO[SM2_LENGTH] = (m_memory->mmIO[SM2_LENGTH]&0xC0) | (length & 0x3F);
	}

	//Channel 3 - Wave
	length		= (m_memory->mmIO[SM3_LENGTH]);
	reload		= (m_memory->mmIO[SM3_FREQ_HI] & 0x40);
	if (reload) {
		length = (length + 1);
		if (length >= 256 ) {
			m_ch3_on = false;
			m_memory->mmIO[S_CONTROL] &= 0xFB;
		}
		m_memory->mmIO[SM3_LENGTH] = (length & 0xFF);
	}
	
	//Channel 4 - Noise
	length = (m_memory->mmIO[SM4_LENGTH] & 0x3F);
	reload = (m_memory->mmIO[SM4_COUNTER] & 0x40);
	if (reload) {
		length = (length + 1);
		if (length >= 64) {
			m_ch4_on = false;
			m_memory->mmIO[S_CONTROL] &= 0xF7;
		}
		m_memory->mmIO[SM4_LENGTH] = (length & 0x3F);
	}
}

inline void APU::advanceEnvelope()
{
	int envelopePeriod;
	int envelopeDir;
	int envelopeVol;

	//Channel 1 - Square
	if (m_ch1_envelope_en) {
		m_ch1_envelope_count = (m_ch1_envelope_count - 1) & 0x7;
		if (m_ch1_envelope_count <= 0) {

			envelopePeriod = m_memory->mmIO[SM1_ENVELOPE] & 7;
			envelopeDir = m_memory->mmIO[SM1_ENVELOPE] & 8;
			envelopeVol = m_memory->mmIO[SM1_ENVELOPE] >> 4;

			m_ch1_envelope_count = envelopePeriod;
			if (envelopePeriod) {
				if (envelopeDir && envelopeVol < 15) {
					m_memory->mmIO[SM1_ENVELOPE] += 0x10;
				}
				else if (!envelopeDir && envelopeVol > 0) {
					m_memory->mmIO[SM1_ENVELOPE] -= 0x10;
				}
				else {
					m_ch1_envelope_en = false;
				}
			}
		}
	}

	//Channel 2 - Square
	if (m_ch2_envelope_en) {
		m_ch2_envelope_count = (m_ch2_envelope_count - 1) & 0x7;
		if (m_ch2_envelope_count <= 0) {

			envelopePeriod = m_memory->mmIO[SM2_ENVELOPE] & 7;
			envelopeDir = m_memory->mmIO[SM2_ENVELOPE] & 8;
			envelopeVol = m_memory->mmIO[SM2_ENVELOPE] >> 4;

			m_ch2_envelope_count = envelopePeriod;
			if (envelopePeriod) {
				if (envelopeDir && envelopeVol < 15) {
					m_memory->mmIO[SM2_ENVELOPE] += 0x10;
				}
				else if (!envelopeDir && envelopeVol > 0) {
					m_memory->mmIO[SM2_ENVELOPE] -= 0x10;
				}
				else {
					m_ch2_envelope_en = false;
				}
			}
		}
	}

	//Channel 4 - Noise
	if (m_ch4_envelope_en) {
		m_ch4_envelope_count = (m_ch4_envelope_count - 1) & 0x7;
		if (m_ch4_envelope_count <= 0) {
			envelopePeriod = m_memory->mmIO[SM4_ENVELOPE] & 7;
			envelopeDir = m_memory->mmIO[SM4_ENVELOPE] & 8;
			envelopeVol = m_memory->mmIO[SM4_ENVELOPE] >> 4;

			m_ch4_envelope_count = envelopePeriod;
			if (envelopePeriod) {
				if (envelopeDir && envelopeVol < 15) {
					m_memory->mmIO[SM4_ENVELOPE] += 0x10;
				}
				else if (!envelopeDir && envelopeVol > 0) {
					m_memory->mmIO[SM4_ENVELOPE] -= 0x10;
				}
				else {
					m_ch4_envelope_en = false;
				}
			}
		}
	}
}

int APU::calculateSweep()
{
	int sweep		=	m_memory->mmIO[SM1_SWEEP];

	int freqShift	=	m_ch1_sweep_shadow>>(sweep&7);
	int dir			=	(sweep&8) != 0;
	
	if (dir) {
			freqShift *= -1;
	}

	//look up subtrahand

	return m_ch1_sweep_shadow + freqShift;
}
