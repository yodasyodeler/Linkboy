#pragma once
#include <iostream>
#include "SFML/Network.hpp"
#include "common.h"

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
	Readlobby,
	Createlobby,
	Game,
	Message,
	Leave,
	Disconnected
};
struct msgLinkboy {
	MsgType msgType;
	bool moreToRead;
	union {
		char generalMsg[256];
		struct {
			char username[31];			
			char game[16];
		};
		uint8_t gameMsg;
	};
	bool success;
	int lobby;
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

		bool createLobby(const int index);

		bool joinLobby(Player& player, const int lobby);
		
		//Misc
		void sendMessage(const Player& player);

	private:
		int findPlayer(const Player& player);

		int findFirstLobby();

		int readList(int playerNum);

		static const int maxLobbyCount = 32;

		Player*		m_playerList = nullptr;
		int			m_playerCount = 0;

		msgLinkboy message;

		sf::UdpSocket m_socket;
};