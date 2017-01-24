#pragma once
#include "SFML-GUI/Button.h"

class ColorMenu : public sf::Drawable, public sf::Transformable
{
	static const int ArrowBtn = 30;
	public:
		ColorMenu();

		~ColorMenu();

		void displayColorMenu(bool visible);

		void scaleMenu(sf::Vector2f scale);

		int checkButtonPress(const sf::Vector2i pos);

		void hoverMouse(const sf::Vector2i pos);

		sf::Color getColor(int color);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		

		Button m_colorButton[4];
		Button m_arrwButton[4];

		sf::Color m_color[4];
		Rectangle m_border;
		bool m_visible = false;
};