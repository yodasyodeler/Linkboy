#include "SFML-GUI/ColorMenu.h"

ColorMenu::ColorMenu()
{
	m_border.setPos({ 0,288 - 60 });
	m_border.setSize({ 320, 60  });
	m_border.setColorForeground(sf::Color(0xCCCCCCFF));

	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setPos({ (float)(15+(80*i)), (float)(288-60+15) });
		m_colorButton[i].setSize({ 80,60 });
		m_colorButton[i].setPadding(5);
	}

	m_color[1] = sf::Color::Red;
	m_color[2] = sf::Color::Blue;
	m_color[3] = sf::Color::Green;
	m_color[4] = sf::Color::Black;

	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setColorForeground(m_color[i]);
		m_colorButton[i].setColorBackground(m_color[i]);
	}
}

ColorMenu::~ColorMenu()
{
}

void ColorMenu::displayControlMenu(bool visible)
{
	m_visible = visible;
	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setVisible(visible);
		m_colorButton[i].setActive(visible);
	}
	m_border.setVisible(visible);
}

void ColorMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i<4; ++i)
		m_colorButton[i].scaleButton(scale);
}

int ColorMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i=0; i<4; ++i) 
		if (m_colorButton[i].isClicked(pos))
			re |= (1<<i);

	return re;
}

void ColorMenu::hoverMouse(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);

	for (int i = 0; i < 4; ++i)
		m_colorButton[i].setColorBackground(m_color[i]);

	switch (temp){
		case 0b0001:
			m_colorButton[0].setColorBackground(sf::Color(m_color[0].r, m_color[0].g, m_color[0].b, 125));
			break;
		case 0b0010:
			m_colorButton[1].setColorBackground(sf::Color(m_color[1].r, m_color[1].g, m_color[1].b, 125));
			break;
		case 0b0100:
			m_colorButton[2].setColorBackground(sf::Color(m_color[2].r, m_color[2].g, m_color[2].b, 125));
			break;
		case 0b1000:
			m_colorButton[3].setColorBackground(sf::Color(m_color[3].r, m_color[3].g, m_color[3].b, 125));
			break;
		default:
			break;
	}
}

sf::Color ColorMenu::getColor(int color)
{
	return m_color[color];
}

void ColorMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		target.draw(m_border, states);
		for (int i=0; i<4; ++i)
			target.draw(m_colorButton[i],states);
	}
}

