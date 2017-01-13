#include "DebugLogger/CycleLogger.h"

CycleLogger::~CycleLogger()
{
	writeLog();
	delete [] m_debug;
}

void CycleLogger::log(debug & log)
{
	m_debug[m_logEntry++] = log;
	m_logEntry %= m_maxEntries;
}

CycleLogger* CycleLogger::getInstance()
{
	static CycleLogger instance;
	return &instance;
}

CycleLogger::CycleLogger()
	:m_logEntry(0)
{
	m_debug = new debug[m_maxEntries] {};

	ofstream file("linkboy.log", ios_base::out);
	if (!file.is_open()) {
		std::cerr << "log file was unable to be instantiated" << std::endl;
		exit(-1);
	}
	file << "\t\t\t\t\t\t-----Cycle Log Of Linkboy-----" << std::endl 
		 << "#\t\tinstructions\t\t\t\t|\tPC\t\t|\tA\tF\tB\tC\tD\tE\tHL\t\tPC" << std::endl;

	file.close();
}

void CycleLogger::writeLog()
{
	ofstream file("linkboy.log", ios_base::app);
	if (!file.is_open()) {
		std::cerr << "unable to write to the log file" << std::endl;
		exit(-1);
	}

	for (int i = 0; i < m_maxEntries; ++i) {
		debug& temp = m_debug[(i + m_logEntry) % m_maxEntries];
		file << i << "\t\t" << instr[temp.opcodeIndex]  << "\t\t\t\t|\t" << std::hex << temp.oldReg.pc << "\t|\t"
			<< (uint32_t)temp.reg.a  << "\t" << (uint32_t)temp.reg.f  << "\t" 
			<< (uint32_t)temp.reg.b  << "\t" << (uint32_t)temp.reg.c  << "\t"
			<< (uint32_t)temp.reg.d  << "\t" << (uint32_t)temp.reg.e  << "\t"
			<< (uint32_t)temp.reg.hl << "\t" << (uint32_t)temp.reg.pc << "\t"
			<< (uint32_t)temp.cycles << "\n";
	}
	file.close();

	m_logEntry = 0;
}
