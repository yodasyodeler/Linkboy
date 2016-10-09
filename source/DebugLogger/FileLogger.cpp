#include "DebugLogger\FileLogger.h"


FileLogger::~FileLogger()
{
	writeLog();
	delete[] m_debug;
}

void FileLogger::log(debug& log)
{
	m_debug[m_logEntry++] = log;
	if (m_logEntry >= m_maxEntries)
		writeLog();
}

FileLogger* FileLogger::getInstance()
{
	static FileLogger instance;
	return &instance;
}

void FileLogger::writeLog()
{
	ofstream file("linkboy.log", ios_base::app);
	if (!file.is_open()) {
		std::cerr << "unable to write to the log file" << std::endl;
		exit(-1);
	}

	for (int i = 0; i < m_maxEntries; ++i) {
		debug& temp = m_debug[i];
		file << std::hex << (i + m_runningCount) << "\t\t" << instr[temp.opcodeIndex] << "\t\t\t\t|\t"  
			 << temp.reg.pc << "\t\t"
			 << (uint32_t)temp.reg.a << "\t" << (uint32_t)temp.reg.f << "\t"
			 << (uint32_t)temp.reg.b << "\t" << (uint32_t)temp.reg.c << "\t"
			 << (uint32_t)temp.reg.d << "\t" << (uint32_t)temp.reg.e << "\t"
			 << (uint32_t)temp.reg.hl << "\n";
	}
	file.close();

	m_runningCount += m_logEntry;
	m_logEntry = 0;
}

FileLogger::FileLogger()
	:m_logEntry(0)
{
	m_debug = new debug[m_maxEntries]{};
	ofstream file("linkboy.log", ios_base::out);
	if (!file.is_open()) {
		std::cerr << "log file was unable to be instantiated" << std::endl;
		exit(-1);
	}
	//Print Header
	file << "\t\t\t\t\t\t-----File Log Of Linkboy-----" << std::endl
		 << "#\t\tinstructions\t\t\t\t|\tPC\t\tA\tF\tB\tC\tD\tE\tHL" << std::endl;

	file.close();
}
