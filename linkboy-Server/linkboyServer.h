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

struct Lobby {
	char gameName[17];
	uint8_t userNum;
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

enum consoleMsg {
	Nop = 0,
	Help,
	List,
	Kick,
	Ban,
	Close
};

struct serverCommand {
	consoleMsg msg;
	char user[100];
};

struct msgLinkboy {
	MsgType msgType;
	bool moreToRead;
	union {
		char generalMsg[256];
		struct {
			char username[31];			
			char game[17];
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

		bool runCommand(const serverCommand& cmd);

	private:
		//user functions
		int addUser(const Player& player);

		bool removeUser(const Player& player);

		bool pollGameLobby(const Player& player, const int lobby);

		bool pollAllLobbies(const Player& player);

		//lobby functions
		bool sendLobbyTextMessage(const int lobby);

		bool sendLobbyGameMessage(const uint8_t message, const Player& player);

		bool createLobby(const char* gameName, const int index);

		bool joinLobby(Player& player, const int lobby);
		
		void sendMessage(const Player& player);

		int findPlayer(const Player& player);

		int findFirstLobby();

		int readList(int playerNum);

		static const int maxLobbyCount = 32;

		Player*		m_playerList = nullptr;
		int			m_playerCount = 0;
		Lobby*		m_lobbyList = nullptr;
		int 		m_lobbyCount = 0;

		msgLinkboy m_message;

		sf::UdpSocket m_socket;
};