#include "ppu.h"

PPU::PPU(MMU* memory)
	:m_memory(memory)
{
	m_color[0] = WHITE;
	m_color[1] = LIGHTGREY;
	m_color[2] = DARKGREY;
	m_color[3] = BLACK;
}

PPU::~PPU()
{
	m_memory = nullptr;
	m_currentState = nullptr;
}

void PPU::changeColor(const int scheme)
{
	m_color[0] = WHITE;
	switch (scheme) {
		case 2:
			m_color[2] = LIGHTRED;
			m_color[1] = DARKRED;
			m_color[3] = BLACK;
			break;
		case 3:
			m_color[2] = LIGHTGREEN;
			m_color[1] = DARKGREEN;
			m_color[3] = BLACK;
			break;
		case 4:
			m_color[2] = LIGHTBLUE;
			m_color[1] = DARKBLUE;
			m_color[3] = BLACK;
			break;
		case 5:
			m_color[0] = WHITEGB;
			m_color[1] = LIGHTGB;
			m_color[2] = DARKGB;
			m_color[3] = BLACKGB;
			break;
		default:
			m_color[2] = LIGHTGREY;
			m_color[1] = DARKGREY;
			m_color[3] = BLACK;
	}
}

void PPU::advanceState(const int cycle)
{
	uint8_t LCDC = m_memory->mmIO[LCDC_CONTROL];
	m_displayOn = (LCDC & 0x80) != 0;
	m_VBlank = false;

	//Display not Enabled
	if (!(m_displayOn)) {
		m_memory->mmIO[LY] = 0;
		m_memory->mmIO[LCDC_STAT] = ((m_memory->mmIO[LCDC_STAT] & 0b11111100) | 0b01);

		//Prevent LockOut
		m_offDelay += cycle;
		if (m_offDelay >= 70224) {
			m_offDelay = 0;
			m_VBlank = true;
		}
		
	}
	else {
		m_cycle += cycle;
		//Implement Graphical Hardware Bugs


		//Perform State 
		if (m_cycle >= m_stateTime) {
			m_cycle -= m_stateTime;
			m_stateTime = (this->*(m_currentState))();

			//Check V-blank Interrupt
			if (m_memory->mmIO[LY] == 144)
				m_memory->mmIO[IF] |= 1;
		}
	}
}

void PPU::clearDisplay()
{
	for (int y = 0; y < 144; ++y)
		for (int x = 0; x < 160; ++x)
			m_buffer[y][x] = m_color[0];
}

const uint32_t* PPU::getBuffer()
{
	return (const uint32_t*)m_buffer;
}

bool PPU::isVBlank()
{
	return m_VBlank;
}

bool PPU::isDisplayOn()
{
	return m_displayOn;
}

int PPU::HBlank()
{
	int re = 80;

	//State Transition
	if (m_memory->mmIO[LY] >= 143) {
		m_memory->mmIO[LCDC_STAT] |= 1;
		m_VBlank = true;
		m_currentState = &PPU::VBlank;
		re = 456;

		//Interrupt Mode 1 V-blank
		if (m_memory->mmIO[LCDC_STAT] & 0x10)
			m_memory->mmIO[IF] |= 2;

	}
	else {
		m_memory->mmIO[LCDC_STAT] |= 0b10;

		m_currentState = &PPU::readOAM;
	}

	//Interrupt LY coincidence
	if (m_memory->mmIO[LY] == m_memory->mmIO[LYC]) {
		if (m_memory->mmIO[LCDC_STAT] & 0x40)
			m_memory->mmIO[IF] |= 2;
		m_memory->mmIO[LCDC_STAT] |= 0x4;
	}
	else
		m_memory->mmIO[LCDC_STAT] &= ~(0x4);

	//Operation
	++m_memory->mmIO[LY];

	return re;
}

int PPU::readOAM()
{
	m_memory->mmIO[LCDC_STAT]  |= 1;

	m_currentState				= &PPU::readVRAM;

	return 172;
}

int PPU::readVRAM()
{
	uint8_t LCDC			= m_memory->mmIO[LCDC_CONTROL];
	uint8_t line			= m_memory->mmIO[LY];

	int32_t temp = m_memory->mmIO[BGP];

	uint8_t BGcolor[4] = {
		(uint8_t)(temp & 0x03), 
		(uint8_t)((temp & 0x0C) >> 2),
		(uint8_t)((temp & 0x30) >> 4),
		(uint8_t)((temp & 0xC0) >> 6)
	};
	
	uint8_t pixelData1, pixelData2;
	uint8_t colorOffset;

	if (LCDC & 1) {
		uint8_t scrollX			= m_memory->mmIO[SCX];
		int16_t winY			= m_memory->mmIO[WY];
		int16_t winX			= m_memory->mmIO[WX] - 7;

		uint16_t WinTileMap		= ((LCDC & 0x40) ? 0x1C00 : 0x1800);
		uint16_t BGWinTileData	= ((LCDC & 0x10) ? 0x0000 : 0x0800) << 1;
		uint16_t BGTileMap		=  (LCDC & 0x08) ? 0x1C00 : 0x1800;

		uint16_t posY			= ((m_memory->mmIO[SCY] + line) & 0xFF); 
		

		int	pixel				=	0;

		uint8_t posX;

		int16_t startWindow		=	160 - (scrollX & 0x7);

		int	endPoint1			=	8;
		int	endPoint2			=	(scrollX & 0x7);

		//If Window Enabled
		if (LCDC & 0x20 && line >= winY && winX < 160) {

			startWindow = winX & 0xFFF8;

			if (winX == 0) {
				endPoint1 = 0;
				endPoint2 = (winX & 0x7);
			}
			else if (winX < 0) {
				endPoint1 = 0;
				endPoint2 = 0;
			}
			else if (winX + (scrollX & 0x7) < 8) {
				endPoint1 = winX + (scrollX & 0x7);
				endPoint2 = 0;
			}
		}

		BGWinTileData += ((posY & 0x7) << 1);
	
		//--------------Start Background--------------
		//--------------BackGround First Tile--------------
		posX		= scrollX;
		if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
			temp = m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
		else
			temp = (int8_t)m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
		temp <<= 4;

		pixelData1	= m_memory->VRAM[ BGWinTileData + temp ];
		pixelData2	= m_memory->VRAM[ BGWinTileData + temp + 1 ];

		for (int i = posX & 0x7; i < endPoint1; ++i) {
			colorOffset	= BGcolor[ (pixelData1 >> (7-i) & 0x1) | (((pixelData2 >> (7-i)) & 0x1) << 1) ];		//Grab Tile Color Pallet
			m_buffer[line][pixel++] = m_color[colorOffset];												//Update Pixel
		}

		
		//--------------BackGround Midsection--------------
		while (pixel < startWindow) {
			posX = pixel + scrollX;

			if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
				temp = m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
			else
				temp = (int8_t)m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
			temp <<= 4;

			pixelData1 = m_memory->VRAM[ BGWinTileData + temp ];
			pixelData2 = m_memory->VRAM[ BGWinTileData + temp + 1 ];

			for (int i = 0; i < 8; ++i) {
				colorOffset = BGcolor[(pixelData1 >> (7-i) & 0x1) | (((pixelData2 >> (7-i)) & 0x1) << 1)];		//Grab Tile Color Pallet
				m_buffer[line][pixel++] = m_color[colorOffset];											//Update Pixel
			}
		}
			

		//--------------BackGround End Tile--------------
		posX = pixel + scrollX;

		if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
			temp = m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
		else
			temp = (int8_t)m_memory->VRAM[ BGTileMap + ((posY >>3)<<5) + (posX>>3) ];
		temp <<= 4;

		pixelData1 = m_memory->VRAM[ BGWinTileData + temp ];
		pixelData2 = m_memory->VRAM[ BGWinTileData + temp + 1 ];

		for (int i = 0; i < endPoint2; ++i) {
			colorOffset = BGcolor[ (pixelData1 >> (7-i) & 0x1) | (((pixelData2 >> (7-i)) & 0x1) << 1) ];		//Grab Tile Color Pallet
			m_buffer[line][pixel++] = m_color[colorOffset];												//Update Pixel
		}
		

		//--------------Start Window--------------
		if (startWindow != 160 - (scrollX & 0x7) ) {
			posX = 0; 
			posY = line - winY;
			BGWinTileData = ((LCDC & 0x10) ? 0x0000 : 0x0800) << 1;
			BGWinTileData += ((posY & 0x7) << 1);
			//--------------Window Starts Off Screen--------------
			if (winX < 0) {
				posX = -winX;

				if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
					temp = m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				else
					temp = (int8_t)m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				temp <<= 4;

				pixelData1 = m_memory->VRAM[ BGWinTileData + temp];
				pixelData2 = m_memory->VRAM[ BGWinTileData + temp + 1 ];

				for (int i = posX; i < 8; ++i) {
					colorOffset = BGcolor[(pixelData1 >> (7-i) & 0x1) | (((pixelData2 >> (7-i)) & 0x1) << 1)];			//Grab Tile Color Pallet
					m_buffer[line][pixel++] = m_color[colorOffset];												//Update Pixel
					++posX;
				}
			}

			//--------------Window Midsection--------------
			if (startWindow < 0)
				startWindow = winX;
			while (pixel < 152 - startWindow) {
				if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
					temp = m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				else
					temp = (int8_t)m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				temp <<= 4;

				pixelData1 = m_memory->VRAM[ BGWinTileData + temp ];
				pixelData2 = m_memory->VRAM[ BGWinTileData + temp + 1 ];

				for (int i = 0; i < 8; ++i) {
					colorOffset = BGcolor[ (pixelData1>>(7-i) & 0x1) | (((pixelData2>>(7-i)) & 0x1) << 1) ];		//Grab Tile Color Pallet
					m_buffer[line][pixel++] = m_color[colorOffset];											//Update Pixel
					++posX;
				}
			}

			//--------------Window End Tile--------------
			if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
				temp = m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
			else
				temp = (int8_t)m_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
			temp <<= 4;

			pixelData1 = m_memory->VRAM[ BGWinTileData + temp ];
			pixelData2 = m_memory->VRAM[ BGWinTileData + temp + 1 ];

			int i = 0;
			while (pixel < 160) {
				colorOffset = BGcolor[ (pixelData1>>(7-i) & 0x1) | (((pixelData2>>(7-i)) & 0x1) << 1) ];			//Grab Tile Color Pallet
				m_buffer[line][pixel++] = m_color[colorOffset];												//Update Pixel
				++i;
				++posX;
			}
		}
	}


	//---Draw Sprites---
	if (LCDC & 0b10) {
		uint16_t spriteHeight = (LCDC & 0x4) ? 16 : 8;

		uint8_t spriteNum = 0;
		spriteData sprites[11] = {};
		spriteData tempSprite = {};

		temp = m_memory->mmIO[OBP0];

		uint8_t OBP0color[4];
		OBP0color[3] = (temp & 0xC0) >> 6;
		OBP0color[2] = (temp & 0x30) >> 4;
		OBP0color[1] = (temp & 0x0C) >> 2;
		OBP0color[0] = 0xFF;

		temp = m_memory->mmIO[OBP1];

		uint8_t OBP1color[4];
		OBP1color[3] = (temp & 0xC0) >> 6;
		OBP1color[2] = (temp & 0x30) >> 4;
		OBP1color[1] = (temp & 0x0C) >> 2;
		OBP1color[0] = 0xFF;
	
		uint8_t mask = (LCDC & 0x4) ? 0xFE : 0xFF;


		//Obtain Priority of Sprites Being Displayed
		for (int i = 0; i < 40 && spriteNum < 10; i++) {
			tempSprite.y		= (m_memory->spriteRAM[i << 2] - 16);
			tempSprite.x		= (m_memory->spriteRAM[(i << 2) + 1]);
			tempSprite.tileNum	= (m_memory->spriteRAM[(i << 2) + 2]) & mask;
			tempSprite.flags	= (m_memory->spriteRAM[(i << 2) + 3]);

			if ( (tempSprite.y <= line) && ( (tempSprite.y + spriteHeight) > line) ) {
				if (spriteNum == 0)
					sprites[spriteNum++] = tempSprite;
				else {
					int x = 0;
					while ((tempSprite.x >= sprites[x].x) && x < spriteNum)
						++x;

					for (int j = spriteNum; j >= x; --j)
						sprites[j + 1] = sprites[j];
					
					sprites[x] = tempSprite;
					++spriteNum;
				}
			}
		}

		
		//Display Sprites
		for (int i = spriteNum-1; i >= 0; --i) {
			uint8_t *spriteColor = OBP0color;
			int endPoint = 7;
			int startPoint = 0;

			if (sprites[i].x < 168 && sprites[i].x != 0) {
				int spriteLine = (line - sprites[i].y);

				if (sprites[i].flags & Y_FLIP)
					spriteLine = (spriteHeight-1) - spriteLine; 

				if (sprites[i].flags & PALETTE_NUM)
					spriteColor = OBP1color;
				
				/*if (spriteLine > 7)
					sprites[i].tileNum += 1;*/

				spriteLine = (sprites[i].tileNum << 4) + (spriteLine << 1);
				pixelData1 = m_memory->VRAM[spriteLine];
				pixelData2 = m_memory->VRAM[spriteLine + 1];

				bool spritePriority = (sprites[i].flags & PRIORITY) == 0;


				sprites[i].x -= 8;

				if (sprites[i].x > 152) {
					if (sprites[i].flags & X_FLIP) 
						startPoint = 7-(160 - sprites[i].x);
					else 
						endPoint = 160 - sprites[i].x;
				}
				else if (sprites[i].x < 0) {
					if (sprites[i].flags & X_FLIP) {
						endPoint += sprites[i].x;
						//sprites[i].x = -sprites[i].x;
					}
					else {
						startPoint = -sprites[i].x;
						sprites[i].x = 0;
					}
				}
				

				if (sprites[i].flags & X_FLIP) {
					for (int j = endPoint; j >= startPoint; --j) {
						if (spritePriority || (m_buffer[line][(endPoint-j)+sprites[i].x].Val == m_color[BGcolor[0]].Val)) {
							colorOffset = spriteColor[(((pixelData1 >> (7 - j)) & 0x1) | (((pixelData2 >> (7 - j)) & 0x1) << 1))];	//Grab Tile Color Pallet
							if ( colorOffset != 0xFF)
								m_buffer[line][(7-j)+sprites[i].x] = m_color[colorOffset];							//Update Pixel
						}
					}
				}   
				else {
					for (int j = startPoint; j <= endPoint; ++j) {
						if ( spritePriority || (m_buffer[line][ (j-startPoint)+sprites[i].x ].Val == m_color[BGcolor[0]].Val) ) {
							colorOffset = spriteColor[((pixelData1 >> (7 - j) & 0x1) | (((pixelData2 >> (7 - j)) & 0x1) << 1))];	//Grab Tile Color Pallet
							if ( colorOffset != 0xFF) 
								m_buffer[line][ (j-startPoint)+sprites[i].x ] = m_color[colorOffset];									//Update Pixel
						}
					}
				}
			}
		}

	}

	//State Transition
	m_memory->mmIO[LCDC_STAT]	&= 0b11111100;
	m_currentState				 = &PPU::HBlank;

	//Interrupt Mode 0 H-Blank
	if (m_memory->mmIO[LCDC_STAT] & 0x08)
		m_memory->mmIO[IF] |= 2;

	return 204;
}

int PPU::VBlank()
{
	int re = 456;

	//State Transition
	if (m_memory->mmIO[LY] >= 153) {
		m_memory->mmIO[LY]			= 0;
		m_memory->mmIO[LCDC_STAT]	= ( (m_memory->mmIO[LCDC_STAT] & 0b11111100) | 0b10 );

		m_currentState				= &PPU::readOAM;
		re							= 80;

		//Interrupt Mode 2 OAM
		if (m_memory->mmIO[LCDC_STAT] & 0x20)
			m_memory->mmIO[IF] |= 2;
			
	}
	else {
		//Interrupt LY coincidence
		if (m_memory->mmIO[LY] == m_memory->mmIO[LYC]) {
			if (m_memory->mmIO[LCDC_STAT] & 0x40)
				m_memory->mmIO[IF] |= 2;
			m_memory->mmIO[LCDC_STAT] |= 0x4;
		}
		else
			m_memory->mmIO[LCDC_STAT] &= ~(0x4);

		//Operation
		++m_memory->mmIO[LY];
	}

	return re;
}
