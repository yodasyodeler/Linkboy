#include "linkboy.h"

bool expectingMessage = false;

const double linkboy::gbperiod = (1000000 / 59.7);


linkboy::linkboy( const char* dirName, const char* filename, BaseLogger* log)
	:_memory(filename), _cpu(&_memory, log), _display(&_memory), 
	_sound(&_memory), _timer(&_memory), _clock()
{
	loadGame(filename);
	initScreen(160*2, 144*2, _display.getBuffer(), dirName);
}

linkboy::~linkboy()
{
	delete [] _saveState;
	_saveState = nullptr;
}

void linkboy::startEmulation()
{
	int		CPUCycle	=	0;
	bool	quit		=	false;

	while (!quit) {
		if ( _clock.stepFrame()) {
			
			quit = handleEvents(_settings);
			handleSettings();

			if ( !_settings.pause) {
				do {
					//checkDebug();

					_timer.advanceTimer(CPUCycle);
					_sound.advanceSound(CPUCycle);
					_display.advanceState(CPUCycle);
					//link cable
					handleNetwork();

					if (!_cpu.getHalt())
						CPUCycle = (_cpu.advanceCPU()*4);
					else 
						CPUCycle = 1;
					
					_memory.checkJoyPadPress();
					CPUCycle += (_cpu.processInterrupt()*4);

				} while ( !_display.isVBlank() && !_settings.pause);
				
				
				//render 60 times a sec
				if (_clock.checkFrameTime()) {
					_display.updateColor();
					renderScreen();
					_settings.fps = _clock.getFPS();
				}
			}
		}
	}
}

void linkboy::handleSettings()
{
	switch (_settings.operation) {
		case NoOperation:
			break;
		case LoadGame:
			loadGame(_settings.loadGameFile);
			break;
		case ChangeColor:
			_display.changeColor(_settings.color);
			_display.updateColor();
			renderScreen();		
			break;
		case ChangeSpeed:
			if (_client.getConnected() == false) {
				_clock.changeSpeed(_settings.speed);
				//_sound.changeSpeed(_settings.speed); //currently no effect
			}
			break;
		case SaveGameState:
			saveState();			
			break;
		case LoadGameState:
			loadState();			
			break;
		case ConnectToServer:
			if (_client.getConnected() == false) {
				if (_client.joinServer(_settings.network.ip, _settings.network.port, _settings.network.name) ) {
					_clock.changeSpeed(3);
					_sound.changeSpeed(3);
					//_client.readLobby(0);
					if (_client.joinLobby(0) == false) 
						_client.createLobby();
				}
			} 
			break;
		case ReadLobby:
			if (_client.getConnected() == true) {
				for (int i=0; i<0; ++i)
					_settings.lobby.lobbyName[i] = _client.readLobby(i);
				
			}
			break;
		case JoinLobby:
			if (_client.getConnected() == true) {

			}
			break;
		default:
			break;
	}

}

void linkboy::saveState()
{
	uint32_t offset;
	
	if (_saveState != nullptr) {
		std::ifstream file(_saveState, ios::out | ios::trunc);

		if (file.is_open()) {
			file.close();
			offset = _memory.saveToFile(_saveState, 0);
			offset = _cpu.saveToFile(_saveState, offset);
			_timer.saveToFile(_saveState, offset);
		}
	}
}

void linkboy::loadState()
{
	uint32_t offset;

	if (_saveState != nullptr) {
		std::ifstream file(_saveState, ios::in);

		if (file.is_open()) {
			file.close();

			offset = _memory.loadFromFile(_saveState, 0);
			offset = _cpu.loadFromFile(_saveState, offset);
			_timer.loadFromFile(_saveState, offset);
		}
		else
			std::cout << "No save state detected" << std::endl;
	}
}

void linkboy::checkDebug()
{
	#ifdef DEBUG
		//if breakpoint hit
		if (_cpu.getPC() == breakpoint) {
			buffer[0] = '\0';
			std::cout << "Breakpoint hit: " << std::hex << _cpu.getPC();

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
					_settings.pause = true;
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

		delete [] _saveState;
		_saveState = new char[len + 4];

		strcpy_s(_saveState, len + 4, filename);

		_saveState[len - 2] = 's';
		_saveState[len - 1] = 't';
		_saveState[len]	 = 'a';
		_saveState[len + 1] = 't';
		_saveState[len + 2] = 'e';
		_saveState[len + 3] = '\0';

		_memory.loadGame(filename);
		_cpu.loadGame();
		_sound.loadGame();
		_timer.loadGame();
	}
}

void linkboy::handleNetwork()
{
	uint8_t gameMessage = _memory.rdByteMMU(0xFF01);

	//Check out going messages and send if at right time
	if (_client.getConnected()) {
		if (_timer.getMessageOut()) {
			_client.sendGameMessage(gameMessage);
			_timer.clrMessageOut();
			expectingMessage = true;
			//std::cout <<"Internal Clock: write -> " << std::hex << (uint32_t)gameMessage << "\n";
		}
		else if (expectingMessage && _client.readGameMessage(gameMessage) != -1) {
				_memory.wrByteMMU(0xFF01, gameMessage);
				//std::cout <<"Internal Clock: read -> " << std::hex << (uint32_t)gameMessage << "\n\n";
				expectingMessage = false;			
		}
		else if(_timer.getMessageIn() ) {
			if (_client.readGameMessage(gameMessage) != -1) {
				//std::cout << "\nExternal Clock: read  -> " << std::hex << (uint32_t)gameMessage << "\n";
				//std::cout << "External Clock: write -> " << std::hex << (uint32_t)(_memory.rdByteMMU(0xFF01)) << "\n\n";
				_client.sendGameMessage((_memory.rdByteMMU(0xFF01)));
				_memory.wrByteMMU(0xFF01, gameMessage);
				_timer.clrSerialIn();
			
			}
			_timer.clrMessageIn();
		}
	}
}
