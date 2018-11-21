#pragma once 

#include <chrono>
#include <thread>
#include "SFML/System.hpp"

class Clock {

	static const double GBFREQ;
	static const double PPUFRAMECYCLE;
	static const double GBFRAMETIME;
	static const double FPS60;

	public:
		Clock();
		bool stepFrame();
		bool checkFrameTime();
		void changeSpeed(const int speed);
		double getFPS();

		void setPerformance(bool performance);
		bool getPerformance();

	private:
		bool stepFrameAccuracy();
		bool stepFramePerformance();

		sf::Clock _clock;
		sf::Time _frameInterval;
		sf::Time _startTime;
		sf::Time _endTime;

		sf::Time _frameStart;

		sf::Time _sleepAmmount;


		double _FPS 		= 0;
		double _frameCount 	= 0;

		bool _performance = true;
};