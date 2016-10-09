#include "Button.h"

class MainMenu : public sf::Drawable, public sf::Transformable
{
	public:
		MainMenu();
		MainMenu(const sf::Font& font);	//const unsigned int width, const unsigned int height, 
		~MainMenu();

		void scaleMenu(sf::Vector2f scale);

		void setFont(sf::Font& font);

		void displayMainMenu(bool visible);

		int checkButtonPress(const sf::Vector2i pos);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		Button		m_menu[4];
		Label		m_label[4];

		sf::Texture		dPadTexture;
		sf::Sprite		menuDPadSprite;

		bool			m_visible	=	false;

		const char* dPad = "../assets/dPad.png";
};