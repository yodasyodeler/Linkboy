#pragma once
#include "SFML-GUI/Rectangle.h"

class Label : public Rectangle
{
	public:
		Label();
		Label(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color = sf::Color::Red);
		~Label();

		void setFont(const sf::Font& font);
		void setFontSize(const uint32_t size);
		void setAutoResize(const bool resize);
		void setTextColor(const sf::Color color);
		void setText(const char* text);
		void setPadding(const unsigned int padding) override;

	protected:
		virtual void adjustRectangle();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		sf::Text		m_text;
		uint32_t		m_fontSize;
		bool			m_autoResize = true;
};