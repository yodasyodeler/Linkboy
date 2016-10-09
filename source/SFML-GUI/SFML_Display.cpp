#include "Display.h"
#include "SFML-GUI\MainMenu.h"
#include "SFML-GUI\FileMenu.h"


using sf::Keyboard;

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
static int displaySpeed = false;

static sf::Font font;
static Label text;

static Rectangle background;

//Main Menu
static MainMenu mainMenu;

//fileMenu
static FileMenu fileMenu;

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
void handleLink(const sf::Event& event, emulatorSettings& settings);

static void (*eventState) (const sf::Event&, emulatorSettings& settings) = handleGame;

void initScreen(const int winX, const int winY, const uint32_t* pix, const char* dirName)
{
	sf::Image icon;
	icon.loadFromFile("../assets/linkboyIcon.png");

	pixels = (uint8_t*)pix;

	window.create(sf::VideoMode(winX, winY), "Linkboy-SFML");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	texture.create(160, 144);
	sprite.setTexture(texture);
	sprite.setScale(2,2);
	
	if (!font.loadFromFile("..\\assets\\arial.ttf")) {
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

	background.setSize({ (float)winX, (float)winY });
	background.setPos({ 0,0 });
	background.setColorForeground(sf::Color(31,31,20,90));

	fileMenu.changeDirectory(dirName);
	//fileMenu.moveUpDirectory();
}

void renderScreen()
{
	window.clear(sf::Color::Black);
	if ( !(pauseFocus && showMenu) )
		texture.update(pixels);
	window.draw(sprite);

	window.draw(background);

	window.draw(mainMenu);
	window.draw(fileMenu);

	window.draw(text);

	window.display();
}

bool handleEvents( emulatorSettings& settings)
{
	sf::Event event;

	while (window.pollEvent(event)) {
		eventState(event, settings);
	}

	//Timer Delay for Label
	if (displaySpeed == 2)
		text.setText(("FPS: "+std::to_string((int)settings.framesPerSecond)).c_str());

	settings.pause = pauseFocus || eventState != handleGame;

	return !window.isOpen();
}
void handleGame(const sf::Event& event, emulatorSettings& settings)
{
	sf::Vector2f scale;
	switch(event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			scale = { event.size.width / (160.0f*2.0f), event.size.height / (144.0f*2.0f) };
			mainMenu.scaleMenu(scale);
			fileMenu.scaleMenu(scale);
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

			if (event.key.code == Keyboard::F1)
				settings.color = 1;
			if (event.key.code == Keyboard::F2)
				settings.color = 2;
			if (event.key.code == Keyboard::F3)
				settings.color = 3;
			if (event.key.code == Keyboard::F4)
				settings.color = 4;
			if (event.key.code == Keyboard::F5)
				settings.color = 5;

			if (event.key.code == Keyboard::F8) {
				displaySpeed += 1;
				displaySpeed %= 3;
				if (displaySpeed == 1)
					text.setText(SPEED_OUTPUT[settings.speed]);
				text.setVisible(displaySpeed > 0);
			}

		/*	if (event.key.code == Keyboard::Multiply)
				settings.breakpoint = -2;*/


			if ((event.key.code == Keyboard::Add) || (event.key.code == Keyboard::Equal)) {
				if (settings.speed < 10) {
					settings.speed += 1;
					text.setText(SPEED_OUTPUT[settings.speed]);
				}
			}

			if ((event.key.code == Keyboard::Subtract) || (event.key.code == Keyboard::Dash)) {
				if (settings.speed > 0) {
					settings.speed -= 1;
					text.setText(SPEED_OUTPUT[settings.speed]);
				}
			}

			if (event.key.code == Keyboard::Home)
				settings.saveState = 1;

			if (event.key.code == Keyboard::End)
				settings.saveState = 2;
			break;

		case sf::Event::LostFocus: 
			//pauseFocus = true;
			break;
		case sf::Event::GainedFocus:
			//pauseFocus = false;
			break;
		default:
			break;
	}

}
void handleMenu(const sf::Event& event, emulatorSettings& settings)
{
	sf::Vector2f scale;

	switch(event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			scale = { event.size.width / (160.0f*2.0f), event.size.height / (144.0f*2.0f) };
			mainMenu.scaleMenu(scale);
			fileMenu.scaleMenu(scale);
			renderScreen();
			break;

		case sf::Event::KeyReleased:
			if (event.key.code == DOWN) {
				mainMenu.displayMainMenu(false);
				background.setVisible(false);
				eventState = handleGame;
				renderScreen();
			}
			
			if (event.key.code == UP) {
				renderScreen();
			}
			if (event.key.code == LEFT) {
				mainMenu.displayMainMenu(false);
				fileMenu.displayFileMenu(true);
				eventState = handleFile;

				renderScreen();
			}
				
			if (event.key.code == RIGHT) {
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
						//displayFileMenu(showMenu);
						renderScreen();
						break;
					case 0b0100:
						renderScreen();
						break;
					case 0b1000:
						mainMenu.displayMainMenu(false);
						background.setVisible(false);
						eventState = handleGame;
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
	sf::Vector2f scale;

	switch (event.type) {

		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::Resized:
			scale = { event.size.width / (160.0f*2.0f), event.size.height / (144.0f*2.0f) };
			mainMenu.scaleMenu(scale);
			fileMenu.scaleMenu(scale);
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
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00000100: 
						if ((settings.loadGameFile = fileMenu.selectFile(1)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00001000:
						if ((settings.loadGameFile = fileMenu.selectFile(2)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00010000: 
						if ((settings.loadGameFile = fileMenu.selectFile(3)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b00100000: 
						if ((settings.loadGameFile = fileMenu.selectFile(4)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
							eventState = handleGame;
						}
						renderScreen();
						break;
					case 0b01000000: 
						if ((settings.loadGameFile = fileMenu.selectFile(5)) != nullptr) {
							fileMenu.displayFileMenu(false);
							background.setVisible(false);
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
void handleLink(const sf::Event& event, emulatorSettings& settings)
{}