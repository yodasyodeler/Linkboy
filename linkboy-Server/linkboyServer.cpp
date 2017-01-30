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
		if (m_socket.receive((char*)(&m_message), sizeof(msgLinkboy), received, tempPlayer.ip, tempPlayer.port) != sf::Socket::Done) {
			std::cout << "Error on Socket Recieve\n";
			exit(-1);
		}

		lb_strcpy(tempPlayer.username, m_message.username);
		tempPlayer.lobby = -1;


		//Is user joined
		index = findPlayer(tempPlayer);

		int playerCount = 0;
		if (index != -1 || m_message.msgType == Newuser) {
			switch (m_message.msgType) {

				case Newuser:
					if (index == -1) {
						if (addUser(tempPlayer) == 0)
							std::cout << "Welcome new player: " << tempPlayer.username << std::endl;
						else
							std::cout << "Failed to add: " << tempPlayer.username << std::endl;

						m_message.success = true;
						sendMessage(tempPlayer);
					}
					else
						m_message.success = false;
					
					break;

				case Joinlobby:
					if (joinLobby(m_playerList[index], m_message.lobby))
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

					//std::cout << "game message: " << std::hex << (int)m_message.gameMsg <<", from: " << m_playerList[index].username << " \n";
					for (int i=0; i<m_playerCount; ++i) {
						if (m_playerList[i].lobby == m_playerList[index].lobby && lb_strncmp(m_playerList[i].username, m_playerList[index].username, 31) ) {
							sendMessage(m_playerList[i]);
						}
					}
					break;
				case Createlobby:
					if (createLobby("testing", index))
						std::cout << m_playerList[index].username << " has created lobby: " << m_playerList[index].lobby << std::endl;
					else
						std::cout << "failed create: " << tempPlayer.username << std::endl;
					break;
				case Message:
					std::cout << m_message.generalMsg << "\n";
					break;

				case Disconnected:
					removeUser(findPlayer(tempPlayer));
					break;

				default:
					break;
			};
		}
	}

	return true;
}

bool Server::runCommand(const serverCommand& cmd)
{
	bool re = false;
	int index = -1;

	switch (cmd.msg) {
		case Help:
			std::cout << "\tlist [Lobby]\n"
					  << "\t\tDisplay users by Lobby\n"
					  << "\t\tby default all lobbys will be displayed\n"
					  << "\tban [Player]\n"
					  << "\t\tKicks and doesn't allow player to rejoin\n"
					  << "\tkick [Player]\n"
					  << "\t\tKicks player but allows them to rejoin\n"
					  << "\tclose\n"
					  << "\t\tCloses this application\n";  
			break; 
		case List:
			std::cout << cmd.user <<"\t\t------ ServerList ------\n";
			for (int i=0; i<m_playerCount; ++i)
				std::cout << m_playerList[i].username << " " << m_playerList[i].lobby << " "<< m_playerList[i].port << " " << m_playerList[i].ip <<"\n";  
		break;
		case Ban:
			//add to ban list
		case Kick:
			for (int i = 0; i < m_playerCount && index != -1; ++i) {
				if (!lb_strncmp(cmd.user, m_playerList[i].username, 31))
					index = i;
			}

			if (removeUser(index))
				std::cout << "Player: " << cmd.user << " Removed from server\n";
			else
				std::cout << "Player: " << cmd.user << " not found\n";
		break;
		case Close:
			std::cout << "Closing Server\n";
			re = true;
		default:
		break;
	}

	return re;
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
				m_message.msgType = Error;
				lb_strcpy(m_message.generalMsg, "Port Conflict, try a diffrent Port");
				std::cout << "Port Conflict, try a diffrent Port";
				sendMessage(player);
				re = -1;
			}
			else if (lb_strncmp(m_playerList[i].username, player.username, 100) == 0) {
				m_message.msgType = Error;
				lb_strcpy(m_message.generalMsg, "Username Conflict, try a diffrent Username");
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

bool Server::removeUser(const int index)
{
	bool re = false;

	if (index >= 0 && index < m_playerCount) {
		int lobby = m_playerList[index].lobby;
		if ( lobby != -1) {
			if (--m_lobbyList[lobby].userNum <= 0)
				removeLobby(index);
		}

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

bool Server::removeLobby(const int index)
{

	return false;
}

bool Server::createLobby(const char* gameName, const int index)
{
	Lobby* tempLobby;
	int temp = findFirstLobby();

	m_message.msgType = Createlobby;
	if (temp > -1) {
		m_message.success = true;
		m_playerList[index].lobby = temp;

		Lobby lob = {{},1};
		lb_strcpy(lob.gameName, m_message.game);		
		tempLobby = new Lobby[m_lobbyCount];
		for (int i=0; i<m_lobbyCount; ++i)
			tempLobby[i] = m_lobbyList[i];
		delete [] m_lobbyList;
		m_lobbyList = tempLobby;

		m_lobbyList[m_lobbyCount] = lob;
		m_lobbyCount++;
	}
	else
		m_message.success = false;

	m_message.lobby = m_playerList[index].lobby;
	sendMessage(m_playerList[index]);

	

	return m_message.success;
}

bool Server::joinLobby(Player& player, const int lobby)
{
	int temp = 0;
	bool re = false;

	m_message.msgType = Joinlobby;

	if (lobby == -1) {
		m_message.success = true;
		player.lobby = -1;
	}
	if (player.lobby != lobby) {
		for (int i = 0; i < m_playerCount; ++i) {
			if (m_playerList[i].lobby == lobby)
				++temp;
		}
		if (temp == 0) {		//No Lobby Yet
			m_message.success = false;
			//std::cout << player.username << " has joined lobby: " << player.lobby;
		}
		else if (temp < 2) {	//Join Lobby
			m_message.success = true;
			player.lobby = lobby;
			re = true;
		}
		else				//Too Many In Lobby Send Error
			m_message.success = false;
	}

	m_message.lobby = player.lobby;
	sendMessage(player);

	return re;
}

void Server::sendMessage(const Player & player)
{
	m_message.lobby = player.lobby;
	if (m_socket.send((const char*)(&m_message), sizeof(msgLinkboy), player.ip, player.port) != sf::Socket::Done) {
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

	m_message.moreToRead = false;
	if (m_playerList != nullptr) {
		m_message.generalMsg[0] = '\0';
		m_message.lobby = m_playerList[playerNum].lobby;

		while (count < 256 && playerNum < m_playerCount) {
			if (m_message.lobby == m_playerList[playerNum].lobby) {
				int len = lb_strlen(m_playerList[playerNum].username);
				if ((len + count) < 255) {
					lb_strncat(m_message.generalMsg, m_playerList[playerNum++].username, len);
					lb_strncat(m_message.generalMsg, "\n", 1);
					count += len;
				}
				else
					lb_strncat(m_message.generalMsg, "\0", 1);
			}
		}

		if (playerNum < m_playerCount)
			m_message.moreToRead = true;
		else
			playerNum = -1;
	}
	else
		playerNum = -2;

	return playerNum;
}