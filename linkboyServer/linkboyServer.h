#pragma once
#include <iostream>
#include "SFML\Network.hpp"

struct Player {
	sf::IpAddress ip;
	unsigned short port;
	char username[31];
	int lobby;
};

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

class Server {
	public:
		Server();
		~Server();

		bool checkPort();

		//user functions
		int addUser(const Player& player);

		bool removeUser(const Player& player);

		bool pollGameLobby(const Player& player, const int lobby);

		bool pollAllLobbies(const Player& player);

		//lobby functions
		bool sendLobbyTextMessage(const int lobby);

		bool sendLobbyGameMessage(const uint8_t message, const Player& player);

		bool joinLobby(Player* player, const int lobby);

		bool leaveLobby(Player* player);


		
		//Misc
		void sendMessage(const Player& player, const msgLinkboy& message);

	private:
		int findPlayer(const Player& player);

		Player*		m_playerList = nullptr;
		int			m_playerCount = 0;

		msgLinkboy message;

		sf::UdpSocket m_socket;
};