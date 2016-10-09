#include "SFML-GUI\Rectangle.h"

Rectangle::Rectangle()
	:m_foreground(sf::Triangles, 6), m_background(sf::Triangles, 6)
{
	for (int i = 0; i < 6; ++i) {
		m_foreground[i].color = sf::Color::Black;
		m_background[i].color = sf::Color::Black;
	}
}

Rectangle::Rectangle(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color)
	:m_foreground(sf::Triangles, 6), m_background(sf::Triangles, 6)
{
	m_origin = pos;
	m_length = size;

	m_pos = pos;
	m_size = size;

	adjustRectangle();

	for (int i = 0; i < 6; ++i)
		m_foreground[i].color = color;
}

Rectangle::~Rectangle()
{
}

void Rectangle::setPos(const sf::Vector2f pos)
{
	m_pos = pos;
	m_origin = pos;

	adjustRectangle();
}

void Rectangle::setSize(const sf::Vector2f size)
{
	m_size = size;
	m_length = size;

	adjustRectangle();
}

void Rectangle::setColorForeground(const sf::Color color)
{
	for (int i = 0; i < 6; ++i)
		m_foreground[i].color = color;
}

void Rectangle::setColorBackground(const sf::Color color)
{
	for (int i = 0; i < 6; ++i)
		m_background[i].color = color;
}


void Rectangle::setVisible(const bool visible)
{
	m_visible = visible;
}

void Rectangle::setPadding(const unsigned int padding)
{
	m_padding = padding;
	adjustRectangle();
}

void Rectangle::setScale(sf::Vector2f scale)
{
	m_scale = scale;
	adjustRectangle();
}


void Rectangle::adjustRectangle()
{
	m_pos = { m_origin.x * m_scale.x, m_origin.y * m_scale.y };
	m_size = { m_length.x * m_scale.x, m_length.y * m_scale.y };

	m_background[0].position = sf::Vector2f(m_pos.x, m_pos.y);
	m_foreground[0].position = sf::Vector2f(m_pos.x + m_padding, m_pos.y + m_padding);

	m_background[1].position = sf::Vector2f(m_pos.x + m_size.x, m_pos.y);
	m_foreground[1].position = sf::Vector2f(m_pos.x + m_size.x - m_padding, m_pos.y + m_padding);

	m_background[2].position = sf::Vector2f(m_pos.x, m_pos.y + m_size.y);
	m_foreground[2].position = sf::Vector2f(m_pos.x + m_padding, m_pos.y + m_size.y - m_padding);

	m_background[3].position = sf::Vector2f(m_pos.x, m_pos.y + m_size.y);
	m_foreground[3].position = sf::Vector2f(m_pos.x + m_padding, m_pos.y + m_size.y - m_padding);

	m_background[4].position = sf::Vector2f(m_pos.x + m_size.x, m_pos.y);
	m_foreground[4].position = sf::Vector2f(m_pos.x + m_size.x - m_padding, m_pos.y + m_padding);

	m_background[5].position = sf::Vector2f(m_pos.x + m_size.x, m_pos.y + m_size.y);
	m_foreground[5].position = sf::Vector2f(m_pos.x + m_size.x - m_padding, m_pos.y + m_size.y - m_padding);
}

void Rectangle::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (m_visible) {
		states.transform *= getTransform();

		if (m_padding != 0)
			target.draw(m_background, states);
		if (m_padding < m_size.x && m_padding < m_size.y)
			target.draw(m_foreground, states);
	}
}
