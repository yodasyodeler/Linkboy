#include "SFML-GUI/ControlMenu.h"

static const float scOff = 26;
static const float dpadOff = 195;
static const float dpadW = 26;

static const float miscBtnH = 55;

ControlMenu::ControlMenu()
{
	m_ctrlTexture.loadFromFile(ctrlLoc);
	m_ctrlSprite.setTexture(m_ctrlTexture);
	m_ctrlSprite.setPosition({ 0,0 });

	//Misc
	for (int i=0; i<3; ++i) {
		m_optionBtn[i].setSize({ miscBtnH,miscBtnH });
		m_optionBtn[i].setPos({ (float)(miscBtnH*i+6*i+scOff+55), 48 });
		m_optionBtn[i].setColorForeground(sf::Color::Red);
		m_optionBtn[i].setText("misc");
	}
	for (int i=0; i<3; ++i) {
		m_optionBtn[i+3].setSize({ miscBtnH,miscBtnH });
		m_optionBtn[i+3].setPos({ (float)(miscBtnH*i+6*i+scOff+55), 48+miscBtnH+10 });
		m_optionBtn[i+3].setColorForeground(sf::Color::Red);
		m_optionBtn[i+3].setText("misc");
	}

	//Dpad
	for (int i=0; i<4; ++i) {
		m_dpadBtn[i].setSize({ dpadW,dpadW });
		m_dpadBtn[i].setColorForeground(sf::Color::Transparent);
	}
	m_dpadBtn[0].setPos({ scOff+dpadW		,dpadOff });			//up
	m_dpadBtn[1].setPos({ scOff				,dpadOff+dpadW });		//left
	m_dpadBtn[2].setPos({ scOff+dpadW		,dpadOff+dpadW+dpadW });//down
	m_dpadBtn[3].setPos({ scOff+(dpadW*2)	,dpadOff+dpadW });		//right

	m_dpadBtn[0].setText("Up");
	m_dpadBtn[1].setText("Left");
	m_dpadBtn[2].setText("Down");
	m_dpadBtn[3].setText("Right");

	//Buttons
	m_aBtn.setSize({ 46,46 });
	m_aBtn.setPos({ 160+45+52, 196 });
	m_aBtn.setColorForeground(sf::Color::Transparent);	
	m_aBtn.setText("X");

	m_bBtn.setSize({ 46,46 });
	m_bBtn.setPos({ 160+47, 212 });
	m_bBtn.setColorForeground(sf::Color::Transparent);	
	m_bBtn.setText("Z");
	
	m_selectBtn.setSize({ 38,22 });
	m_startBtn.setSize({ 38,22 });
	
	m_selectBtn.setColorForeground(sf::Color::Transparent);
	m_startBtn.setColorForeground(sf::Color::Transparent);

	m_selectBtn.setPos({ 160-42,262 });
	m_startBtn.setPos({  160+5,262 });

	m_selectBtn.setText("LShift");
	m_startBtn.setText("Enter");
}

ControlMenu::~ControlMenu()
{

}

void ControlMenu::setFont(sf::Font& font)
{
	for (int i=0; i<4; ++i)
		m_dpadBtn[i].setFont(font);
	for (int i=0; i<6; ++i)
		m_optionBtn[i].setFont(font);

	m_aBtn.setFont(font);
	m_bBtn.setFont(font);
	m_selectBtn.setFont(font);
	m_startBtn.setFont(font);

}

void ControlMenu::displayControlMenu(bool visible)
{
	m_visible = visible;

	for (int i=0; i<4; ++i) {
		m_dpadBtn[i].setVisible(visible);
		m_dpadBtn[i].setActive(visible);
	}
	// for (int i=0; i<6; ++i) {
	// 	m_optionBtn[i].setVisible(visible);
	// 	m_optionBtn[i].setActive(visible);
	// }
	m_aBtn.setVisible(visible);
	m_aBtn.setActive(visible);

	m_bBtn.setVisible(visible);
	m_bBtn.setActive(visible);

	m_selectBtn.setVisible(visible);
	m_selectBtn.setActive(visible);

	m_startBtn.setVisible(visible);
	m_startBtn.setActive(visible);
}

void ControlMenu::scaleMenu(sf::Vector2f scale)
{
	for (int i=0; i<4; ++i)
		m_dpadBtn[i].scaleButton(scale);
	for (int i=0; i<6; ++i)
		m_optionBtn[i].scaleButton(scale);

	m_aBtn.scaleButton(scale);
	m_bBtn.scaleButton(scale);
	m_selectBtn.scaleButton(scale);
	m_startBtn.scaleButton(scale);
}

int ControlMenu::checkButtonPress(const sf::Vector2i pos)
{
	int re = 0;
	
	for (int i=0; i<6; ++i)
		if (m_optionBtn[i].isClicked(pos))
			re |= (1<<i);

	for (int i=0; i<4; ++i)
		if (m_dpadBtn[i].isClicked(pos))
			re |= ((1<<i)<<6);

	if (m_aBtn.isClicked(pos))
		re |= (1<<10);
	if (m_bBtn.isClicked(pos))
		re |= (1<<11);
	if (m_startBtn.isClicked(pos))
		re |= (1<<12);
	if (m_selectBtn.isClicked(pos))
		re |= (1<<13);

	m_btnPressed = re;

	return re;
}

void ControlMenu::hoverMouse(const sf::Vector2i pos)
{
	//int temp = checkButtonPress(pos);
}

int ControlMenu::setKey(sf::Keyboard::Key key)
{
	int re = m_btnPressed;
	static const char ControlKeys [][12] = {
		"A", "B", "C", 
		"D", "E", "F", "G", 
		"H", "I", "J", "K", 
		"L", "M", "N", "O", 
		"P", "Q", "R", "S", 
		"T", "U", "V", "W", 
		"X", "Y", "Z", "Num0", 
		"Num1", "Num2", "Num3", "Num4", 
		"Num5", "Num6", "Num7", "Num8", 
		"Num9", "Escape",  "LControl", "LShift", 
		"LAlt", "LSystem", "RControl", "RShift", 
		"RAlt", "RSystem", "Menu", "LBracket", 
		"RBracket", "SemiColon", "Comma", "Period", 
		"Quote", "Slash", "BackSlash", "Tilde", 
		"Equal", "Dash", "Space", "Return", 
		"BackSpace", "Tab", "PageUp", "PageDown", 
		"End", "Home", "Insert", "Delete", 
		"Add", "Subtract", "Multiply", "Divide", 
		"Left", "Right", "Up", "Down", 
		"Numpad0", "Numpad1", "Numpad2", "Numpad3", 
		"Numpad4", "Numpad5", "Numpad6", "Numpad7", 
		"Numpad8", "Numpad9", "F1", "F2", 
		"F3", "F4", "F5", "F6", 
		"F7", "F8", "F9", "F10", 
		"F11", "F12", "F13", "F14", 
		"F15", "Pause"
	};
 
	switch (m_btnPressed) {
		case (1<<0):
			m_optionBtn[0].setText(ControlKeys[key]);
			break;
		case (1<<1):
			m_optionBtn[1].setText(ControlKeys[key]);
			break;
		case (1<<2):
			m_optionBtn[2].setText(ControlKeys[key]);
			break;
		case (1<<3):
			m_optionBtn[3].setText(ControlKeys[key]);
			break;
		case (1<<4):
			m_optionBtn[4].setText(ControlKeys[key]);
			break;
		case (1<<5):
			m_optionBtn[5].setText(ControlKeys[key]);
			break;
		case (1<<6):
			m_dpadBtn[0].setText(ControlKeys[key]);
			break;
		case (1<<7):
			m_dpadBtn[1].setText(ControlKeys[key]);
			break;
		case (1<<8):
			m_dpadBtn[2].setText(ControlKeys[key]);
			break;
		case (1<<9):
			m_dpadBtn[3].setText(ControlKeys[key]);
			break;
		case (1<<10):
			m_aBtn.setText(ControlKeys[key]);
			break;
		case (1<<11):
			m_bBtn.setText(ControlKeys[key]);
			break;
		case (1<<12):
			m_startBtn.setText(ControlKeys[key]);
			break;
		case (1<<13):
			m_selectBtn.setText(ControlKeys[key]);
			break;
		default:
			re = 0;
			break;
	};
	
	return re; 
}


void ControlMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		target.draw(m_ctrlSprite, states);

		for (int i=0; i<4; ++i)
			target.draw(m_dpadBtn[i], states);
		for (int i=0; i<6; ++i)
			target.draw(m_optionBtn[i], states);

		target.draw(m_aBtn, states);
		target.draw(m_bBtn, states);
		target.draw(m_selectBtn, states);
		target.draw(m_startBtn, states);
	} 
}