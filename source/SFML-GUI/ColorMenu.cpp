#include "SFML-GUI/ColorMenu.h"

const ColorFloat ColorMenu::ColorRatio[13]  = {
	{1,0,0},  {1,.5,0}, {1,1,0},			//red->yellow
	{.5,1,0}, {0,1,0},  {0,1,.5},			//Greens
	{0,1,1},  {0,.5,1}, {0,0,1},			//Blue
	{.5,0,1}, {1,0,1},  {1,0,.5},			//Purple
	{.5,.5,.5}								//Grey
};

ColorMenu::ColorMenu()
{
	m_border.setPos({ 0,288 - 60 });
	m_border.setSize({ 320, 60  });
	m_border.setColorForeground(sf::Color(0xCCCCCCFF));

	m_color[0] = sf::Color::Red;
	m_color[1] = sf::Color::Blue;
	m_color[2] = sf::Color::Green;
	m_color[3] = sf::Color::Black;

	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setPos({ (float)(10+(80*i)), (float)(288-60+10) });
		m_colorButton[i].setSize({ 60,40 });
		m_colorButton[i].setPadding(5);
		m_colorButton[i].setColorForeground(m_color[i]);
		m_colorButton[i].setColorBackground(m_color[i]);
		m_arrwButton[i].setColorForeground(m_color[i]);
		m_arrwButton[i].setSize({ (float)ArrowBtn, (float)ArrowBtn });
	}


	m_arrwButton[0].setPos({ (float)(10+(80)), (float)(288-60-ArrowBtn*2) });
	m_arrwButton[1].setPos({ (float)(10+(80)+ArrowBtn), (float)(288-60-ArrowBtn) });
	m_arrwButton[2].setPos({ (float)(10+(80)+ArrowBtn*2), (float)(288-60-ArrowBtn*2) });
	m_arrwButton[3].setPos({ (float)(10+(80)+ArrowBtn), (float)(288-60-ArrowBtn*3) });

}

ColorMenu::~ColorMenu()
{
}

void ColorMenu::displayColorMenu(bool visible)
{
	m_visible = visible;
	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setVisible(visible);
		m_colorButton[i].setActive(visible);
		m_arrwButton[i].setVisible(visible);
		m_arrwButton[i].setActive(visible);
	}
	m_border.setVisible(visible);
}

void ColorMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].scaleButton(scale);
		m_arrwButton[i].scaleButton(scale);
	}
}

void ColorMenu::hoverMouse(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);

	for (int i = 0; i < 4; ++i)
		if (i != m_selectedBtn)
			m_colorButton[i].setColorBackground(m_color[i]);

	switch (temp){
		case 0b0001:
			m_colorButton[0].setColorBackground(sf::Color(m_color[0].r, m_color[0].g, m_color[0].b, 80));
			break;
		case 0b0010:
			m_colorButton[1].setColorBackground(sf::Color(m_color[1].r, m_color[1].g, m_color[1].b, 80));
			break;
		case 0b0100:
			m_colorButton[2].setColorBackground(sf::Color(m_color[2].r, m_color[2].g, m_color[2].b, 80));
			break;
		case 0b1000:
			m_colorButton[3].setColorBackground(sf::Color(m_color[3].r, m_color[3].g, m_color[3].b, 80));
			break;
		default:
			break;
	}
}

void ColorMenu::handleButtonPress(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);


	switch (temp) {
		case 0b00000001:
			if (m_selectedBtn != 0)
				m_selectedBtn = 0;
			else
				m_selectedBtn = -1;
	
			break;
		case 0b00000010:
			if (m_selectedBtn != 1)
				m_selectedBtn = 1;
			else
				m_selectedBtn = -1;

			break;
		case 0b00000100:
			if (m_selectedBtn != 2)
				m_selectedBtn = 2;
			else 
				m_selectedBtn = -1;

			break;
		case 0b00001000:
			if (m_selectedBtn != 3)
				m_selectedBtn = 3;
			else 
				m_selectedBtn = -1;

			break;
		case 0b00010000:
			if (--m_colorIndex[m_selectedBtn] < 0)
				m_colorIndex[m_selectedBtn] = 12;
			m_color[m_selectedBtn].r =  ColorRatio[(m_colorIndex[m_selectedBtn])].r*255;
			m_color[m_selectedBtn].g =  ColorRatio[(m_colorIndex[m_selectedBtn])].g*255;
			m_color[m_selectedBtn].b =  ColorRatio[(m_colorIndex[m_selectedBtn])].b*255;


			m_colorButton[m_selectedBtn].setColorForeground(m_color[m_selectedBtn]);

			break;
		case 0b00100000:
			//m_arrwButton[1].setColorForeground();

			break;
		case 0b01000000:
			m_colorIndex[m_selectedBtn] = (m_colorIndex[m_selectedBtn] + 1) % 13;
			m_color[m_selectedBtn].r =  ColorRatio[(m_colorIndex[m_selectedBtn])].r*255;
			m_color[m_selectedBtn].g =  ColorRatio[(m_colorIndex[m_selectedBtn])].g*255;
			m_color[m_selectedBtn].b =  ColorRatio[(m_colorIndex[m_selectedBtn])].b*255;

			m_colorButton[m_selectedBtn].setColorForeground(m_color[m_selectedBtn]);

			break;
		case 0b10000000:
			//m_arrwButton[3].setColorForeground();

			break;
		default:
			break;
	}

	if (m_selectedBtn != -1) {
		for (int i = 0; i < 4; ++i) {
			m_arrwButton[0].setPos({ (float)(10+(m_selectedBtn*80)), (float)(288-60-ArrowBtn*2) });
			m_arrwButton[1].setPos({ (float)(10+(m_selectedBtn*80)+ArrowBtn), (float)(288-60-ArrowBtn) });
			m_arrwButton[2].setPos({ (float)(10+(m_selectedBtn*80)+ArrowBtn*2), (float)(288-60-ArrowBtn*2) });
			m_arrwButton[3].setPos({ (float)(10+(m_selectedBtn*80)+ArrowBtn), (float)(288-60-ArrowBtn*3) });
		}

		for (int i = 0; i < 4; ++i)
			if (i != m_selectedBtn)
				m_colorButton[i].setColorBackground(m_color[i]);
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
		if (m_selectedBtn != -1) {
			for (int i = 0; i < 4; ++i)
				target.draw(m_arrwButton[i], states);
		}
	}
}

int ColorMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;

	for (int i=0; i<4; ++i) 
		if (m_colorButton[i].isClicked(pos))
			re |= (1<<i);

	for (int i=4; i<8; ++i) 
		if (m_arrwButton[i-4].isClicked(pos))
			re |= (1<<i);

	return re;
}
