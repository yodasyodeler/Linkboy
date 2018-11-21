#include "ppu.h"

PPU::PPU(MMU* memory)
	:_memory(memory)
{
	_color[0] = WHITE;
	_color[1] = DARKGREY;
	_color[2] = LIGHTGREY;
	_color[3] = BLACK;
}

PPU::~PPU()
{
	_memory = nullptr;
	_currentState = nullptr;
}

void PPU::changeColor(const Color color[4])
{
	for (int i=0; i<4; ++i)
		_color[i] = color[i];
}

void PPU::advanceState(const int cycle)
{
	uint8_t LCDC = _memory->mmIO[LCDC_CONTROL];
	_displayOn = (LCDC & 0x80) != 0;
	_VBlank = false;

	//Display not Enabled
	if (!(_displayOn)) {
		_memory->mmIO[LY] = 0;
		_memory->mmIO[LCDC_STAT] = ((_memory->mmIO[LCDC_STAT] & 0b11111100) | 0b01);

		//Prevent LockOut
		_offDelay += cycle;
		if (_offDelay >= 70224) {
			_offDelay = 0;
			_VBlank = true;
		}
		
	}
	else {
		_cycle += cycle;
		//Implement Graphical Hardware Bugs


		//Perform State 
		if (_cycle >= _stateTime) {
			_cycle -= _stateTime;
			_stateTime = (this->*(_currentState))();

			//Check V-blank Interrupt
			if (_memory->mmIO[LY] == 144)
				_memory->mmIO[IF] |= 1;
		}
	}
}

void PPU::clearDisplay()
{
	for (int y = 0; y < 144; ++y)
		for (int x = 0; x < 160; ++x)
			_colorIndex[y][x] = 0;
}

const uint32_t* PPU::getBuffer()
{
	return (const uint32_t*)_buffer;
}

void PPU::updateColor() {
	if (_displayOn) {
		for (int y = 0; y < 144; ++y)
			for (int x = 0; x < 160; ++x)
				_buffer[y][x] = _color[_colorIndex[y][x]];
	}
	else
		clearDisplay();
}

bool PPU::isVBlank()
{
	return _VBlank;
}

bool PPU::isDisplayOn()
{
	return _displayOn;
}

void PPU::toggleBackground()
{
	_enableBackground = !_enableBackground;
}

void PPU::toggleWindow()
{
	_enableWindow = !_enableWindow;
}

void PPU::toggleSprites()
{
	_enableSprites = !_enableSprites;
}

int PPU::HBlank()
{
	int re = OAMCycles;

	//State Transition
	if (_memory->mmIO[LY] >= 143) {
		_memory->mmIO[LCDC_STAT] |= 1;
		_VBlank = true;
		_currentState = &PPU::VBlank;
		re = VBlankLineCycles;

		//Interrupt Mode 1 V-blank
		if (_memory->mmIO[LCDC_STAT] & 0x10)
			_memory->mmIO[IF] |= 2;

	}
	else {
		_memory->mmIO[LCDC_STAT] |= 0b10;
		_currentState = &PPU::readOAM;
	}

	//Interrupt LY coincidence
	if (_memory->mmIO[LY] == _memory->mmIO[LYC]) {
		if (_memory->mmIO[LCDC_STAT] & 0x40)
			_memory->mmIO[IF] |= 2;
		_memory->mmIO[LCDC_STAT] |= 0x4;
	}
	else
		_memory->mmIO[LCDC_STAT] &= ~(0x4);

	//Operation
	++_memory->mmIO[LY];

	return re;
}

int PPU::readOAM()
{
	_memory->mmIO[LCDC_STAT]  |= 1;

	_currentState				= &PPU::readVRAM;

	return VRAMCycles;
}

int PPU::readVRAM()
{
	uint8_t LCDC			= _memory->mmIO[LCDC_CONTROL];
	uint8_t line			= _memory->mmIO[LY];

	int32_t temp = _memory->mmIO[BGP];

	uint8_t BGcolor[4] = {
		(uint8_t)(temp & 0x03), 
		(uint8_t)((temp & 0x0C) >> 2),
		(uint8_t)((temp & 0x30) >> 4),
		(uint8_t)((temp & 0xC0) >> 6)
	};
	
	uint8_t pixelData1, pixelData2;
	uint8_t colorOffset;

	if (LCDC & 1) {
		uint8_t scrollX			= _memory->mmIO[SCX];
		int16_t winY			= _memory->mmIO[WY];
		int16_t winX			= _memory->mmIO[WX] - 7;

		uint16_t WinTileMap		= ((LCDC & 0x40) ? 0x1C00 : 0x1800);
		uint16_t BGWinTileData	= ((LCDC & 0x10) ? 0x0000 : 0x0800) << 1;
		uint16_t BGTileMap		=  (LCDC & 0x08) ? 0x1C00 : 0x1800;

		uint16_t posY			= ((_memory->mmIO[SCY] + line) & 0xFF); 
		

		int	pixel				=	0;

		uint8_t posX;

		int16_t startWindow		=	160 - (scrollX & 0x7);

		int	endPoint1			=	8;
		int	endPoint2			=	(scrollX & 0x7);

		//If Window Enabled
		if (_enableWindow && (LCDC & 0x20) && line >= winY && winX < 160) {

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
		if (_enableBackground) {
			//--------------BackGround First Tile--------------
			posX = scrollX;
			if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
				temp = _memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
			else
				temp = (int8_t)_memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
			temp <<= 4;

			pixelData1 = _memory->VRAM[BGWinTileData + temp];
			pixelData2 = _memory->VRAM[BGWinTileData + temp + 1];

			for (int i = posX & 0x7; i < endPoint1; ++i) {
				colorOffset = BGcolor[(pixelData1 >> (7 - i) & 0x1) | (((pixelData2 >> (7 - i)) & 0x1) << 1)];		//Grab Tile Color Pallet
				_colorIndex[line][pixel++] = colorOffset;															//Update Pixel
			}


			//--------------BackGround Midsection--------------
			while (pixel < startWindow) {
				posX = pixel + scrollX;

				if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
					temp = _memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
				else
					temp = (int8_t)_memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
				temp <<= 4;

				pixelData1 = _memory->VRAM[BGWinTileData + temp];
				pixelData2 = _memory->VRAM[BGWinTileData + temp + 1];

				for (int i = 0; i < 8; ++i) {
					colorOffset = BGcolor[(pixelData1 >> (7 - i) & 0x1) | (((pixelData2 >> (7 - i)) & 0x1) << 1)];		//Grab Tile Color Pallet
					_colorIndex[line][pixel++] = colorOffset;											//Update Pixel
				}
			}


			//--------------BackGround End Tile--------------
			posX = pixel + scrollX;

			if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
				temp = _memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
			else
				temp = (int8_t)_memory->VRAM[BGTileMap + ((posY >> 3) << 5) + (posX >> 3)];
			temp <<= 4;

			pixelData1 = _memory->VRAM[BGWinTileData + temp];
			pixelData2 = _memory->VRAM[BGWinTileData + temp + 1];

			for (int i = 0; i < endPoint2; ++i) {
				colorOffset = BGcolor[(pixelData1 >> (7 - i) & 0x1) | (((pixelData2 >> (7 - i)) & 0x1) << 1)];		//Grab Tile Color Pallet
				_colorIndex[line][pixel++] = colorOffset;												//Update Pixel
			}
		}
		else {
			while (pixel < startWindow + endPoint2)
				_colorIndex[line][pixel++] = 0;
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
					temp = _memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				else
					temp = (int8_t)_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				temp <<= 4;

				pixelData1 = _memory->VRAM[ BGWinTileData + temp];
				pixelData2 = _memory->VRAM[ BGWinTileData + temp + 1 ];

				for (int i = posX; i < 8; ++i) {
					colorOffset = BGcolor[(pixelData1 >> (7-i) & 0x1) | (((pixelData2 >> (7-i)) & 0x1) << 1)];			//Grab Tile Color Pallet
					_colorIndex[line][pixel++] = colorOffset;												//Update Pixel
					++posX;
				}
			}

			//--------------Window Midsection--------------
			if (startWindow < 0)
				startWindow = winX;
			while (pixel < 152 - startWindow) {
				if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
					temp = _memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				else
					temp = (int8_t)_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
				temp <<= 4;

				pixelData1 = _memory->VRAM[ BGWinTileData + temp ];
				pixelData2 = _memory->VRAM[ BGWinTileData + temp + 1 ];

				for (int i = 0; i < 8; ++i) {
					colorOffset = BGcolor[ (pixelData1>>(7-i) & 0x1) | (((pixelData2>>(7-i)) & 0x1) << 1) ];		//Grab Tile Color Pallet
					_colorIndex[line][pixel++] = colorOffset;											//Update Pixel
					++posX;
				}
			}

			//--------------Window End Tile--------------
			if (LCDC & 0x10)											//Grab Tile number From BG Tile Map
				temp = _memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
			else
				temp = (int8_t)_memory->VRAM[ WinTileMap + ((posY >>3)<<5) + (posX>>3) ];
			temp <<= 4;

			pixelData1 = _memory->VRAM[ BGWinTileData + temp ];
			pixelData2 = _memory->VRAM[ BGWinTileData + temp + 1 ];

			int i = 0;
			while (pixel < 160) {
				colorOffset = BGcolor[ (pixelData1>>(7-i) & 0x1) | (((pixelData2>>(7-i)) & 0x1) << 1) ];			//Grab Tile Color Pallet
				_colorIndex[line][pixel++] = colorOffset;												//Update Pixel
				++i;
				++posX;
			}
		}
	}


	//---Draw Sprites---
	if (_enableSprites && (LCDC & 0b10)) {
		uint16_t spriteHeight = (LCDC & 0x4) ? 16 : 8;

		uint8_t spriteNum = 0;
		spriteData sprites[11] = {};
		spriteData tempSprite = {};

		temp = _memory->mmIO[OBP0];

		uint8_t OBP0color[4];
		OBP0color[3] = (temp & 0xC0) >> 6;
		OBP0color[2] = (temp & 0x30) >> 4;
		OBP0color[1] = (temp & 0x0C) >> 2;
		OBP0color[0] = 0xFF;

		temp = _memory->mmIO[OBP1];

		uint8_t OBP1color[4];
		OBP1color[3] = (temp & 0xC0) >> 6;
		OBP1color[2] = (temp & 0x30) >> 4;
		OBP1color[1] = (temp & 0x0C) >> 2;
		OBP1color[0] = 0xFF;
	
		uint8_t mask = (LCDC & 0x4) ? 0xFE : 0xFF;


		//Obtain Priority of Sprites Being Displayed
		for (int i = 0; i < 40 && spriteNum < 10; i++) {
			tempSprite.y		= (_memory->spriteRAM[i << 2] - 16);
			tempSprite.x		= (_memory->spriteRAM[(i << 2) + 1]);
			tempSprite.tileNum	= (_memory->spriteRAM[(i << 2) + 2]) & mask;
			tempSprite.flags	= (_memory->spriteRAM[(i << 2) + 3]);

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
				pixelData1 = _memory->VRAM[spriteLine];
				pixelData2 = _memory->VRAM[spriteLine + 1];

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
						if (spritePriority || (_colorIndex[line][(endPoint-j)+sprites[i].x] == BGcolor[0])) {
							colorOffset = spriteColor[(((pixelData1 >> (7 - j)) & 0x1) | (((pixelData2 >> (7 - j)) & 0x1) << 1))];	//Grab Tile Color Pallet
							if ( colorOffset != 0xFF)
								_colorIndex[line][(7-j)+sprites[i].x] = colorOffset;							//Update Pixel
						}
					}
				}   
				else {
					for (int j = startPoint; j <= endPoint; ++j) {
						if ( spritePriority || (_colorIndex[line][ (j-startPoint)+sprites[i].x ] == BGcolor[0]) ) {
							colorOffset = spriteColor[((pixelData1 >> (7 - j) & 0x1) | (((pixelData2 >> (7 - j)) & 0x1) << 1))];	//Grab Tile Color Pallet
							if ( colorOffset != 0xFF) 
								_colorIndex[line][ (j-startPoint)+sprites[i].x ] = colorOffset;									//Update Pixel
						}
					}
				}
			}
		}

	}

	//State Transition
	_memory->mmIO[LCDC_STAT]	&= 0b11111100;
	_currentState				 = &PPU::HBlank;

	//Interrupt Mode 0 H-Blank
	if (_memory->mmIO[LCDC_STAT] & 0x08)
		_memory->mmIO[IF] |= 2;

	return HBlankCycles;
}

int PPU::VBlank()
{
	int re = VBlankLineCycles;

	//State Transition
	if (_memory->mmIO[LY] >= 153) {
		_memory->mmIO[LY]			= 0;
		_memory->mmIO[LCDC_STAT]	= ( (_memory->mmIO[LCDC_STAT] & 0b11111100) | 0b10 );

		_currentState				= &PPU::readOAM;
		re							= OAMCycles;

		//Interrupt Mode 2 OAM
		if (_memory->mmIO[LCDC_STAT] & 0x20)
			_memory->mmIO[IF] |= 2;
			
	}
	else {
		//Interrupt LY coincidence
		if (_memory->mmIO[LY] == _memory->mmIO[LYC]) {
			if (_memory->mmIO[LCDC_STAT] & 0x40)
				_memory->mmIO[IF] |= 2;
			_memory->mmIO[LCDC_STAT] |= 0x4;
		}
		else
			_memory->mmIO[LCDC_STAT] &= ~(0x4);

		//Operation
		++_memory->mmIO[LY];
	}

	return re;
}
