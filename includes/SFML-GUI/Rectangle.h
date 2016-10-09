#pragma once
#include <SFML/Graphics.hpp>

class Rectangle : public sf::Drawable, public sf::Transformable
{
	public:
		Rectangle();
		Rectangle(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color = sf::Color::Red);
		~Rectangle();

		virtual void setPos(const sf::Vector2f pos);
		virtual void setSize(const sf::Vector2f size);
		virtual void setColorForeground(const sf::Color color);
		virtual void setColorBackground(const sf::Color color);

		virtual void setVisible(const bool visible);
		virtual void setPadding(const unsigned int padding);
		virtual void setScale(sf::Vector2f scale);

	protected:
		virtual void adjustRectangle();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		unsigned int		m_padding		=	0;
		sf::Vector2f		m_scale			=	{1.0f,1.0f};

		sf::Vector2f		m_pos			=	{};
		sf::Vector2f		m_size			=	{};

		sf::Vector2f		m_origin		=	{};
		sf::Vector2f		m_length		=	{};

		sf::VertexArray		m_foreground;
		sf::VertexArray		m_background;

		bool				m_visible		=	false;
};

