#pragma once
#include "SFML-GUI/Button.h"
#include "common.h"

bool acceptAllRule(char * buffer, int lwn);

class TextBox : public Button
{
public:
	TextBox();
	TextBox(const sf::Vector2f pos, const sf::Vector2f size, const sf::Color color = sf::Color::Red);
	~TextBox();

	bool isClicked(const sf::Vector2i pos) override;

	void keyPress(sf::Keyboard::Key key);

	const char* getBuffer();
	void setBuffer( const char* buffer);
	void  clearBuffer();

	void setTextRule(bool(*checkRule)(char*, int));
	bool isValid();

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	static const int m_bufferMax = 256; 

	bool m_selected = false;
	bool m_validEntry = false;
	int  m_index;
	char m_buffer[m_bufferMax] = {};
	
	bool (*m_checkRule)(char*, int) = acceptAllRule;
};
