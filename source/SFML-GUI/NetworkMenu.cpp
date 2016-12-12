#include "SFML-GUI/NetworkMenu.h"

bool ipTextRule(char* buffer, int len);
bool portTextRule(char*buffer, int len);
bool userTextRule(char*buffer, int len);

NetworkMenu::NetworkMenu()
{
	m_label[0].setPos({ 35, 7 });
	m_label[1].setPos({ 35, 7 + 55 + 7 });
	m_label[2].setPos({ 35, 7 + 55 + 7 + 55 + 7 });

	m_label[0].setText("IP:");
	m_label[1].setText("PORT:");
	m_label[2].setText("User:");


	//Delete after testing
	m_textBox[0].setBuffer("127.0.0.1");
	m_textBox[1].setBuffer("54005");
	m_textBox[2].setBuffer("Cory B");

	m_textBox[0].setPos({ 27+(45*2), 12 });
	m_textBox[1].setPos({ 27+(45*2), 12 + 55 + 7 });
	m_textBox[2].setPos({ 27+(45*2), 12 + 55 + 7 + 55 + 7 });

	m_connectButton.setSize({ 50 * 2, 45 });
	m_connectButton.setPadding(3);
	m_connectButton.setColorBackground(sf::Color(0xd3,0xd3,0xd3));
	m_connectButton.setColorForeground(sf::Color(0xd3,0xd3,0xd3));
	m_connectButton.setText("Connect");
	m_connectButton.setTextColor(sf::Color::White);

	for (int i = 0; i < 3; ++i) {
		m_label[i].setTextColor(sf::Color::White);
		m_label[i].setColorBackground(sf::Color::Transparent);
		m_label[i].setColorForeground(sf::Color::Transparent);
		m_label[i].setSize({ 45 * 2, 45 });

		m_textBox[i].setTextColor(sf::Color::Black);
		m_textBox[i].setColorBackground(sf::Color::Red);
		m_textBox[i].setColorForeground(sf::Color::White);
		m_textBox[i].setSize({ 55 * 3, 40 });
		m_textBox[i].setPadding(5);
	}

	m_textBox[0].setTextRule(ipTextRule);
	m_textBox[1].setTextRule(portTextRule);
	m_textBox[2].setTextRule(userTextRule);

	m_connectButton.setPos({ (320-(55*2+7)), 288-(55+14)});
}

//NetworkMenu::NetworkMenu(const sf::Font & font)
//{
//	setFont(font);
//}

NetworkMenu::~NetworkMenu()
{
}

void NetworkMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i = 0; i < 3; ++i)
		m_textBox[i].scaleButton(scale);

	m_connectButton.scaleButton(scale);
}

void NetworkMenu::setFont(const sf::Font & font)
{
	for (int i = 0; i < 3; ++i) {
		m_label[i].setFont(font);
		m_textBox[i].setFont(font);
	}
	m_connectButton.setFont(font);
}

void NetworkMenu::displayNetworkMenu(bool visible)
{
	m_visible = visible;

	m_connectButton.setVisible(visible);

	for (int i = 0; i < 3; ++i) {
		m_label[i].setVisible(visible);
		m_textBox[i].setVisible(visible);
		m_textBox[i].setActive(visible);
	}
}

int NetworkMenu::checkButtonPress(const sf::Vector2i pos)
{
	for (int i=0; i<3; ++i)
		m_textBox[i].isClicked(pos);

	return m_connectButton.isClicked(pos);
}

void NetworkMenu::checkKeyPress(sf::Keyboard::Key key)
{
	int temp = 0;

	for (int i=0; i<3; ++i)
		m_textBox[i].keyPress(key);

	for (int i=0; i<3; ++i)
		temp |= m_textBox[i].isValid() << i;

	if (temp == 0x7) {
		m_connectButton.setActive(true);
		m_connectButton.setColorForeground(sf::Color::Green);
	}
	else {
		m_connectButton.setActive(false);
		m_connectButton.setColorForeground(sf::Color(0xd3,0xd3,0xd3));
	}

}

sf::IpAddress NetworkMenu::getIPAddress()
{
	return sf::IpAddress(m_textBox[0].getBuffer());
}

unsigned short NetworkMenu::getPort()
{
	return atoi(m_textBox[1].getBuffer());
}

const char * NetworkMenu::getName()
{
	return m_textBox[2].getBuffer();
}

void NetworkMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		for (int i = 0; i < 3; ++i) {
			target.draw(m_label[i], states);
			target.draw(m_textBox[i], states);
		}
		target.draw(m_connectButton, states);
	}
}

bool ipTextRule(char* buffer, int len)
{
	bool re = true;
	int i = 0;
	int temp = 0;
	int decimalCount = 0;

	while (i < len && re) {
		while (buffer[i] != '.' && i < len) {
			if (buffer[i] >= 0x30 && buffer[i] < 0x40)
				temp = (buffer[i] - 0x30) + (temp * 10);
			else
				re = false;
			++i;
		}
		if (temp > 255)
			re = false;

		temp = 0;
		if (buffer[i] == '.')
			++decimalCount;
		++i;
	}

	if (decimalCount != 3)
		re = false;

	return re;
}

bool portTextRule(char*buffer, int len)
{
	bool re = true;
	int temp = 0;
	
	for (int i = 0; i < len; ++i) {
		if (buffer[i] >= 0x30 && buffer[i] < 0x40)
			temp = (buffer[i] - 0x30) + (temp * 10);
		else
			re = false;
	}

	if (temp > 65535)
		re = false;

	return (re && len > 0);
}

bool userTextRule(char*buffer, int len)
{
	return (len < 32 && len > 0);
}