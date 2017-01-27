#pragma once
#include "SFML-GUI/Button.h"

class ColorMenu : public sf::Drawable, public sf::Transformable
{
	static const int ArrowBtn = 20; 
	static const float ColorValues[21][2];

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

		const sf::Color convertColor(int hue, float saturation, float brightnesslight);

		int checkButtonPress(const sf::Vector2i pos);
		
		int m_selectedBtn = -1;

		Button m_colorButton[4];
		Button m_arrwButton[4];

		int m_hue[4] = {0,0,0,0};
		int m_satValIndex[4] = {10,10,10,10};

		sf::Color m_color[4];
		Rectangle m_border;
		
		bool m_visible = false;
};