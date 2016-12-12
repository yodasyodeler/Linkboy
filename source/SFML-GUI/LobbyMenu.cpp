#include "SFML-GUI/LobbyMenu.h"

LobbyMenu::LobbyMenu()
{
	for (int i = 0; i < 8; ++i) {
		lobbySelectBtn[i].setText("FileTest");
		lobbySelectBtn[i].setPadding(2);
		lobbySelectBtn[i].setColorForeground(sf::Color::Green);
		lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[i].setTextColor(sf::Color::White);
		lobbySelectBtn[i].setSize({ 320,36 });
		lobbySelectBtn[i].setPos({ 10.0f,36.0f*i });
	}
	lobbySelectBtn[0].setText("^");
	lobbySelectBtn[7].setText("v");
}

LobbyMenu::~LobbyMenu()
{
}

void LobbyMenu::setFont(sf::Font & font)
{
	for (int i = 0; i<8; ++i)
		lobbySelectBtn[i].setFont(font);
}

void LobbyMenu::displayLobbyMenu(bool visible)
{
	m_visible = visible;
	for (int i = 0; i < 8; ++i) {
		lobbySelectBtn[i].setVisible(visible);
		lobbySelectBtn[i].setActive(visible);
	}

	//displayDirectory();
}

void LobbyMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i < 8; ++i)
		lobbySelectBtn[i].scaleButton(scale);
}

int LobbyMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i = 0; i < 8; ++i)
		if (lobbySelectBtn[i].isClicked(pos))
			re |= (1<<i);

	return re;
}

void LobbyMenu::hoverMouse(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);

	lobbySelectBtn[0].setColorBackground(sf::Color::Transparent);
	lobbySelectBtn[7].setColorBackground(sf::Color::Transparent);

	switch (temp) {
	case 0b00000001:
		lobbySelectBtn[0].setColorBackground(sf::Color::Cyan);
		break;
	case 0b00000010:
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[1].setColorBackground(sf::Color::Blue);
		m_selection = 1;
		break;
	case 0b00000100: 
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[2].setColorBackground(sf::Color::Blue);
		m_selection = 2;
		break;
	case 0b00001000:
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[3].setColorBackground(sf::Color::Blue);
		m_selection = 3;
		break;
	case 0b00010000: 
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[4].setColorBackground(sf::Color::Blue);
		m_selection = 4;
		break;
	case 0b00100000: 
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[5].setColorBackground(sf::Color::Blue);
		m_selection = 5;
		break;
	case 0b01000000: 
		for (int i=1; i<7; ++i)
			lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
		lobbySelectBtn[6].setColorBackground(sf::Color::Blue);
		m_selection = 6;
		break;
	case 0b10000000: 
		lobbySelectBtn[7].setColorBackground(sf::Color::Cyan);
		break;
	default:
		break;
	}
}

void LobbyMenu::moveSelection(bool upDown)
{
	//if (m_selection) {
	//	if (upDown) {
	//		if (m_selection > 1) {
	//			m_selection -= 1;
	//			for (int i=1; i<7; ++i)
	//				lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
	//			lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
	//		}
	//		else {
	//			if (m_startOffset > 0) {
	//				m_startOffset -= 1;
	//			}
	//			displayLobby();
	//		}
	//	}
	//	else {
	//		if (m_selection < 6) {
	//			m_selection += 1;
	//			for (int i=1; i<7; ++i)
	//				lobbySelectBtn[i].setColorBackground(sf::Color::Transparent);
	//			lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
	//		}
	//		else {
	//			//int count = m_fileDir.readDirectory();
	//			int count;// = setting.client->readLobby();

	//			if (m_startOffset + m_selection < count) {
	//				m_startOffset += 1;
	//			}
	//			displayLobby();
	//		}
	//	}
	//}
	//else {
	//	if (upDown)
	//		m_selection = 6;
	//	else
	//		m_selection = 1;
	//	lobbySelectBtn[m_selection].setColorBackground(sf::Color::Blue);
	//}

}

void LobbyMenu::movePage(bool upDown)
{
	/*if (upDown) {
		if (m_startOffset > 0) {
			m_startOffset -= 1;
		}
		displayDirectory();
	}
	else {
		int count = m_fileDir.readDirectory();

		if (m_startOffset + m_selection < count) {
			m_startOffset += 1;
		}
		displayDirectory();
	}*/
}

bool LobbyMenu::changeLobby(const int lobby)
{
	/*m_startOffset = 0;
	return m_fileDir.changeDirectory(lobby);*/
	return false;
}

void LobbyMenu::displayLobby()
{
	/*int count = m_fileDir.readDirectory();

	for (int i = 1; i < 7; ++i) {
		lobbySelectBtn[i].setText(m_fileDir.getFileName(i+m_startOffset-1));
	}*/
}

const char* LobbyMenu::selectFile(int index)
{
	/*const char* re = nullptr;

	if (index == -1)
		index = m_selection-1;

	if (index >= 0) {
		if (m_fileDir.isDir(index + m_startOffset)) {
			if (lb_strncmp(m_fileDir.getFileName(index + m_startOffset), "..", 3) == 0) {
				moveUpDirectory();
				displayDirectory();
			}
			else {
				changeDirectory(m_fileDir.getFilePath(index + m_startOffset));
				displayDirectory();
			}
		}
		else
			re = m_fileDir.getFilePath(index + m_startOffset);
	}

	return re;*/
	return "false";
}

void LobbyMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	/*if (m_visible) {
		for (int i = 0; i < 8; ++i)
			target.draw(lobbySelectBtn[i], states);
		target.draw(fileBackground, states);
	} */
}
