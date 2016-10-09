#include "SFML-GUI\FileMenu.h"

FileMenu::FileMenu(const char* dirName)
{
	for (int i = 0; i < 8; ++i) {
		fileSelectBtn[i].setText("FileTest");
		fileSelectBtn[i].setPadding(2);
		fileSelectBtn[i].setColorForeground(sf::Color::Green);
		fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
		fileSelectBtn[i].setTextColor(sf::Color::White);
		fileSelectBtn[i].setSize({ 280,36 });
		fileSelectBtn[i].setPos({ 40.0f,36.0f*i });
	}
	fileSelectBtn[0].setText("^");
	fileSelectBtn[7].setText("v");

	m_fileDir.changeDirectory(dirName);
}

FileMenu::FileMenu(const sf::Font & font, const char* dirName)
{
	for (int i = 0; i < 8; ++i) {
		fileSelectBtn[i].setFont(font);
		fileSelectBtn[i].setText("FileTest");
		fileSelectBtn[i].setPadding(2);
		fileSelectBtn[i].setColorForeground(sf::Color::Green);
		fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
		fileSelectBtn[i].setTextColor(sf::Color::White);
		fileSelectBtn[i].setSize({ 280,36 });
		fileSelectBtn[i].setPos({ 40.0f,36.0f*i });
	}
	fileSelectBtn[0].setText("^");
	fileSelectBtn[7].setText("v");

	m_fileDir.changeDirectory(dirName);
}

FileMenu::~FileMenu()
{
}

void FileMenu::setFont(sf::Font & font)
{
	for (int i = 0; i<8; ++i)
		fileSelectBtn[i].setFont(font);
}

void FileMenu::displayFileMenu(bool visible)
{
	m_visible = visible;
	fileBackground.setVisible(visible);
	for (int i = 0; i < 8; ++i) {
		fileSelectBtn[i].setVisible(visible);
		fileSelectBtn[i].setActive(visible);
	}

	displayDirectory();
}

void FileMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i < 8; ++i)
		fileSelectBtn[i].scaleButton(scale);
}

int FileMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i = 0; i < 8; ++i)
		if (fileSelectBtn[i].isClicked(pos))
			re |= (1<<i);

	return re;
}

void FileMenu::hoverMouse(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);

	fileSelectBtn[0].setColorBackground(sf::Color::Transparent);
	fileSelectBtn[7].setColorBackground(sf::Color::Transparent);

	switch (temp) {
		case 0b00000001:
			fileSelectBtn[0].setColorBackground(sf::Color::Cyan);
			break;
		case 0b00000010:
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[1].setColorBackground(sf::Color::Blue);
			m_selection = 1;
			break;
		case 0b00000100: 
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[2].setColorBackground(sf::Color::Blue);
			m_selection = 2;
			break;
		case 0b00001000:
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[3].setColorBackground(sf::Color::Blue);
			m_selection = 3;
			break;
		case 0b00010000: 
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[4].setColorBackground(sf::Color::Blue);
			m_selection = 4;
			break;
		case 0b00100000: 
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[5].setColorBackground(sf::Color::Blue);
			m_selection = 5;
			break;
		case 0b01000000: 
			for (int i=1; i<7; ++i)
				fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
			fileSelectBtn[6].setColorBackground(sf::Color::Blue);
			m_selection = 6;
			break;
		case 0b10000000: 
			fileSelectBtn[7].setColorBackground(sf::Color::Cyan);
			break;
		default:
			break;
	}
}

void FileMenu::moveSelection(bool upDown)
{
	if (m_selection) {
		if (upDown) {
			if (m_selection > 1) {
				m_selection -= 1;
				for (int i=1; i<7; ++i)
					fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
				fileSelectBtn[m_selection].setColorBackground(sf::Color::Blue);
			}
			else {
				if (m_startOffset > 0) {
					m_startOffset -= 1;
				}
				displayDirectory();
			}
		}
		else {
			if (m_selection < 6) {
				m_selection += 1;
				for (int i=1; i<7; ++i)
					fileSelectBtn[i].setColorBackground(sf::Color::Transparent);
				fileSelectBtn[m_selection].setColorBackground(sf::Color::Blue);
			}
			else {
				int count = m_fileDir.readDirectory();

				if (m_startOffset + m_selection < count) {
					m_startOffset += 1;
				}
				displayDirectory();
			}
		}
	}
	else {
		if (upDown)
			m_selection = 6;
		else
			m_selection = 1;
		fileSelectBtn[m_selection].setColorBackground(sf::Color::Blue);
	}

}

void FileMenu::movePage(bool upDown)
{
	if (upDown) {
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
	}
}

bool FileMenu::changeDirectory(const char* name)
{
	m_startOffset = 0;
	return m_fileDir.changeDirectory(name);
}

void FileMenu::moveUpDirectory()
{
	m_startOffset = 0;
	m_fileDir.moveUpDirectory();
}

void FileMenu::displayDirectory()
{
	int count = m_fileDir.readDirectory();

	for (int i = 1; i < 7; ++i) {
		fileSelectBtn[i].setText(m_fileDir.getFileName(i+m_startOffset-1));
	}
}

const char* FileMenu::selectFile(int index)
{
	const char* re = nullptr;

	if (index == -1)
		index = m_selection-1;

	if (index >= 0) {
		if (m_fileDir.isDir(index + m_startOffset)) {
				if (strncmp(m_fileDir.getFileName(index + m_startOffset), "..", 3) == 0) {
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

	return re;
}

void FileMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		for (int i = 0; i < 8; ++i)
			target.draw(fileSelectBtn[i], states);
		target.draw(fileBackground, states);
	} 
}
