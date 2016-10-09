#include "linkboy.h"

linkboy::linkboy(const char* dirName, const char* filename)
	:m_memory(filename),   m_cpu(&m_memory),  
	 m_display(&m_memory), m_timer(&m_memory)
{
	loadGame(filename);
	initScreen(160*2, 144*2, m_display.getBuffer(), dirName);
}

linkboy::linkboy(BaseLogger* log, const char* dirName, const char* filename)
	:m_memory(filename), m_cpu(&m_memory, log),
	m_display(&m_memory), m_timer(&m_memory)
{
	loadGame(filename);
	initScreen(160*2, 144*2, m_display.getBuffer(), dirName);
}

linkboy::~linkboy()
{
	delete [] m_saveState;
	m_saveState = nullptr;
}

void linkboy::startEmulation()
{
	int		CPUCycle	=	0;
	bool	quit		=	false;
	char	buffer[100]	= {};

	while (!quit) {
		quit = handleEvents(m_settings);

		m_timer.changeSpeed(m_settings.speed);
		m_display.changeColor(m_settings.color);

		if (m_settings.loadGameFile != nullptr) {
			loadGame(m_settings.loadGameFile);
			m_settings.loadGameFile = nullptr;
		}

#ifdef DEBUG
		//set breakpoint
		if (breakpoint == -2) {
			do {
				std::cout << "\nNew Breakpoint: ";
				std::cin  >> buffer;
			} while (!convertString(breakpoint, buffer, 16));
		}
#endif

		if (m_settings.saveState == 1)
			loadState();
		if (m_settings.saveState == 2)
			saveState();


		//CPU Step
		if ( !m_settings.pause && m_timer.advanceCPU()) 
		{
			do {
				checkDebug();

				m_timer.advanceTimer(CPUCycle);
				//Sound
				m_display.advanceState(CPUCycle);

				if (!m_cpu.getHalt())
					CPUCycle = m_cpu.advanceCPU();
				else 
					CPUCycle = 1;
				
				m_memory.checkJoyPadPress();
				m_cpu.processInterrupt();

			} while ( !m_display.isVBlank() && !m_settings.pause);
			
			if ( !m_display.isDisplayOn() )
				m_display.clearDisplay();

			//max renders to 60fps
			if (m_timer.displayFrame()) {
				renderScreen();
				m_settings.framesPerSecond = m_timer.getFPS();
			}
		}
	}
}

void linkboy::saveState()
{
	uint32_t offset;

	m_settings.saveState = 0;

	if (m_saveState != nullptr) {
		offset = m_memory.saveToFile(m_saveState, 0);
		offset = m_cpu.saveToFile(m_saveState, offset);
		m_timer.saveToFile(m_saveState, offset);
	}
}

void linkboy::loadState()
{
	uint32_t offset;

	m_settings.saveState = 0;

	if (m_saveState != nullptr) {
		std::ifstream file(m_saveState, ios::in);

		if (file.is_open()) {
			file.close();

			offset = m_memory.loadFromFile(m_saveState, 0);
			offset = m_cpu.loadFromFile(m_saveState, offset);
			m_timer.loadFromFile(m_saveState, offset);
		}
		else
			std::cout << "No save state detected" << std::endl;
	}
}

void linkboy::checkDebug()
{
#ifdef DEBUG
	//if breakpoint hit
	if (m_cpu.getPC() == breakpoint) {
		buffer[0] = '\0';
		std::cout << "Breakpoint hit: " << std::hex << m_cpu.getPC();

		while (buffer[0] != 'q' && buffer[0] != 'r' && buffer[0] != '*') {
			std::cout << "\ncmd: ";
			std::cin  >> buffer; 

			switch (buffer[0]) {
			case 'h':
				std::cout << "\tq: end Breakpoint\n\tr: run\n\tb: new breakpoint\n\t*: exit program\n\n";
				break;
			case 'q':
				breakpoint = -1;
				break;
			case 'r':
				break;
			case 'b':
				do {
					std::cout << "\nNew Breakpoint: ";
					std::cin  >> buffer;
				} while (!convertString(breakpoint, buffer, 16));
				break;
			case '*':
				m_settings.pause = true;
				quit = true;
				break;
			default:
				break;
			}
		}
	}
#endif
}

void linkboy::loadGame(const char * filename)
{
	if (filename != nullptr) {
		int len = strlen(filename);

		delete [] m_saveState;
		m_saveState = new char[len + 4];

		strcpy_s(m_saveState, len + 1, filename);

		m_saveState[len - 2] = 's';
		m_saveState[len - 1] = 't';
		m_saveState[len]	 = 'a';
		m_saveState[len + 1] = 't';
		m_saveState[len + 2] = 'e';
		m_saveState[len + 3] = '\0';

		m_memory.loadGame(filename);
		m_cpu.loadGame();
	}
}
