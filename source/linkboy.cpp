#include "linkboy.h"

bool expectingMessage = false;

const double linkboy::gbperiod = (1000000 / 59.7);

linkboy::linkboy(const char* dirName, const char* filename)
	:m_memory(filename), m_cpu(&m_memory), m_display(&m_memory), 
	 m_sound(&m_memory), m_timer(&m_memory), m_clock(),
	m_frameInterval(sf::microseconds((uint32_t)gbperiod))
{
	frameTime		= m_clock.getElapsedTime();
	m_frameInterval = sf::microseconds((uint32_t)gbperiod);

	loadGame(filename);
	initScreen(160*2, 144*2, m_display.getBuffer(), dirName);
}

linkboy::linkboy(BaseLogger* log, const char* dirName, const char* filename)
	:m_memory(filename), m_cpu(&m_memory, log), m_display(&m_memory), 
	m_sound(&m_memory), m_timer(&m_memory), m_clock(),
	m_frameInterval(sf::microseconds((uint32_t)gbperiod))
{
	frameTime		= m_clock.getElapsedTime();
	


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

	while (!quit) {
		quit = handleEvents(m_settings);			//Handle Events
		handleSettings();

		//CPU Step
		if ( !m_settings.pause && checkTime()) 
		{
			do {
				//checkDebug();

				m_timer.advanceTimer(CPUCycle);
				m_sound.advanceSound(CPUCycle);
				m_display.advanceState(CPUCycle);
				//link cable
				handleNetwork();

				if (!m_cpu.getHalt())
					CPUCycle = (m_cpu.advanceCPU()*4);
				else 
					CPUCycle = 1;
				
				m_memory.checkJoyPadPress();
				CPUCycle += (m_cpu.processInterrupt()*4);

			} while ( !m_display.isVBlank() && !m_settings.pause);
			
			if ( m_display.isDisplayOn() == false )
				m_display.clearDisplay();

			//max renders to 60fps
			if (checkFrameTime()) {
				m_display.updateColor();
				renderScreen();
				m_settings.framesPerSecond = m_framePerSecond;
			}
		}
	}
}

void linkboy::handleSettings()
{
	switch (m_settings.operation) {
		case NoOperation:
			break;
		case LoadGame:
			loadGame(m_settings.loadGameFile);
			break;
		case ChangeColor:
			m_display.changeColor(m_settings.color);
			m_display.updateColor();
			renderScreen();		
			break;
		case ChangeSpeed:
			if (m_client.getConnected() == false) {
				changeSpeed(m_settings.speed);
				m_sound.changeSpeed(m_settings.speed);
			}
			break;
		case SaveGameState:
			saveState();			
			break;
		case LoadGameState:
			loadState();			
			break;
		case ConnectToServer:
			if (m_client.getConnected() == false) {
				if (m_client.joinServer(m_settings.network.ip, m_settings.network.port, m_settings.network.name) ) {
					changeSpeed(3);
					m_sound.changeSpeed(3);
					//m_client.readLobby(0);
					if (m_client.joinLobby(0) == false) 
						m_client.createLobby();
				}
			} 
			break;
		case ReadLobby:
			if (m_client.getConnected() == true) {
				for (int i=0; i<0; ++i)
					m_settings.lobby.lobbyName[i] = m_client.readLobby(i);
				
			}
			break;
		case JoinLobby:
			if (m_client.getConnected() == true) {

			}
			break;
		default:
			break;
	}

}

void linkboy::changeSpeed(const int speed)
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

bool linkboy::checkTime()
{
	bool re = false;

	endTime = m_clock.getElapsedTime();
	if ( (cpuTime + m_frameInterval) < endTime ) {
		re = true;
		cpuTime = endTime;
	}
	return re;
}

bool linkboy::checkFrameTime()
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

void linkboy::saveState()
{
	uint32_t offset;
	
	if (m_saveState != nullptr) {
		std::ifstream file(m_saveState, ios::out | ios::trunc);

		if (file.is_open()) {
			file.close();
			offset = m_memory.saveToFile(m_saveState, 0);
			offset = m_cpu.saveToFile(m_saveState, offset);
			m_timer.saveToFile(m_saveState, offset);
		}
	}
}

void linkboy::loadState()
{
	uint32_t offset;

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
		int len = lb_strlen(filename);

		delete [] m_saveState;
		m_saveState = new char[len + 4];

		lb_strcpy(m_saveState, filename);

		m_saveState[len - 2] = 's';
		m_saveState[len - 1] = 't';
		m_saveState[len]	 = 'a';
		m_saveState[len + 1] = 't';
		m_saveState[len + 2] = 'e';
		m_saveState[len + 3] = '\0';

		m_memory.loadGame(filename);
		m_cpu.loadGame();
		m_sound.loadGame();
		m_timer.loadGame();
	}
}

void linkboy::handleNetwork()
{
	uint8_t gameMessage = m_memory.rdByteMMU(0xFF01);

	//Check out going messages and send if at right time
	if (m_client.getConnected()) {
		if (m_timer.getMessageOut()) {
			m_client.sendGameMessage(gameMessage);
			m_timer.clrMessageOut();
			expectingMessage = true;
			//std::cout <<"Internal Clock: write -> " << std::hex << (uint32_t)gameMessage << "\n";
		}
		else if (expectingMessage && m_client.readGameMessage(gameMessage) != -1) {
				m_memory.wrByteMMU(0xFF01, gameMessage);
				//std::cout <<"Internal Clock: read -> " << std::hex << (uint32_t)gameMessage << "\n\n";
				expectingMessage = false;			
		}
		else if(m_timer.getMessageIn() ) {
			if (m_client.readGameMessage(gameMessage) != -1) {
				//std::cout << "\nExternal Clock: read  -> " << std::hex << (uint32_t)gameMessage << "\n";
				//std::cout << "External Clock: write -> " << std::hex << (uint32_t)(m_memory.rdByteMMU(0xFF01)) << "\n\n";
				m_client.sendGameMessage((m_memory.rdByteMMU(0xFF01)));
				m_memory.wrByteMMU(0xFF01, gameMessage);
				m_timer.clrSerialIn();
			
			}
			m_timer.clrMessageIn();
		}
	}
}
