#include "SFML-GUI/Label.h"

Label::Label()
	:Rectangle()
{
	m_text.setScale({ .5f, .5f });
}

Label::Label(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color)
	:Rectangle(pos, size, color)
{
	m_text.setScale({ .5f, .5f });
}

Label::~Label()
{}

void Label::setFont(const sf::Font& font)
{
	m_text.setFont(font);
}

void Label::setFontSize(const uint32_t size)
{
	if (m_autoResize == false) {
		if (size > 0)
			m_fontSize = size;
		else
			m_fontSize = 0;

		m_text.setCharacterSize(m_fontSize);
	}
}

void Label::setAutoResize(const bool resize)
{
	m_autoResize = resize;
	if (m_autoResize)
		adjustRectangle();
}

void Label::setTextColor(const sf::Color color)
{
	m_text.setColor(color);
}

void Label::setText(const char* text)
{
	m_text.setString(text);
}

void Label::setPadding(const unsigned int padding)
{
	m_padding = padding;
	adjustRectangle();
}

void Label::adjustRectangle()
{
	Rectangle::adjustRectangle();

	if (m_autoResize) {
		m_fontSize = (int)(((m_size.y) < 0) ? 0 : (m_size.y));
		m_text.setCharacterSize(m_fontSize);
		m_text.setPosition({ m_pos.x + 6, (m_size.y / 4) + m_pos.y });
	}
}

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	Rectangle::draw(target, states);
	if (m_text.getFont() != nullptr && m_visible)
		target.draw(m_text, states);
}
