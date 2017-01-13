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

		lb_strcpy(tempPlayer.username, message.username);
		tempPlayer.lobby = -1;


		//Is user joined
		index = findPlayer(tempPlayer);

		int playerCount = 0;
		if (index != -1 || message.msgType == Newuser) {
			switch (message.msgType) {

				case Newuser:
					if (index == -1) {
						if (addUser(tempPlayer) == 0)
							std::cout << "Welcome new player: " << tempPlayer.username << std::endl;
						else
							std::cout << "failed to add: " << tempPlayer.username << std::endl;

						message.success = true;
						sendMessage(tempPlayer);
					}
					else
						message.success = false;
					
					break;

				case Joinlobby:
					if (joinLobby(m_playerList[index], message.lobby))
						std::cout << m_playerList[index].username << " has joinned lobby: " << m_playerList[index].lobby << std::endl;
					else
						std::cout << "failed to join: " << tempPlayer.username << std::endl;
					break;
				case Readlobby:
					while (playerCount > -1) {
						playerCount = readList(playerCount);
						sendMessage(tempPlayer);
					}
					break;
				case Game:

					//std::cout << "game message: " << std::hex << (int)message.gameMsg <<", from: " << m_playerList[index].username << " \n";
					for (int i=0; i<m_playerCount; ++i) {
						if (m_playerList[i].lobby == m_playerList[index].lobby && lb_strncmp(m_playerList[i].username, m_playerList[index].username, 31) ) {
							sendMessage(m_playerList[i]);
						}
					}
					break;
				case Createlobby:
					if (createLobby(index))
						std::cout << m_playerList[index].username << " has created lobby: " << m_playerList[index].lobby << std::endl;
					else
						std::cout << "failed create: " << tempPlayer.username << std::endl;
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
		m_playerList[m_playerCount++] = player;
	}
	else {
		for (int i = 0; i < m_playerCount; ++i) {
			if ((m_playerList[i].ip == player.ip) && (m_playerList[i].port == player.port)) {
				//Use a diffrent Port
				message.msgType = Error;
				lb_strcpy(message.generalMsg, "Port Conflict, try a diffrent Port");
				std::cout << "Port Conflict, try a diffrent Port";
				sendMessage(player);
				re = -1;
			}
			else if (lb_strncmp(m_playerList[i].username, player.username, 100) == 0) {
				message.msgType = Error;
				lb_strcpy(message.generalMsg, "Username Conflict, try a diffrent Username");
				std::cout << "Username Conflict, try a diffrent Username";
				sendMessage(player);
				re = -2;
			}
			
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

	return re;
}

bool Server::removeUser(const Player& player)
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

bool Server::createLobby(const int index)
{
	Lobby* tempLobby;
	int temp = findFirstLobby();

	message.msgType = Createlobby;
	if (temp > -1) {
		message.success = true;
		m_playerList[index].lobby = temp;

		Lobby lob = {{},1};
		lb_strcpy(lob, m_message.game);		
		temp = new Lobby[m_lobbyCount++];
		for (int i=0; i<m_lobbyCount; ++i)
			temp[i] = m_lobbyList[i];
		delete [] tempLobby;
		m_lobbyList = temp;

		m_lobbyList[m_lobbyCount-1] = tempLobby;
	}
	else
		message.success = false;

	message.lobby = m_playerList[index].lobby;
	sendMessage(m_playerList[index]);

	

	return message.success;
}

bool Server::joinLobby(Player& player, const int lobby)
{
	int temp = 0;
	bool re = false;

	message.msgType = Joinlobby;

	if (lobby == -1) {
		message.success = true;
		player.lobby = -1;
	}
	if (player.lobby != lobby) {
		for (int i = 0; i < m_playerCount; ++i) {
			if (m_playerList[i].lobby == lobby)
				++temp;
		}
		if (temp == 0) {		//No Lobby Yet
			message.success = false;
			//std::cout << player.username << " has joined lobby: " << player.lobby;
		}
		else if (temp < 2) {	//Join Lobby
			message.success = true;
			player.lobby = lobby;
			re = true;
		}
		else				//Too Many In Lobby Send Error
			message.success = false;
	}

	message.lobby = player.lobby;
	sendMessage(player);

	return re;
}

void Server::sendMessage(const Player & player)
{
	message.lobby = player.lobby;
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

int Server::findFirstLobby()
{
	uint8_t lobbyCount[maxLobbyCount] = {};
	int temp = 0;

	for (int i = 0; i < m_playerCount; ++i) {
		temp = m_playerList[i].lobby;
		if (temp < maxLobbyCount && i > 0) {
			++(lobbyCount[temp]);
		}
	}

	temp = -1;
	for (int i = 0; (i < maxLobbyCount) && (temp == -1); ++i) {
		if (lobbyCount[i] == 0)
			temp = i;
	}

	return temp;
}

int Server::readList(int playerNum)
{
	int count = 0;

	message.moreToRead = false;
	if (m_playerList != nullptr) {
		message.generalMsg[0] = '\0';
		message.lobby = m_playerList[playerNum].lobby;

		while (count < 256 && playerNum < m_playerCount) {
			if (message.lobby == m_playerList[playerNum].lobby) {
				int len = lb_strlen(m_playerList[playerNum].username);
				if ((len + count) < 255) {
					lb_strncat(message.generalMsg, m_playerList[playerNum++].username, len);
					lb_strncat(message.generalMsg, "\n", 1);
					count += len;
				}
				else
					lb_strncat(message.generalMsg, "\0", 1);
			}
		}

		if (playerNum < m_playerCount)
			message.moreToRead = true;
		else
			playerNum = -1;
	}
	else
		playerNum = -2;

	return playerNum;
}