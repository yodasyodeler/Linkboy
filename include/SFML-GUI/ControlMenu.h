#pragma once
#include "SFML-GUI/Button.h"

class ControlMenu : public sf::Drawable, public sf::Transformable
{
	public:
		ControlMenu();
		~ControlMenu();

		void setFont(sf::Font& font);

		void displayControlMenu(bool visible);

		void scaleMenu(sf::Vector2f scale);

		int checkButtonPress(const sf::Vector2i pos);

		void hoverMouse(const sf::Vector2i pos);

		int setKey(sf::Keyboard::Key key);
	
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		Button m_dpadBtn[4];
		Button m_aBtn;
		Button m_bBtn;
		Button m_selectBtn;
		Button m_startBtn;
		Button m_optionBtn[6];

		sf::Texture		m_ctrlTexture;
		sf::Sprite		m_ctrlSprite;

		int m_btnPressed;

		bool m_visible = false;

		const char* ctrlLoc = "../assets/linkboyCtrl.png";
};