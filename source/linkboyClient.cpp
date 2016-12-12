#include "linkboyClient.h"

Client::Client()
{}

Client::~Client()
{}

//change later
MsgType Client::checkPort()
{
	MsgType re = NoMessage;
	if (readMessage(sf::milliseconds(20)) == 0) {
		re = m_message.msgType;
		
		if (re == Error || re == Disconnected) {
			m_connected = false;
			m_linkSock.unbind();
		}	
	}

	return re;
}

bool Client::joinServer(sf::IpAddress IP, unsigned short port, const char* name)
{
	if (!m_connected) {
		m_connected = true;
		if (m_linkSock.bind(port) != sf::Socket::Done) {
			std::cout << "Error on Socket Bind\n";
			exit(-1);
		}

		m_IP = IP;

		m_message.msgType = Newuser;
		lb_strcpy(m_message.username, name);
		sendMessage();

		if (readMessage(sf::milliseconds(20)) != 0) {
			m_connected = false;
			m_linkSock.unbind();
			std::cout << "join failed\n";
		}
	}

	return m_connected;
}

void Client::leaveServer()
{
	std::cout << "Disconnecting\n";
	m_message.msgType = Disconnected;
	sendMessage();
}

bool Client::createLobby()
{
	m_message.msgType = Createlobby;
	sendMessage();

	return (readMessage(sf::milliseconds(20)) == 0);
}

bool Client::joinLobby(int lobby)
{
	bool re = false;
	m_message.msgType = Joinlobby;
	m_message.lobby = lobby;
	sendMessage();

	if (readMessage(sf::milliseconds(20)) == 0) {
		re = true;
	}
	else
		m_message.lobby = -1;

	return re;
}

void Client::readLobby(int lobby)
{
	if (m_connected) {
		do {
			m_message.msgType = Readlobby;
			m_message.lobby = lobby;
			sendMessage();

			if (readMessage(sf::milliseconds(20)) == 0) {
				std::cout << "----Lobby: " << m_message.lobby << " ----\n"
				<< m_message.generalMsg;
			}
		} while(m_message.moreToRead);
	} 
}

int Client::sendMessage(const char * msg, const int len)
{
	m_message.msgType = Message;
	lb_strcpy(m_message.generalMsg, msg);

	return sendMessage();
}

int Client::readMessage(char * msg, const int len)
{
	int re = readMessage(sf::milliseconds(20));
	lb_strcpy(msg, m_message.generalMsg);

	return re;
}

int Client::sendMessage()
{
	int re = -1;
	if (m_connected) {
		if (m_linkSock.send((const char*)(&m_message), sizeof(msgLinkboy), m_IP, m_port) != sf::Socket::Done) {
			std::cout << "Error on Socket Message Send\n";
			exit(-1);
		}
		re = 0;
	}

	return re;
}

int Client::readMessage(sf::Time timeout)
{
	//sf::Time timeout(sf::milliseconds(20));
	sf::SocketSelector selector;
	
	sf::IpAddress IP;
	unsigned short port;
	int re = -1;

	if (m_connected) {
		selector.add(m_linkSock);
		if (selector.wait(timeout)) {
			if (m_linkSock.receive((char*)(&m_message), sizeof(msgLinkboy), m_received, IP, port) != sf::Socket::Done) {
				std::cout << "Error on Socket Message Recieve\n";
				exit(-1);
			}
			if ( (m_IP == IP) && (m_port == port) )
			{
				if (m_message.success)
					re = 0;
				else
					re = 1;
			}
		}
	}

	return re;
}

int Client::sendGameMessage(uint8_t msg)
{
	m_message.msgType = Game;
	m_message.gameMsg = msg;
	return sendMessage();
}

int Client::readGameMessage(uint8_t& msg)
{
	int re = -1; 

	if (readMessage(sf::microseconds(1)) == 0) {
		msg = m_message.gameMsg;
		re = 0;
	}
	
	return re;
}

bool Client::getConnected()
{
	return m_connected;
}