#pragma once
#include <SFML/Audio.hpp>
#include <stdint.h>

class Sound {

	public:
		Sound(uint32_t samplerate);
		~Sound();

		void playChannel1(const int16_t* raw, const uint32_t samples);
		void playChannel2(const int16_t* raw, const uint32_t samples);
		void playChannel3(const int16_t* raw, const uint32_t samples);
		void playChannel4(const int16_t* raw, const uint32_t samples);

	private:
		uint32_t _samplerate;


		sf::SoundBuffer m_ch1_buffer;
		sf::Sound		m_ch1_sound;

		sf::SoundBuffer m_ch2_buffer;
		sf::Sound		m_ch2_sound;

		sf::SoundBuffer m_ch3_buffer;
		sf::Sound		m_ch3_sound;

		sf::SoundBuffer m_ch4_buffer;
		sf::Sound		m_ch4_sound;
};