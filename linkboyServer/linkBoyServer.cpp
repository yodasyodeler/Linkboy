#include "linkboyServer.h"

Server::Server()
{
	if (m_socket.bind(54000) != sf::Socket::Done) {
		std::cout << "Error on Socket Bind\n";
		exit(-1);
	}
}

Server::~Server()
{
	delete [] m_playerList;
}

bool Server::checkPort()
{
	
	std::size_t received = 0;
	Player tempPlayer = {};
	sf::Time timeout(sf::seconds(1));
	sf::SocketSelector selector;

	int index = 0;


	selector.add(m_socket);
	if (selector.wait(timeout)) {
		std::cout << "\n";
		if (m_socket.receive((char*)(&message), sizeof(msgLinkboy), received, tempPlayer.ip, tempPlayer.port) != sf::Socket::Done) {
			std::cout << "Error on Socket Recieve\n";
			exit(-1);
		}


		strcpy_s(tempPlayer.username, message.username);
		tempPlayer.lobby = -1;

		//Is user joined
		index = findPlayer(tempPlayer);


		int temp = 0;
		if (index != -1 || message.msgType == Newuser) {
			switch (message.msgType) {

				case Newuser:
					if (index == -1) 
						addUser(tempPlayer);
					break;

				case Joinlobby:
					joinLobby(&m_playerList[index], message.gameMsg);
					break;

				case Game:

					break;

				case Message:
					std::cout << message.generalMsg << "\n";
					break;

				case Disconnected:
					removeUser(tempPlayer);
					break;

				default:

					break;
			};
		}
	}

	return true;
}

int Server::addUser(const Player & player)
{
	int re = 0;

	if (m_playerList == nullptr) {
		m_playerList = new Player[1];
		++m_playerCount;
		m_playerList[m_playerCount-1] = player;
	}
	else {
		for (int i = 0; i < m_playerCount; ++i) {
			if ((m_playerList[i].ip == player.ip) && (m_playerList[i].port == player.port)) {
				//Use a diffrent Port
				message.msgType = Error;
				strcpy_s(message.generalMsg, "Port Conflict, try a diffrent Port");
				sendMessage(player, message);
				re = -1;
			}
			else if (strcmp(m_playerList[i].username, player.username)) {
				message.msgType = Error;
				strcpy_s(message.generalMsg, "Username Conflict, try a diffrent Username");
				sendMessage(player, message);
				re = -2;
			}
			
			if (!re) {
				++m_playerCount;
				Player* temp = new Player[m_playerCount];

				for (int i = 0; i < m_playerCount-1; ++i)
					temp[i] = m_playerList[i];

				delete [] m_playerList;
				m_playerList = temp;
				m_playerList[m_playerCount-1] = player;
			}
		}
	}

	return re;
}

bool Server::removeUser(const Player & player)
{
	int index = findPlayer(player);
	bool re = false;

	if (index >= 0) {
		Player* temp = new Player[--m_playerCount];
		int i = 0;
		for ( i = 0; i < index; ++i)
			temp[i] = m_playerList[i];

		while (++i < m_playerCount) 
			temp[i-1] = m_playerList[i];

		delete [] m_playerList;
		m_playerList = temp;

		re = true;
	}

	return re;
}

bool Server::joinLobby(Player* player, const int lobby)
{
	int temp = 0;
	bool re = true;

	if (player->lobby != lobby) {
		for (int i = 0; i < m_playerCount; ++i) {
			if (m_playerList[i].lobby == lobby)
				++temp;
		}
		if (temp == 0) { //Create Lobby
			player->lobby = lobby;
			std::cout << player->username << " has joined lobby: " << player->lobby;
		}
		else if (temp < 2) { //Join Lobby
			player->lobby = lobby;
		}
		else { //too many in lobby send Error
			re = false;
		}
	}

	return re;
}

void Server::sendMessage(const Player & player, const msgLinkboy& message)
{
	if (m_socket.send((const char*)(&message), sizeof(msgLinkboy), player.ip, player.port) != sf::Socket::Done) {
		std::cout << "Error on Socket Bind\n";
		exit(-1);
	}
}

int Server::findPlayer(const Player& player)
{
	for (int i = 0; i < m_playerCount; ++i) {
		if ((m_playerList[i].ip == player.ip) && (m_playerList[i].port == player.port))
			return i;
	}

	return -1;
}