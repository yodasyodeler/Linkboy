#include "linkboyServer.h"

int main()
{
	Server server;
	bool exit = true;

	while (server.checkPort()) {
		std::cout << ".";
	}

	//while (strcmp(data, "exit")) {

	//	

	//	if (strcmp(data, "exit")) {
	//		std::cout << "Received " << received << " bytes from " << sender << " on port " << port << std::endl;
	//		std::cout << data <<std::endl;
	//	}
	//}

	return 0;
}