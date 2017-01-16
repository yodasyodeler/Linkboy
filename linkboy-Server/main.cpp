#include <thread>
#include <mutex>
#include "linkboyServer.h"

//Shared resources
std::mutex commandMux;
serverCommand command = {Nop,{}};

void handleInput();

int main()
{
	bool close = false;
	Server server;
	
	std::thread t1(handleInput);

	while (!close) {
		server.checkPort();

		//Safe enough for this application
		if (command.msg != Nop) {
			//std::cout << command.user << "\n";
			commandMux.lock();
			close = server.runCommand(command);
			command.msg = Nop;
			commandMux.unlock();
		}
	}
	t1.join();

	return 0;
}

void handleInput() {
	char buffer [100];
	bool close = false;	
	char* ptr;

	while (!close) {
		while (command.msg != Nop);

		std::cout << "$ ";
		std::cin.getline(buffer, 100); 

		if (!lb_strncmp(buffer, "help", 4)) {
			commandMux.lock();
			command.msg = Help;
			commandMux.unlock();
		}
		else if (!lb_strncmp(buffer, "list", 4)) {
			ptr = buffer;
			while (*ptr != ' ' && *ptr != '\0')
				ptr++;
			commandMux.lock();
			command.msg = List;
			if (*ptr != '\0'){
				ptr++;
				
				lb_strcpy(command.user, ptr);
			}
			commandMux.unlock();
		}
		else if (!lb_strncmp(buffer, "kick", 4)) {
			ptr = buffer;
			while (*ptr != ' ' && *ptr != '\0')
				ptr++;
			if (*ptr != '\0'){
				ptr++;
				commandMux.lock();
				command.msg = Kick;
				lb_strcpy(command.user, ptr);
				commandMux.unlock();
			}
			else
				std::cout << "Proper usage: kick [Player]\n"; 
		}
		else if (!lb_strncmp(buffer, "ban", 4)) {
			ptr = buffer;
			while (*ptr != ' ' && *ptr != '\0')
				ptr++;
			if (*ptr != '\0'){
				ptr++;
				commandMux.lock();
				command.msg = Ban;
				lb_strcpy(command.user, ptr);
				commandMux.unlock();
			}
			else	
				std::cout << "Proper usage: ban [Player]\n";
		}
		else if (!lb_strncmp(buffer, "close", 5)) {
			command.msg = Close;			
			close = true;
		}
		else {
			std::cout << "invalid command\n";
		}
	}
}