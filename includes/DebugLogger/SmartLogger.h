#pragma once
#include "DebugLogger\BaseLogger.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

using std::ofstream;
using std::ios_base;

using std::ios;

struct logPoint { uint16_t count; uint16_t regPC; };

class SmartLogger : public BaseLogger
{
public:
	~SmartLogger();

	void log(debug& log) override;

	static SmartLogger* getInstance();

private:
	SmartLogger();

	void writeLog();

	std::vector<logPoint> m_list;
	int m_logEntry;
};