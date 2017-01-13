#pragma once
#include "SFML-GUI/Button.h"
#include "linkboyClient.h"

class LobbyMenu : public sf::Drawable, public sf::Transformable
{
public:
	LobbyMenu();
	~LobbyMenu();

	void setFont(sf::Font& font);

	void displayLobbyMenu(bool visible);

	void scaleMenu(sf::Vector2f scale);

	int checkButtonPress(const sf::Vector2i pos);

	void hoverMouse(const sf::Vector2i pos);

	void moveSelection(bool upDown);
	void movePage(bool upDown);

	bool changeLobby(const int lobby);
	void displayLobby();
	const char* selectLobby(int index);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	Button m_lobbySelectBtn[8];

	Client* m_client;

	int m_selection = 0;
	int m_startOffset = 0;

	bool m_visible = false;
};