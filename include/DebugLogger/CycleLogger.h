#pragma once
#include "DebugLogger/BaseLogger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using std::ofstream;
using std::ios_base;
using std::string;
using std::stringbuf;

class CycleLogger : public BaseLogger
{
public:
	~CycleLogger() override;

	void log(debug& log) override;

	static CycleLogger* getInstance();

private:
	CycleLogger();

	void writeLog();

	debug* m_debug;
	int m_logEntry;
	int m_maxEntries = 2000;
};