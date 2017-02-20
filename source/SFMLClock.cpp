#include "SFMLClock.h"

const double Clock::GBFREQ = 4194304.0;
const double Clock::PPUFRAMECYCLE = 70224.0;
const double Clock::GBFRAMETIME = (PPUFRAMECYCLE/GBFREQ)*1000000.0;

Clock::Clock()
	:_clock()
{
	_startTime = _clock.getElapsedTime();
	_frameStart = _clock.getElapsedTime();
	_frameInterval = sf::microseconds((uint32_t)GBFRAMETIME);
	_sleepAmmount = sf::microseconds((uint32_t)(_frameInterval.asMicroseconds()/4));
}

void Clock::changeSpeed(const int speed) 
{
	switch (speed) {
		case 0:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME*4));
			break;
		case 1:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME*2));
			break;
		case 2:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/.75));
			break;
		case 3:
			_frameInterval = sf::microseconds((uint32_t)GBFRAMETIME);
			break;
		case 4:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/1.5));
			break;
		case 5:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/2));
			break;
		case 6:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/3));
			break;
		case 7:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/4));
			break;
		case 8: 
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/5));
			break;
		case 9:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/7.5));
			break;
		case 10:
			_frameInterval = sf::microseconds((uint32_t)(GBFRAMETIME/10));
			break;
		default:
			_frameInterval = sf::microseconds((uint32_t)GBFRAMETIME);
			break;

			_sleepAmmount = sf::microseconds((uint32_t)(_frameInterval.asMicroseconds()/4));
	}
}

bool Clock::stepFrame()
{
	bool re = false;

	if (_performance)
		re = stepFramePerformance();
	else
		re = stepFrameAccuracy();
	
	return re;
}

bool Clock::stepFrameAccuracy()
{
	bool re = false;
	_endTime = _clock.getElapsedTime();

	if ( (_startTime + _frameInterval) < _endTime ) {
		_startTime = _endTime;
		re = true;
	}

	return re;
}

bool Clock::stepFramePerformance()
{
	bool re = false;
	_endTime = _clock.getElapsedTime();

	if ( (_startTime + _frameInterval) < _endTime ) {
		_startTime = _endTime;
		re = true;
	}
	else
		sf::sleep(_sleepAmmount);

	return re;
}

bool Clock::checkFrameTime()
{
	bool re = false;

	++_frameCount;
	while ( (_frameStart + sf::microseconds((uint32_t)GBFRAMETIME)) < _endTime ) {

		_FPS = _frameCount / (double)((_endTime - _frameStart).asSeconds());
		
		_frameStart = _endTime;
		_frameCount = 0;

		re = true;
	}

	return re;
}

double Clock::getFPS()
{
	return _FPS;
}

void Clock::setPerformance(bool performance)
{
	_performance = performance;
}

bool Clock::getPerformance()
{
	return _performance;
}
