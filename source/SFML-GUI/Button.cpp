#include "SFML-GUI\Button.h"

Button::Button()
	:Label()
{}
Button::Button(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color)
	:Label(pos, size, color)
{
}

Button::~Button()
{}


void Button::setActive(const bool active)
{
	m_active = active;
}

void Button::scaleButton(const sf::Vector2f scale)
{
	Bscale = scale;
}

bool Button::isClicked(const sf::Vector2i pos)
{
	bool re = false;



	sf::Vector2f m_ButtonPos = {m_pos.x*Bscale.x, m_pos.y*Bscale.y};
	sf::Vector2f m_ButtonSize = {m_size.x*Bscale.x, m_size.y*Bscale.y};

	if (m_active)
		if (m_ButtonPos.x <= pos.x  && m_ButtonPos.y <= pos.y)
			if ((m_ButtonPos.x + m_ButtonSize.x >= pos.x) && (m_ButtonPos.y + m_ButtonSize.y >= pos.y))
				re = true;

	return re;
}



void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	Label::draw(target, states);
}
