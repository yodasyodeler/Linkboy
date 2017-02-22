#include "SFMLSound.h"

Sound::Sound(uint32_t samplerate)
	:_samplerate(samplerate)
{
	m_ch1_sound.setBuffer(m_ch1_buffer);
	m_ch1_sound.setLoop(false);
	m_ch2_sound.setBuffer(m_ch2_buffer);
	m_ch2_sound.setLoop(false);
	m_ch3_sound.setBuffer(m_ch3_buffer);
	m_ch3_sound.setLoop(false);
	m_ch4_sound.setBuffer(m_ch4_buffer);
	m_ch4_sound.setLoop(false);
}

Sound::~Sound()
{}

void Sound::playChannel1(const int16_t* raw, const uint32_t samples)
{
	m_ch1_buffer.loadFromSamples(raw, samples, 1, _samplerate);
	m_ch1_sound.play();
}

void Sound::playChannel2(const int16_t* raw, const uint32_t samples)
{
	m_ch2_buffer.loadFromSamples(raw, samples, 1, _samplerate);
	m_ch2_sound.play();
}

void Sound::playChannel3(const int16_t* raw, const uint32_t samples)
{
	m_ch3_buffer.loadFromSamples(raw, samples, 1, _samplerate);
	m_ch3_sound.play();
}
void Sound::playChannel4(const int16_t* raw, const uint32_t samples)
{
	m_ch4_buffer.loadFromSamples(raw, samples, 1, _samplerate);
	m_ch4_sound.play();
}