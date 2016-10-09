#include "timer.h"


const double timer::gbperiod = (1000000 / 59.7);

const int timer::frequencies[4] {
	gbFrequency / 4096,
	gbFrequency / 262144,
	gbFrequency / 65536,
	gbFrequency / 16384,
};

#ifdef USING_SFML_TIME
timer::timer(MMU* memory)
	:m_memory(memory), m_clock()
{
	frameTime		= m_clock.getElapsedTime();
	m_frameInterval = sf::microseconds((uint32_t)gbperiod);
}

bool timer::advanceCPU()
{
	bool re = false;

	endTime = m_clock.getElapsedTime();
	if ( (cpuTime + m_frameInterval) < endTime ) {
		re = true;
		cpuTime = endTime;
	}
	return re;
}

bool timer::displayFrame()
{
	bool re = false;

	++m_frameCount;
	if ( (frameTime + sf::microseconds((uint32_t)gbperiod)) < endTime ) {

		m_framePerSecond = m_frameCount / (double)((endTime - frameTime).asSeconds());

		m_frameCount = 0;

		re = true;
		frameTime = endTime;
	}
	return re;
}
#endif

#ifdef USING_SDL_TIME
bool timer::advanceFrame()
{
	bool re = false;

	endTime = SDL_GetPerformanceCounter();
	if ((frameTime + frameInterval) < endTime) {
		re = true;
		frameTime = endTime;
	}
	return re;
}
#endif

timer::~timer()
{}

void timer::changeSpeed(const int speed)
{
	switch (speed) {
		case 0:
			m_frameInterval	= sf::microseconds((uint32_t)(gbperiod*4));
			break;
		case 1:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod*2));
			break;
		case 2:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/.75));
			break;
		case 3:
			m_frameInterval = sf::microseconds((uint32_t)gbperiod);
			break;
		case 4:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/1.5));
			break;
		case 5:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/2));
			break;
		case 6:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/3));
			break;
		case 7:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/4));
			break;
		case 8: 
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/5));
			break;
		case 9:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/7.5));
			break;
		case 10:
			m_frameInterval = sf::microseconds((uint32_t)(gbperiod/10));
			break;
		default:
			m_frameInterval = sf::microseconds((uint32_t)gbperiod);
			break;
	}

}

void timer::advanceTimer( const int cycle )
{
	uint8_t timerControl = m_memory->mmIO[TIMA_CONTROL];
	 
	if (m_serialActive == false) {
		if (m_memory->mmIO[SERIAL_CONTROL] & 0x80 && m_memory->mmIO[SERIAL_CONTROL] & 0x1) {
			m_serialActive		= true;
			m_serialCount		= 0;
		}
	}
	else {
		m_serialCount += cycle;
		if (m_serialCount > serialInterval) {
			m_memory->mmIO[IF]				|= 0x8;
			m_memory->mmIO[SERIAL_CONTROL]	&= ~0x80;

			m_memory->mmIO[SERIAL_DATA]		= 0xFF;			//Dummy Read

			m_serialActive					= false;
		}
	}

	m_divCount += cycle;
	if (m_divCount >= divInterval) {
		++m_memory->mmIO[DIVIDER];
		m_divCount	-= divInterval;
	}
	

	if (timerControl & 0x4) {
		m_timerCount += cycle;
		if (m_timerCount > frequencies[timerControl & 0x3]) {
			if ((++m_memory->mmIO[TIMA_COUNT]) == 0) {
				m_memory->mmIO[TIMA_COUNT]	= m_memory->mmIO[TIMA_RELOAD];
				m_memory->mmIO[IF]			|= (1 << 2);
			}
			m_timerCount -= frequencies[timerControl & 0x3];
		}
	}
	else
		m_timerCount = 0;
}

uint32_t timer::saveToFile(const char * filename, const uint32_t offset)
{
	std::ofstream file(filename, ios::app | ios::binary);

	if (file.is_open()) {
		file.seekp(offset, ios::beg);

		file.write(reinterpret_cast<char*>(&m_divCount), sizeof(int));
		file.write(reinterpret_cast<char*>(&m_timerCount), sizeof(int));
		file.write(reinterpret_cast<char*>(&m_serialCount), sizeof(int));

		file.write(reinterpret_cast<char*>(&m_serialActive), sizeof(bool));

		file.close();
	}

	return offset + sizeof(bool)*1 + sizeof(int)*3;
}

uint32_t timer::loadFromFile(const char * filename, const uint32_t offset)
{
	std::ifstream file(filename, ios::in | ios::binary);

	if (file.is_open()) {
		file.seekg(offset, ios::beg);

		file.read(reinterpret_cast<char*>(&m_divCount), sizeof(int));
		file.read(reinterpret_cast<char*>(&m_timerCount), sizeof(int));
		file.read(reinterpret_cast<char*>(&m_serialCount), sizeof(int));

		file.read(reinterpret_cast<char*>(&m_serialActive), sizeof(bool));

		file.close();
	}

	return offset + sizeof(bool) + sizeof(int)*3;
}

double timer::getFPS()
{
	return m_framePerSecond;
}
