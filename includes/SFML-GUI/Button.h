#pragma once
#include "SFML-GUI\Label.h"

class Button : public Label
{
	public:
		Button();
		Button(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color = sf::Color::Red);
		~Button();

		void setActive(const bool active);
		void scaleButton(const sf::Vector2f scale);
		bool isClicked(const sf::Vector2i pos);


	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
			
		bool	m_active		=	false;

		sf::Vector2f Bscale = { 1.0f, 1.0f };
};