#pragma once
#include "SFML/Network.hpp"
#include "TextBox.h"

class NetworkMenu : public sf::Drawable, public sf::Transformable
{
	public:
		NetworkMenu();
		//NetworkMenu(const sf::Font& font);
		~NetworkMenu();

		void scaleMenu(sf::Vector2f scale);
		void setFont(const sf::Font& font);

		void displayNetworkMenu(bool visible);

		int checkButtonPress(const sf::Vector2i pos);

		void checkKeyPress(sf::Keyboard::Key key);

		sf::IpAddress getIPAddress();

		unsigned short getPort();

		const char* getName();

		//int isValid();

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		TextBox		m_textBox[3];
		Button		m_connectButton;
		Label		m_label[3];

		bool		m_visible;
};