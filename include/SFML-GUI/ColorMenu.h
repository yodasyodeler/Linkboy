#pragma once
#include "SFML-GUI/Button.h"

struct ColorFloat {
	float r;
	float g;
	float b;
};


class ColorMenu : public sf::Drawable, public sf::Transformable
{
	static const int ArrowBtn = 20; 

	static const ColorFloat ColorRatio[13];

	public:
		ColorMenu();

		~ColorMenu();

		void displayColorMenu(bool visible);

		void scaleMenu(sf::Vector2f scale);


		void hoverMouse(const sf::Vector2i pos);

		void handleButtonPress(const sf::Vector2i pos);

		sf::Color getColor(int color);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		int checkButtonPress(const sf::Vector2i pos);
		
		int m_selectedBtn = -1;

		Button m_colorButton[4];
		Button m_arrwButton[4];

		int m_colorIndex[4];

		sf::Color m_color[4];
		Rectangle m_border;
		bool m_visible = false;
};