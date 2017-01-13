#include "SFML-GUI/Button.h"
#include "Directory.h"

class FileMenu : public sf::Drawable, public sf::Transformable
{
	public:
		FileMenu(const char* dirName = nullptr);
		FileMenu(const sf::Font& font, const char* dirName = nullptr);
		~FileMenu();

		void setFont(sf::Font& font);

		void displayFileMenu(bool visible);

		void scaleMenu(sf::Vector2f scale);

		int checkButtonPress(const sf::Vector2i pos);

		void hoverMouse(const sf::Vector2i pos);

		void moveSelection(bool upDown);
		void movePage(bool upDown);

		bool changeDirectory(const char* name);
		void moveUpDirectory();
		void displayDirectory();
		const char* selectFile(int index);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		bool		m_visible	=	false;

		Button fileSelectBtn[8];
		//Button fileNavBtn[3];
		//Rectangle fileBackground;//({ 0,0 }, {40,288}, sf::Color::Black);

		int m_selection = 0;
		int m_startOffset = 0;
		//Button fileArrws[2];
		Directory m_fileDir;
};