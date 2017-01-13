#include "timer.h"

const int timer::frequencies[4] { 1024, 16, 64, 256 };

timer::timer(MMU* memory)
	:m_memory(memory)
{}

timer::~timer()
{}

void timer::advanceTimer( const int cycle )
{
	uint8_t timerControl = m_memory->mmIO[TIMA_CONTROL];
	
	//------Serial------
	if (m_serialActive == false) {
		//Serial on and Internal Clk
			m_serialCount		= 0;
		if (m_memory->mmIO[SERIAL_CONTROL] & 0x80 && m_memory->mmIO[SERIAL_CONTROL] & 0x1) { 
			m_serialActive		= true;
			m_messageOut		= true;
		}
	}
	else {
		m_serialCount += cycle;
		//Internal Time Out
		if (m_serialCount > 50000) {
			m_memory->mmIO[IF]				|= 0x8;
			m_memory->mmIO[SERIAL_CONTROL]	&= ~0x80;

			//Read Data - Internal Clock | Will break if not connected
			//m_memory->mmIO[SERIAL_DATA]		= 0xFF;			//Dummy Read

			m_serialActive					= false;
		}
	}
	if (m_memory->mmIO[SERIAL_CONTROL] == 0x80) {
		m_serialInCount += cycle;
		if (m_serialInCount > 5000) {
			m_serialInCount = 0;
			m_messageIn = true;
		}
	}

	//------Timer------
	m_divCount += cycle;
	if (m_divCount >= divInterval) {
		++m_memory->mmIO[DIVIDER];
		m_divCount	-= divInterval;
	}
	

	if (timerControl & 0x4) {
		m_timerCount += cycle;
		while (m_timerCount >= frequencies[timerControl & 0x3]) {
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

void timer::loadGame()
{
	m_divCount			= 0;
	m_timerCount		= 0;



	m_serialCount = 0;

	m_serialInCount = 0;

	//Should this be here?
	m_serialActive	= false;
	m_messageOut = false;
	m_messageIn = false;
	m_memory->mmIO[TIMA_CONTROL] = 0;
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



bool timer::getMessageOut()
{
	return m_messageOut;
}

void timer::clrMessageOut()
{
	m_messageOut = false;
}

bool timer::getMessageIn()
{
	return (m_memory->mmIO[SERIAL_CONTROL] == 0x80) && m_messageIn;
}

//Double Check behavior of serial - external clock	
void timer::clrMessageIn()
{
	m_messageIn = false;
}

void timer::clrSerialIn()
{
	m_memory->mmIO[IF]	|= 0x8;
	m_memory->mmIO[SERIAL_CONTROL]	&= ~0x80;
	m_serialActive		= false;
}
