#if defined(_WIN32)
#include <Windows.h>
#endif
#include <dirent.h>
#include "linkboy.h"
#include "DebugLogger\CycleLogger.h"
#include "DebugLogger\FileLogger.h"
#include "SFML/Network.hpp"
#include "common.h"

#if defined(_WIN32)
extern int main(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main(__argc, __argv);
}
#endif // _WIN32

#include "Directory.h"

sf::UdpSocket linkSock;

enum MsgType {
	Error = -1,
	Newuser = 0,
	Joinlobby,
	Game,
	Message,
	Leave,
	Disconnected
};


struct msgLinkboy {
	MsgType msgType;
	union {
		char generalMsg[256];
		struct {
			char username[100];
			char game[16];
		};
		uint8_t gameMsg;
	};
};

void sendMessage(const msgLinkboy* message, sf::IpAddress ip, unsigned short port);

int main(int argc, char* argv[])
{ 
	BaseLogger* log = BaseLogger::getInstance();

	if (argc > 1) {
		linkboy boy(log, argv[0], argv[1]);
		boy.startEmulation();
	}
	else {
		linkboy boy(argv[0]);
		boy.startEmulation();
	}

	//Tesing Network for link cable emualtion
	//std::size_t received = 0;
	//sf::IpAddress ip = "127.0.0.1";
	//unsigned short port= 54000;
	//msgLinkboy message;

	//if (linkSock.bind(54005) != sf::Socket::Done) {
	//	std::cout << "Error on Socket Bind\n";
	//	exit(-1);
	//}

	//while (strcmp(message.generalMsg, "exit")) {
	//	std::cout << "\n";
	//	std::cin.getline (message.generalMsg, 256);
	//	
	//	if (!strcmp(message.generalMsg, "exit")) {
	//		std::cout << "Disconnecting\n";
	//		message.msgType = Disconnected;
	//		sendMessage(&message, ip, port);
	//	}
	//	//New user, So - if( connected == false)
	//	else if (!strncmp(message.generalMsg, "new ", 4)) {
	//		std::cout << "Joinning\n";
	//		strncpy_s(message.generalMsg, 256,(message.generalMsg+4), 256-4);  
	//		message.msgType = Newuser;
	//		sendMessage(&message, ip, port);

	//	}
	//	//join lobby, Change/first time
	//	else if (!strncmp(message.generalMsg, "join ", 5)) {
	//		int temp = 0;
	//		std::cout << "Joinning\n";
	//		message.msgType = Joinlobby;
	//		strncpy_s(message.generalMsg, 256,(message.generalMsg+5), 256-5);
	//		if (convertString(temp, message.generalMsg, 10)) {
	//			message.gameMsg = temp;
	//			sendMessage(&message, ip, port);
	//		}

	//	}
	//	else {
	//		message.msgType = Message;
	//		sendMessage(&message, ip, port);
	//	}
	//}


	return 0;
}


void sendMessage(const msgLinkboy* message, sf::IpAddress ip, unsigned short port)
{
	if (linkSock.send((const char*)(message), sizeof(msgLinkboy), ip, port) != sf::Socket::Done) {
		std::cout << "Error on Socket Bind\n";
		exit(-1);
	}
}