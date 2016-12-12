#include "DebugLogger/SmartLogger.h"

SmartLogger::~SmartLogger()
{
	writeLog();
}

void SmartLogger::log(debug & log)
{
	//Out of BIOS
	if (log.oldReg.pc >= 0x100) {
		switch (log.opcodeIndex) {
			//Jumps
			case 0xC3:
			case 0xC2:
			case 0xCA:
			case 0xD2:
			case 0xDA:
			//Jump Immediate
			case 0x18:
			case 0x20:
			case 0x28:
			case 0x30:
			case 0x38:
				//Forward Branch
				if (log.oldReg.pc < log.reg.pc) { //Log Branch / Flush Branch
					m_list.push_back(logPoint{ 0, log.oldReg.pc });
					std::ofstream myfile("linkboy.log", ios::app | ios::binary);

					logPoint p = (logPoint)m_list.back();
					m_list.pop_back();
					if (myfile.is_open()) {
						myfile.write(reinterpret_cast<char*>(&p), sizeof(logPoint));
						myfile.close();
					}
					else {
						std::cerr << "Failed to Log";
						exit(-1);
					}
				}

				//Backwards Branch
				else {
					//Check list / Add or Modify listing


				}
			//Jump HL
			case 0xE9:

			//RST
			case 0xC7:
			case 0xCF:
			case 0xD7:
			case 0xDF:
			case 0xE7:
			case 0xEF:
			case 0xF7:
			case 0xFF:

			//Call
			case 0xC9:

			case 0xC0:
			case 0xC8:
			case 0xD0:
			case 0xD8:
			case 0xD9:

			default:
				break;
		}
	}
}

SmartLogger* SmartLogger::getInstance()
{
	static SmartLogger instance;
	return &instance;
}

SmartLogger::SmartLogger()
	:m_logEntry(0)
{
	std::ofstream myfile("linkboy.log", ios::out | ios::binary);

	logPoint p = { 0, 0x100 };

	if (myfile.is_open()) {
		myfile.write(reinterpret_cast<char*>(&p), sizeof(logPoint));
		myfile.close();
	}
	else {
		std::cerr << "Failed to Log";
		exit(-1);
	}
}

void SmartLogger::writeLog()
{
	

	m_logEntry = 0;
}
