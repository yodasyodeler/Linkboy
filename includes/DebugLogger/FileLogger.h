#pragma once
#include "DebugLogger\BaseLogger.h"
#include <iostream>
#include <fstream>
#include <string>

using std::ofstream;
using std::ios_base;
using std::string;

class FileLogger : public BaseLogger
{
	public:
		~FileLogger();

		void log(debug& log) override;

		static FileLogger* getInstance();
	private:
		FileLogger();
		void writeLog();


		debug* m_debug;
		int m_logEntry;
		int m_maxEntries = 1000;
		uint32_t m_runningCount;
};