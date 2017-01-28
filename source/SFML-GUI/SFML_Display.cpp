#include "Display.h"
#include "SFML-GUI/MainMenu.h"
#include "SFML-GUI/FileMenu.h"
#include "SFML-GUI/NetworkMenu.h"
#include "SFML-GUI/LobbyMenu.h"
#include "SFML-GUI/ControlMenu.h"
#include "SFML-GUI/ColorMenu.h"

using sf::Keyboard;

const char helpText[] = {
	"---Linkboy help---\n"   \
	"Use arrow keys or mouse to\n navigate menu\n" \
	"\t Esc - Menu\n"        \
	"\t Arrow Keys - dpad\n" \
	"\t X - A button\n"      \
	"\t Y - B button\n"      \
	"\t return - Start button\n" \
	"\t shift - Select button\n" 
};

static sf::RenderWindow window;
static sf::Texture texture;
static sf::Sprite sprite;


static const uint8_t* pixels; 

// Keys for Use
static Keyboard::Key	UP		=	Keyboard::Up;
static Keyboard::Key	DOWN	=	Keyboard::Down;
static Keyboard::Key	LEFT	=	Keyboard::Left;
static Keyboard::Key	RIGHT	=	Keyboard::Right;

static Keyboard::Key	A		=	Keyboard::X;
static Keyboard::Key	B		=	Keyboard::Z;
static Keyboard::Key	START	=	Keyboard::Return;
static Keyboard::Key	SELECT	=	Keyboard::RShift;

static Keyboard::Key	MENU	=	Keyboard::Escape;

static bool pauseFocus	= false;
static bool showMenu = false;
static bool showHelp = false;
static int displaySpeed = 0;

static sf::Font font;
static Label text;

static Rectangle background;

//Help Screen
static Label helpMenu;

//Main Menu
static MainMenu mainMenu;

//file Menu
static FileMenu fileMenu;

//color Menu
static ColorMenu colorMenu;

//network Menu
static NetworkMenu networkMenu;

//lobby Menu not in place
static LobbyMenu lobbyMenu;

//control Menu
static ControlMenu controlMenu;
static bool changeKey = false;

static const char* SPEED_OUTPUT[11] {
	"25%",
	"50%",
	"75%",
	"100%",
	"150%",
	"200%",
	"300%",
	"400%",
	"500%",
	"750%",
	"1000%"
};

void handleGame(const sf::Event& event, emulatorSettings& settings);

void handleMenu(const sf::Event& event, emulatorSettings& settings);

void handleFile(const sf::Event& event, emulatorSettings& settings);

void handleNetwork(const sf::Event& event, emulatorSettings& settings);

void handleLobby(const sf::Event& event, emulatorSettings& settings);

void handleControl(const sf::Event& event, emulatorSettings& settings);

void handleColor(const sf::Event& event, emulatorSettings& settings);

void resizeMenus(const sf::Event& event);

static void (*eventState) (const sf::Event&, emulatorSettings& settings) = handleGame;

void initScreen(const int winX, const int winY, const uint32_t* pix, const char* dirName)
{
	sf::Image icon;
	if (!icon.loadFromFile("../assets/linkboyIcon.png"))
		std::cerr << "Failed to load icon";

	pixels = (uint8_t*)pix;

	window.create(sf::VideoMode(winX, winY), "Linkboy-SFML");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	texture.create(160, 144);
	sprite.setTexture(texture);
	sprite.setScale(2,2);
	
	if (!font.loadFromFile("../assets/arial.ttf")) {
		std::cerr << "Failed to load Font \n";
		exit(-1);
	}

	text.setSize({160.0f*2.0f, 60.0f});
	text.setFont(font);
	text.setText(SPEED_OUTPUT[3]);
	text.setTextColor(sf::Color::Red);
	text.setColorForeground(sf::Color::Transparent);
	text.setColorBackground(sf::Color::Red);
	text.setScale({ .5f,.5f });

	mainMenu.setFont(font);
	fileMenu.setFont(font);
	networkMenu.setFont(font);
	lobbyMenu.setFont(font);
	controlMenu.setFont(font);

	background.setSize({ (float)winX, (float)winY });
	background.setPos({ 0,0 });
	background.setColorForeground(sf::Color(31,31,20,120));

	helpMenu.setSize({(float)winX, (float)winY});
	helpMenu.setPos({ 0,0 });
	helpMenu.setFont(font);
	helpMenu.setAutoResize(false);
	helpMenu.setFontSize(40);
	helpMenu.setColorForeground(sf::Color(0,0,160,120));
	helpMenu.setText(helpText);
	helpMenu.setTextColor(sf::Color::White);

	fileMenu.changeDirectory(dirName);
}

void renderScreen()
{
	window.clear(sf::Color::Black);
	if ( !(pauseFocus && showMenu) )
		texture.update(pixels);
	window.draw(sprite);

	window.draw(background);
	window.draw(helpMenu);

	window.draw(mainMenu);
	window.draw(fileMenu);
	window.draw(networkMenu);
	window.draw(lobbyMenu);
	window.draw(controlMenu);
	window.draw(colorMenu);
	window.draw(text);

	window.display();
}

bool handleEvents( emulatorSettings& settings)
{
	sf::Event event;

	settings.operation = NoOperation;
	while (window.pollEvent(event))
		eventState(event, settings);

	//Timer Delay for Label
	if (displaySpeed == 2)
		text.setText(("FPS: "+std::to_string((int)settings.framesPerSecond)).c_str());

	settings.pause = pauseFocus || eventState != handleGame;

	return !window.isOpen();
}

void handleGame(const sf::Event& event, emulatorSettings& settings)
{
	switch(event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			resizeMenus(event);
			renderScreen();
			break;

		case sf::Event::KeyPressed:
			if (event.key.code == START)
				settings.joyPad = (~(0x80) & settings.joyPad) | 0x100;
			if (event.key.code == SELECT)
				settings.joyPad = (~(0x40) & settings.joyPad) | 0x100;
			if (event.key.code == B)
				settings.joyPad = (~(0x20) & settings.joyPad) | 0x100;
			if (event.key.code == A)
				settings.joyPad = (~(0x10) & settings.joyPad) | 0x100;
			if (event.key.code == DOWN)
				settings.joyPad = (~(0x08) & settings.joyPad) | 0x100;
			if (event.key.code == UP)
				settings.joyPad = (~(0x04) & settings.joyPad) | 0x100;
			if (event.key.code == LEFT)
				settings.joyPad = (~(0x02) & settings.joyPad) | 0x100;
			if (event.key.code == RIGHT)
				settings.joyPad = (~(0x01) & settings.joyPad) | 0x100;
			break;

		case sf::Event::KeyReleased:
			if (event.key.code == START)
				settings.joyPad |= 0x80;
			if (event.key.code == SELECT)
				settings.joyPad |= 0x40;
			if (event.key.code == B)
				settings.joyPad |= 0x20;
			if (event.key.code == A)
				settings.joyPad |= 0x10;
			if (event.key.code == DOWN)
				settings.joyPad |= 0x08;
			if (event.key.code == UP)
				settings.joyPad |= 0x04;
			if (event.key.code == LEFT)
				settings.joyPad |= 0x02;
			if (event.key.code == RIGHT)
				settings.joyPad |= 0x01;

			if (event.key.code == MENU) {
				mainMenu.displayMainMenu(true);
				background.setVisible(true);
				eventState = handleMenu;
				renderScreen();
			}

			// if (event.key.code == Keyboard::F1) {
			// 	settings.color = 1;
			// 	settings.operation = ChangeColor;
			// }
			// if (event.key.code == Keyboard::F2) {
			// 	settings.color = 2;
			// 	settings.operation = ChangeColor;
			// }
			// if (event.key.code == Keyboard::F3) {
			// 	settings.color = 3;
			// 	settings.operation = ChangeColor;
			// }
			// if (event.key.code == Keyboard::F4) {
			// 	settings.color = 4;
			// 	settings.operation = ChangeColor;
			// }
			// if (event.key.code == Keyboard::F5) {
			// 	settings.color = 5;
			// 	settings.operation = ChangeColor;
			// }
			if (event.key.code == Keyboard::Num9) {
				showHelp ^= 1;
				helpMenu.setVisible(showHelp);
			}

			if (event.key.code == Keyboard::F8) {
				displaySpeed += 1;
				displaySpeed %= 3;
				if (displaySpeed == 1)
					text.setText(SPEED_OUTPUT[settings.speed]);
				text.setVisible(displaySpeed > 0);
			}

			if ((event.key.code == Keyboard::Add) || (event.key.code == Keyboard::Equal)) {
				if (settings.speed < 10) {
					settings.speed += 1;
					text.setText(SPEED_OUTPUT[settings.speed]);
					settings.operation = ChangeSpeed;
				}
			}

			if ((event.key.code == Keyboard::Subtract) || (event.key.code == Keyboard::Dash)) {
				if (settings.speed > 0) {
					settings.speed -= 1;
					text.setText(SPEED_OUTPUT[settings.speed]);
					settings.operation = ChangeSpeed;					
				}
			}

			if (event.key.code == Keyboard::Home)
				settings.operation = LoadGameState;

			if (event.key.code == Keyboard::End)
				settings.operation = SaveGameState;
			break;

		// case sf::Event::LostFocus: 
		// 	pauseFocus = true;
		// 	break;
		// case sf::Event::GainedFocus:
		// 	pauseFocus = false;
		// 	break;
		default:
			break;
	}

}

void handleMenu(const sf::Event& event, emulatorSettings& settings)
{
	switch(event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			resizeMenus(event);			
			renderScreen();
			break;

		case sf::Event::KeyReleased:
			if (event.key.code == DOWN) {
				mainMenu.displayMainMenu(false);
				colorMenu.displayColorMenu(true);
				background.setVisible(false);
				eventState = handleColor;
				renderScreen();
			}
			if (event.key.code == UP) {
				mainMenu.displayMainMenu(false);
				controlMenu.displayControlMenu(true);
				eventState = handleControl;
				renderScreen();
			}
			if (event.key.code == LEFT) {
				mainMenu.displayMainMenu(false);
				fileMenu.displayFileMenu(true);
				eventState = handleFile;
				renderScreen();
			}
				
			if (event.key.code == RIGHT) {
				mainMenu.displayMainMenu(false);
				networkMenu.displayNetworkMenu(true);
				eventState = handleNetwork;
				renderScreen();
			}
			if (event.key.code == MENU) {
				mainMenu.displayMainMenu(false);
				background.setVisible(false);
				eventState = handleGame;
				renderScreen();
			}
			
			
			break;

		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				int temp = mainMenu.checkButtonPress(sf::Mouse::getPosition(window));

				switch (temp) {
					case 0b0001:
						mainMenu.displayMainMenu(false);
						fileMenu.displayFileMenu(true);
						eventState = handleFile;
						renderScreen();
						break;
					case 0b0010:
						mainMenu.displayMainMenu(false);
						controlMenu.displayControlMenu(true);
						eventState = handleControl;
						renderScreen();
						break;
					case 0b0100:
						mainMenu.displayMainMenu(false);
						//if (settings.networkConnected)
						networkMenu.displayNetworkMenu(true);
						//else
							//lobbyMenu.displayLobbyMenu(true);
						eventState = handleNetwork;
						renderScreen();
						break;
					case 0b1000:
						mainMenu.displayMainMenu(false);
						colorMenu.displayColorMenu(true);
						background.setVisible(false);
						eventState = handleColor;
						renderScreen();
						break;

					default:
						break;
				}
			}
			break;

		default:
			break;
	}
}

void handleFile(const sf::Event& event, emulatorSettings& settings)
{
	switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			resizeMenus(event);
			renderScreen();
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == DOWN) {
				fileMenu.moveSelection(0);
				renderScreen();
			}

			if (event.key.code == UP) {
				fileMenu.moveSelection(1);
				renderScreen();
			}

			if (event.key.code == START || event.key.code == A) {
				if ((settings.loadGameFile = fileMenu.selectFile(-1)) != nullptr) {
					fileMenu.displayFileMenu(false);
					background.setVisible(false);
					settings.operation = LoadGame;
					eventState = handleGame;
				}
				renderScreen();
			}
			
			if (event.key.code == MENU || event.key.code == B) {
				fileMenu.displayFileMenu(false);
				mainMenu.displayMainMenu(true);
				eventState = handleMenu;
				renderScreen();
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				int temp = fileMenu.checkButtonPress(sf::Mouse::getPosition(window));

				switch (temp) {
					case 0b00000001:
						fileMenu.movePage(1);
						renderScreen();
						break;
					case 0b00000010:
						if ((settings.loadGameFile = fileMenu.selectFile(0)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00000100: 
						if ((settings.loadGameFile = fileMenu.selectFile(1)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00001000:
						if ((settings.loadGameFile = fileMenu.selectFile(2)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00010000: 
						if ((settings.loadGameFile = fileMenu.selectFile(3)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00100000: 
						if ((settings.loadGameFile = fileMenu.selectFile(4)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b01000000: 
						if ((settings.loadGameFile = fileMenu.selectFile(5)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							settings.operation = LoadGame;							
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b10000000: 
						fileMenu.movePage(0);
						renderScreen();
						break;
					default:
						break;
				}
			}
			break;

		case sf::Event::MouseMoved:
			fileMenu.hoverMouse(sf::Mouse::getPosition(window));
			renderScreen();
			break;

		case sf::Event::MouseWheelScrolled:
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (event.mouseWheelScroll.delta > 0)
					fileMenu.movePage(1);
				if (event.mouseWheelScroll.delta < 0)
					fileMenu.movePage(0);
			renderScreen();

		default:
			break;
	}
}

void handleNetwork(const sf::Event& event, emulatorSettings& settings)
{
	switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			resizeMenus(event);			
			renderScreen();
			break;

		case sf::Event::TextEntered:
			networkMenu.checkKeyPress(event.key.code);
			renderScreen();
			break;

		case sf::Event::KeyReleased:
			if (event.key.code == MENU) {
				mainMenu.displayMainMenu(true);
				networkMenu.displayNetworkMenu(false);
				eventState = handleMenu;
				renderScreen();
			}

			break;

		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {

				if (networkMenu.checkButtonPress(sf::Mouse::getPosition(window))) {	
					/* try to connect */
					settings.network.ip = networkMenu.getIPAddress();
					settings.network.port = networkMenu.getPort();
					settings.network.name = networkMenu.getName();
					
					mainMenu.displayMainMenu(true);
					networkMenu.displayNetworkMenu(false);
					settings.operation = ConnectToServer;					
					eventState = handleMenu;
				}
				renderScreen();
			}
			break;

		default:
			break;
	}
}

void handleLobby(const sf::Event& event, emulatorSettings& settings)
{
	switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			resizeMenus(event);
			renderScreen();
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == DOWN) {
				lobbyMenu.moveSelection(0);
				renderScreen();
			}

			if (event.key.code == UP) {
				lobbyMenu.moveSelection(1);
				renderScreen();
			}

			if (event.key.code == START || event.key.code == A) {
				//if ((settings.loadGameFile = fileMenu.selectFile(-1)) != nullptr) {
					lobbyMenu.displayLobbyMenu(false);
					background.setVisible(false);
					//settings.operation = LoadGame;
					eventState = handleGame;
				//}
				renderScreen();
			}
			
			if (event.key.code == MENU || event.key.code == B) {
				lobbyMenu.displayLobbyMenu(false);
				mainMenu.displayMainMenu(true);
				eventState = handleMenu;
				renderScreen();
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				int temp = lobbyMenu.checkButtonPress(sf::Mouse::getPosition(window));

				switch (temp) {
					case 0b00000001:
						lobbyMenu.movePage(1);
						renderScreen();
						break;
					case 0b00000010:
						// if ((settings.loadGameFile = fileMenu.selectFile(0)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b00000100: 
						// if ((settings.loadGameFile = fileMenu.selectFile(1)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b00001000:
						// if ((settings.loadGameFile = fileMenu.selectFile(2)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b00010000: 
						// if ((settings.loadGameFile = fileMenu.selectFile(3)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b00100000: 
						// if ((settings.loadGameFile = fileMenu.selectFile(4)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b01000000: 
						// if ((settings.loadGameFile = fileMenu.selectFile(5)) != nullptr) {
							lobbyMenu.displayLobbyMenu(false);
							background.setVisible(false);
							// settings.operation = LoadGame;							
							eventState = handleGame;
						// }
						renderScreen();
						break;
					case 0b10000000: 
						lobbyMenu.movePage(0);
						renderScreen();
						break;
					default:
						break;
				}
			}
			break;

		case sf::Event::MouseMoved:
			lobbyMenu.hoverMouse(sf::Mouse::getPosition(window));
			renderScreen();
			break;

		case sf::Event::MouseWheelScrolled:
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (event.mouseWheelScroll.delta > 0)
					lobbyMenu.movePage(1);
				if (event.mouseWheelScroll.delta < 0)
					lobbyMenu.movePage(0);
			renderScreen();

		default:
			break;
	}
}

void handleControl(const sf::Event& event, emulatorSettings& settings)
{
	switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			resizeMenus(event);
			renderScreen();
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == MENU) {
				mainMenu.displayMainMenu(true);
				controlMenu.displayControlMenu(false);
				changeKey = false;
				eventState = handleMenu;
			}
			else if (changeKey) {
				switch (controlMenu.setKey(event.key.code)) {
					case (1<<0):
						break;
					case (1<<1):
						break;
					case (1<<2):
						break;
					case (1<<3):
						break;
					case (1<<4):
						break;
					case (1<<5):
						break;
					case (1<<6):
						UP = event.key.code;
						break;
					case (1<<7):
						LEFT = event.key.code;
						break;
					case (1<<8):
						DOWN = event.key.code;
						break;
					case (1<<9):
						RIGHT = event.key.code;
						break;
					case (1<<10):
						A = event.key.code;
						break;
					case (1<<11):
						B = event.key.code;
						break;
					case (1<<12):
						START = event.key.code;
						break;
					case (1<<13):
						SELECT = event.key.code;
						break;
					default:
						break;


				}
				changeKey = false;
				renderScreen();
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				if (controlMenu.checkButtonPress(sf::Mouse::getPosition(window))) {	
					changeKey = true;
					renderScreen();					
				}
			}
			break;
		default:
			break;
	}
}

void handleColor(const sf::Event& event, emulatorSettings& settings)
{
	switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			resizeMenus(event);
			renderScreen();
			break;
		case sf::Event::KeyReleased:
				if (event.key.code == MENU) {
					mainMenu.displayMainMenu(true);
					colorMenu.displayColorMenu(false);
					background.setVisible(true);
					changeKey = false;
					eventState = handleMenu;
				}

				renderScreen();
			break;
		case sf::Event::MouseButtonReleased:
			if (colorMenu.handleButtonPress(sf::Mouse::getPosition(window))) {
				sf::Color temp;
				for (int i=0; i<4; ++i) {
					temp = colorMenu.getColor(i);
					settings.color[i].Red = temp.r;
					settings.color[i].Green = temp.g;
					settings.color[i].Blue = temp.b;				
					settings.color[i].Alpha = 0xFF;
				}
				settings.operation = ChangeColor;
				renderScreen();
			}
			break;
		case sf::Event::MouseMoved:
			colorMenu.hoverMouse(sf::Mouse::getPosition(window));
			renderScreen();
			break;
		default:
			break;
	}
}

void resizeMenus(const sf::Event& event)
{
	sf::Vector2f scale;

	scale = { event.size.width / (160.0f*2.0f), event.size.height / (144.0f*2.0f) };
	mainMenu.scaleMenu(scale);
	networkMenu.scaleMenu(scale);
	fileMenu.scaleMenu(scale);
	controlMenu.scaleMenu(scale);
	lobbyMenu.scaleMenu(scale);
	colorMenu.scaleMenu(scale);
}