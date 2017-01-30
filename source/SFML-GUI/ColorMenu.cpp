#include "SFML-GUI/ColorMenu.h"

const float ColorMenu::ColorValues[22][2] = {
	{1.0f,0.0f}, {1.0f,0.1f}, {1.0f,0.2f}, {1.0f,0.3f}, {1.0f,0.4f},
	{1.0f,0.5f}, {1.0f,0.6f}, {1.0f,0.7f}, {1.0f,0.8f}, {1.0f,0.9f},
	{1.0f,1.0f}, {0.9f,1.0f}, {0.8f,1.0f}, {0.7f,1.0f}, {0.6f,1.0f},
	{0.5f,1.0f}, {0.4f,1.0f}, {0.3f,1.0f}, {0.2f,1.0f}, {0.1f,1.0f},
	{0.0f,1.0f}, {1.0f,0.0f}
};

const float ColorMenu::GreyScale[22] = {
	1.0f,  0.95f, 0.9f,  0.85f, 0.8f,  
	0.75f, 0.7f,  0.65f, 0.6f,  0.55f, 
	0.5f,  0.45f, 0.4f,  0.35f, 0.3f,
	0.25f, 0.2f,  0.15f, 0.1f,  0.05f,
	0.0f,  1.0f 
};

ColorMenu::ColorMenu()
{
	m_border.setPos({ 0,288 - 60 });
	m_border.setSize({ 320, 60  });
	m_border.setColorForeground(sf::Color(0xCCCCCCFF));

	m_color[0] = sf::Color(0xFFFFFFFF);
	m_color[1] = sf::Color(0x909090FF);
	m_color[2] = sf::Color(0x404040FF);
	m_color[3] = sf::Color(0x000000FF);

	for (int i=0; i<4; ++i) 
		m_hue[i] = 360;
	
	m_satValIndex[0] = 0;
	m_satValIndex[1] = 14;	
	m_satValIndex[2] = 17;	
	m_satValIndex[3] = 20;

	for (int i = 0; i < 4; ++i) {
		m_colorButton[i].setPos({ (float)(10+(80*i)), (float)(288-60+10) });
		m_colorButton[i].setSize({ 60,40 });
		m_colorButton[i].setPadding(5);
		m_colorButton[i].setColorForeground(m_color[i]);
		m_colorButton[i].setColorBackground(m_color[i]);
		m_arrwButton[i].setColorForeground(m_color[i]);
		m_arrwButton[i].setSize({ (float)ArrowBtn, (float)ArrowBtn });
		m_arrwButton[i].setPadding(1);
		m_arrwButton[i].setColorBackground(sf::Color(0, 128, 128, 80));
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
			m_colorButton[0].setColorBackground(sf::Color(0, 128, 128, 80));
			break;
		case 0b0010:
			m_colorButton[1].setColorBackground(sf::Color(0, 128, 128, 80));
			break;
		case 0b0100:
			m_colorButton[2].setColorBackground(sf::Color(0, 128, 128, 80));
			break;
		case 0b1000:
			m_colorButton[3].setColorBackground(sf::Color(0, 128, 128, 80));
			break;
		default:
			break;
	}
}

bool ColorMenu::handleButtonPress(const sf::Vector2i pos)
{
	int temp = checkButtonPress(pos);
	sf::Color  tempColor;
	bool re = true;

	if (temp) {
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
			m_hue[m_selectedBtn] -= 10;
			if (m_hue[m_selectedBtn] < 0)
				m_hue[m_selectedBtn] = 360;

			break;
		case 0b00100000:
			m_satValIndex[m_selectedBtn] -= 1;
			if (m_satValIndex[m_selectedBtn] < 0)
				m_satValIndex[m_selectedBtn] = 20;
			break;
		case 0b01000000:
			m_hue[m_selectedBtn] = (10 + m_hue[m_selectedBtn]) % 370;
			break;
		case 0b10000000:
			m_satValIndex[m_selectedBtn] = (m_satValIndex[m_selectedBtn]+1) % 21;
			break;
		default:
			break;
		}

		if (m_selectedBtn != -1) {
			for (int i = 0; i < 4; ++i) {
				m_arrwButton[0].setPos({ (float)(10 + (m_selectedBtn * 80)), (float)(288 - 60 - ArrowBtn * 2) });
				m_arrwButton[1].setPos({ (float)(10 + (m_selectedBtn * 80) + ArrowBtn), (float)(288 - 60 - ArrowBtn) });
				m_arrwButton[2].setPos({ (float)(10 + (m_selectedBtn * 80) + ArrowBtn * 2), (float)(288 - 60 - ArrowBtn * 2) });
				m_arrwButton[3].setPos({ (float)(10 + (m_selectedBtn * 80) + ArrowBtn), (float)(288 - 60 - ArrowBtn * 3) });
			}

			//set current button
			if (m_hue[m_selectedBtn] == 360) {
				m_color[m_selectedBtn] = convertColor(0, 0, GreyScale[m_satValIndex[m_selectedBtn]]);
			}
			else {
				m_color[m_selectedBtn] = convertColor(m_hue[m_selectedBtn], ColorValues[m_satValIndex[m_selectedBtn]][1], ColorValues[m_satValIndex[m_selectedBtn]][0]);
			}

			m_colorButton[m_selectedBtn].setColorForeground(m_color[m_selectedBtn]);

			//Hue Left
			if (m_hue[m_selectedBtn]+10 == 360) {
				m_arrwButton[2].setColorForeground( convertColor(0, 0, GreyScale[m_satValIndex[m_selectedBtn]]) );
			}
			else {
				m_arrwButton[2].setColorForeground( convertColor(m_hue[m_selectedBtn]+10, ColorValues[m_satValIndex[m_selectedBtn]][1], ColorValues[m_satValIndex[m_selectedBtn]][0]) );
			}

			//Hue Right
			if (m_hue[m_selectedBtn]-10 < 0) {
				m_arrwButton[0].setColorForeground( convertColor(0, 0, GreyScale[m_satValIndex[m_selectedBtn]]) );
			}
			else {
				m_arrwButton[0].setColorForeground( convertColor(m_hue[m_selectedBtn]-10, ColorValues[m_satValIndex[m_selectedBtn]][1], ColorValues[m_satValIndex[m_selectedBtn]][0]) );
			}


			//Turn satBright Down
			temp = m_satValIndex[m_selectedBtn]-1;	
			temp = (temp < 0) ? 20 : temp;		
			if (m_hue[m_selectedBtn] == 360) {
				m_arrwButton[1].setColorForeground( convertColor(0, 0, GreyScale[temp]) );
			}
			else {
				m_arrwButton[1].setColorForeground( convertColor(m_hue[m_selectedBtn], ColorValues[temp][1], ColorValues[temp][0]) );
			}


			//Turn satBright Up
			if (m_hue[m_selectedBtn] == 360) {
				m_arrwButton[3].setColorForeground( convertColor(0, 0, GreyScale[m_satValIndex[m_selectedBtn]+1]) );
			}
			else {
				m_arrwButton[3].setColorForeground( convertColor(m_hue[m_selectedBtn], ColorValues[m_satValIndex[m_selectedBtn]+1][1], ColorValues[m_satValIndex[m_selectedBtn]+1][0]) );
			}


			//Btn backgrounds
			for (int i = 0; i < 4; ++i)
				if (i != m_selectedBtn)
					m_colorButton[i].setColorBackground(m_color[i]);
		}
	}
	else 
		re = false;

	 return re;
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

const sf::Color ColorMenu::convertColor(int hue, float saturation, float brightness)
{
	uint8_t red, green, blue;
	float R,G,B;
	float C = brightness * saturation;
	float X = ((float)hue/60);
	while (X >= 2)
		X -= 2;
	X -= 1;
	X = (X<0) ? 1+X : 1-X;
	X *= C;
	float M = brightness - C;
 
	if (hue > 360)
		hue = 0;

	if (saturation <= 0) {
		R = brightness;
		G = brightness;
		B = brightness;
	}
	else {
		if (hue < 60) {
			R = C;
			G = X;
			B = 0;
		}
		else if (hue < 120) {
			R = X;
			G = C;
			B = 0;
		}
		else if (hue < 180) {
			R = 0;
			G = C;
			B = X;
		}
		else if (hue < 240) {
			R = 0;
			G = X;
			B = C;
		}
		else if (hue < 300) {
			R = X;
			G = 0;
			B = C;
		}
		else if (hue < 360){
			R = C;
			G = 0;
			B = X;
		}
	}

	red = (uint8_t)(R+M)*255;
	green = (uint8_t)(G+M)*255;
	blue = (uint8_t)(B+M)*255;

	return sf::Color(red, green, blue);
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
