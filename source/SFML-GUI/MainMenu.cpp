#include "SFML-GUI/MainMenu.h"

MainMenu::MainMenu()
{
	dPadTexture.loadFromFile(dPad);
	menuDPadSprite.setTexture(dPadTexture);
	menuDPadSprite.setPosition({ 32,16 });

	//Cross Buttons
	for (int i = 0; i < 4; ++i) 
		m_menu[i].setSize({ 86, 86 });

	m_menu[0].setPos({ 32,101 });
	m_menu[1].setPos({ 117,16 });
	m_menu[2].setPos({ 203,101 });
	m_menu[3].setPos({ 117,186 });

	for (int i = 0; i < 4; ++i) {
		m_label[i].setTextColor(sf::Color::White);
		m_label[i].setSize({ 50, 30 });
	}
	m_label[0].setPos({ 32+22,101-30 });
	m_label[1].setPos({ 117+22,16-30 });
	m_label[2].setPos({ 203+22,101-30 });
	m_label[3].setPos({ 117+22,186+80 });

	m_label[0].setText("Load");
	m_label[1].setText("Layout");
	m_label[2].setText("Link");
	m_label[3].setText("Exit");
}

MainMenu::MainMenu(const sf::Font & font) //const unsigned int width, const unsigned int height, 
{
	dPadTexture.loadFromFile(dPad);
	menuDPadSprite.setTexture(dPadTexture);
	menuDPadSprite.setPosition({ 32,16 });

	//Cross Buttons
	for (int i = 0; i < 4; ++i) 
		m_menu[i].setSize({ 86, 86 });

	m_menu[0].setPos({ 32,101 });
	m_menu[1].setPos({ 117,16 });
	m_menu[2].setPos({ 203,101 });
	m_menu[3].setPos({ 117,186 });

	for (int i = 0; i < 4; ++i) {
		m_label[i].setFont(font);
		m_label[i].setTextColor(sf::Color::White);
		m_label[i].setSize({ 50, 30 });
	}
	m_label[0].setPos({ 32+22,101-30 });
	m_label[1].setPos({ 117+22,16-30 });
	m_label[2].setPos({ 203+22,101-30 });
	m_label[3].setPos({ 117+22,186+80 });

	m_label[0].setText("Layout");
	m_label[1].setText("Load");
	m_label[2].setText("Link");
	m_label[3].setText("Exit");

	
	//menuDPadSprite.setScale({.8f,.8f});
}

MainMenu::~MainMenu()
{
}

void MainMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i<4; ++i)
		m_menu[i].scaleButton(scale);
}

void MainMenu::setFont(const sf::Font & font)
{
	for (int i=0; i<4; ++i)
		m_label[i].setFont(font);
}

void MainMenu::displayMainMenu(bool visible)
{
	m_visible = visible;
	for (int i = 0; i < 4; ++i) {
		m_menu[i].setActive(visible);
		m_label[i].setVisible(visible);
	}
}

int MainMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i = 0; i < 4; ++i)
		if (m_menu[i].isClicked(pos))
			re |= (1<<i);

	return re;
}

void MainMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		target.draw(menuDPadSprite, states);
		for (int i = 0; i < 4; ++i)
			target.draw(m_label[i], states);
	}
}
