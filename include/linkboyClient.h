#pragma once
#include "SFML/Network.hpp"
#include <iostream>
#include "common.h"

enum MsgType {
	NoMessage = -2,
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
			char game[17];
		};
		uint8_t gameMsg;
	};
	bool success;
	int lobby;
};

class Client {
	public:
		Client();
		~Client();

		MsgType checkPort();

		bool joinServer(sf::IpAddress IP, unsigned short port, const char* name);
		void leaveServer();

		bool createLobby();

		bool joinLobby(int lobby);
		const char* readLobby(int lobby); //return list?

		int sendGameMessage(uint8_t msg);
		int readGameMessage(uint8_t& msg);

		int sendMessage(const char* msg, const int len);
		int readMessage(char* msg, const int len);

		bool getConnected();

	private:
		int sendMessage();
		int readMessage(sf::Time);

		bool			m_connected = false;
		std::size_t		m_received = 0;
		sf::IpAddress	m_IP = "127.0.0.1";
		unsigned short	m_port= 54000;
		msgLinkboy		m_message;
		sf::UdpSocket	m_linkSock;
		char 			m_gameName[17] = {};

		uint8_t gameMessage;
};