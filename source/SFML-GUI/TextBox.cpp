#include "SFML-GUI/TextBox.h"

TextBox::TextBox()
	:Button()
{}

TextBox::TextBox(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color)
	:Button(pos, size, color)
{
}

TextBox::~TextBox()
{
}

bool TextBox::isClicked(const sf::Vector2i pos)
{
	m_selected = Button::isClicked(pos);
	if (m_selected)
		m_buffer[m_index] = '|';
	else
		m_buffer[m_index] = '\0';
	Button::setText(m_buffer);

	return m_selected;
}

void TextBox::keyPress(sf::Keyboard::Key key)
{
	if (m_selected && m_index < m_bufferMax) {
		if (key == 13) {
			m_selected = false;
			m_buffer[m_index] = '\0';
		}
		else if (key == '\b' && m_index > 0) {
			m_buffer[m_index--] = '\0';
			m_buffer[m_index] = '|';
		}
		else if (key == 8 || key == 11) {
		}
		else if (m_index < 255) {
			m_buffer[m_index++] = key;
			m_buffer[m_index] = '|';
		}
		Button::setText(m_buffer);

		if (m_checkRule(m_buffer, m_index)) {
			m_validEntry = true;
			setColorBackground(sf::Color::Black);
		}
		else {
			m_validEntry = false;
			setColorBackground(sf::Color::Red);
		}
	}
}

const char * TextBox::getBuffer()
{
	return m_buffer;
}

void TextBox::setBuffer(const char * buffer)
{
	lb_strcpy(m_buffer, buffer);
	setText(buffer);
	m_index = lb_strlen(m_buffer);
}

void TextBox::clearBuffer()
{
	int i = 0;
	while (m_buffer[i] != '\0')
		m_buffer[i++] = '\0';
	m_index = 0;
}

void TextBox::setTextRule(bool(*checkRule)(char *, int))
{
	m_checkRule = checkRule;
}

bool TextBox::isValid()
{
	return m_validEntry;
}

void TextBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	Button::draw(target, states);
}

bool acceptAllRule(char * buffer, int len)
{
	return true;
}
