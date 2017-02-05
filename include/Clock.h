#pragma once 

#include <chrono>

using namespace std::chrono;

class Clock {
	static const double GBFREQ = 4194304.0;
	static const double PPUFRAMECYCLE = 70224;
	static const double GBFRAMETIME = PPUFRAMECYCLE/GBFREQ;

	typedef high_resolution_clock::time_point timePoint_t;

    public:
		Clock();

    private:
		timePoint_t _startTime;
		timePoint_t _endTime;
};