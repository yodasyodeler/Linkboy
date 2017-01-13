#include "SFML-GUI/LobbyMenu.h"

LobbyMenu::LobbyMenu()
{
	for (int i = 0; i < 8; ++i) {
		m_lobbySelectBtn[i].setText("LobbyTest");
		m_lobbySelectBtn[i].setPadding(2);
		m_lobbySelectBtn[i].setColorForeground(sf::Color::Green);
		m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[i].setTextColor(sf::Color::White);
		m_lobbySelectBtn[i].setSize({ 320,36 });
		m_lobbySelectBtn[i].setPos({ 10.0f,36.0f*i });
	}
	m_lobbySelectBtn[0].setText("^");
	m_lobbySelectBtn[7].setText("v");
}

LobbyMenu::~LobbyMenu()
{
}

void LobbyMenu::setFont(sf::Font & font)
{
	for (int i = 0; i<8; ++i)
		m_lobbySelectBtn[i].setFont(font);
}

void LobbyMenu::displayLobbyMenu(bool visible)
{
	m_visible = visible;
	for (int i = 0; i < 8; ++i) {
		m_lobbySelectBtn[i].setVisible(visible);
		m_lobbySelectBtn[i].setActive(visible);
	}

	//displayDirectory();
}

void LobbyMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i < 8; ++i)
		m_lobbySelectBtn[i].scaleButton(scale);
}

int LobbyMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i = 0; i < 8; ++i)
		if (m_lobbySelectBtn[i].isClicked(pos))
			re |= (1<<i);

	return re;
}

void LobbyMenu::hoverMouse(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);

	m_lobbySelectBtn[0].setColorBackground(sf::Color::Transparent);
	m_lobbySelectBtn[7].setColorBackground(sf::Color::Transparent);

	switch (temp) {
	case 0b00000001:
		m_lobbySelectBtn[0].setColorBackground(sf::Color::Cyan);
		break;
	case 0b00000010:
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[1].setColorBackground(sf::Color::Blue);
		m_selection = 1;
		break;
	case 0b00000100: 
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[2].setColorBackground(sf::Color::Blue);
		m_selection = 2;
		break;
	case 0b00001000:
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[3].setColorBackground(sf::Color::Blue);
		m_selection = 3;
		break;
	case 0b00010000: 
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[4].setColorBackground(sf::Color::Blue);
		m_selection = 4;
		break;
	case 0b00100000: 
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[5].setColorBackground(sf::Color::Blue);
		m_selection = 5;
		break;
	case 0b01000000: 
		for (int i=1; i<7; ++i)
			m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		m_lobbySelectBtn[6].setColorBackground(sf::Color::Blue);
		m_selection = 6;
		break;
	case 0b10000000: 
		m_lobbySelectBtn[7].setColorBackground(sf::Color::Cyan);
		break;
	default:
		break;
	}
}

void LobbyMenu::moveSelection(bool upDown)
{
	if (m_selection) {
		if (upDown) {
			if (m_selection > 1) {
				m_selection -= 1;
				for (int i=1; i<7; ++i)
					m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
				m_lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
			}
			else {
				if (m_startOffset > 0) {
					m_startOffset -= 1;
				}
				else {
					int count = 6; // = m_fileDir.readDirectory();
					if (count > 6) {
						m_startOffset = count - 6;
						m_selection = 6;
					}
					else {
						m_selection = 6;
					}
					for (int i=1; i<7; ++i)
							m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
						m_lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
				}
			}
		}
		else {
			if (m_selection < 6) {
				m_selection += 1;
				for (int i=1; i<7; ++i)
					m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
				m_lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
			}
			else {
				int count = 6;// = m_fileDir.readDirectory();

				if (m_startOffset + m_selection < count) {
					m_startOffset += 1;
				}
				else {
					m_startOffset = 0;
					m_selection = 1;
					for (int i=1; i<7; ++i)
						m_lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
					m_lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
				}
			}
		}
	}
	else {
		if (upDown)
			m_selection = 6;
		else
			m_selection = 1;
		m_lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
	}
}

void LobbyMenu::movePage(bool upDown)
{
}

bool LobbyMenu::changeLobby(const int lobby)
{
	return false;
}

void LobbyMenu::displayLobby()
{
}

const char* LobbyMenu::selectLobby(int index)
{
	return "false";
}

void LobbyMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		for (int i = 0; i < 8; ++i)
			target.draw(m_lobbySelectBtn[i], states);
	} 
}
