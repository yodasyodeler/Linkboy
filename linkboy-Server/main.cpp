#include "linkboyServer.h"

int main()
{
	Server server;

	while (server.checkPort()) {
		std::cout << ".";
	}

	return 0;
}