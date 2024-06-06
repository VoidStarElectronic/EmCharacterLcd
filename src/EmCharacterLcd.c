
#include "EmCharacterLcd.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"

#define PARSE_BOOL(X)             (((X) == 0) ? 0 : 1)
#define PARSE_GPIO_PinState(X)    (((X) == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET)


static inline void EmCharacterLcd__enablePulse(EmCharacterLcd_t self)
{
	HAL_GPIO_WritePin(self->config->en.gpioX, self->config->en.gpioPin, GPIO_PIN_SET);
	self->config->delayUs(50);
	HAL_GPIO_WritePin(self->config->en.gpioX, self->config->en.gpioPin, GPIO_PIN_RESET);
	self->config->delayUs(50);
}
static inline void EmCharacterLcd__write4Bit(EmCharacterLcd_t self, uint8_t data)
{
	Pin_t* p = (Pin_t*)(&self->config->d4);

	for( uint32_t i = 0 ; i < 4 ; ++i )
	{
		HAL_GPIO_WritePin(p->gpioX, p->gpioPin, PARSE_GPIO_PinState(data & (1 << i)));
		++p;
	}

	EmCharacterLcd__enablePulse(self);
}
static inline void EmCharacterLcd__write8Bit(EmCharacterLcd_t self, uint8_t data)
{
	Pin_t* p = (Pin_t*)(&self->config->d0);

	for( uint32_t i = 0 ; i < 8 ; ++i )
	{
		HAL_GPIO_WritePin(p->gpioX, p->gpioPin, PARSE_GPIO_PinState(data & (1 << i)));
		++p;
	}

	EmCharacterLcd__enablePulse(self);
}
static void EmCharacterLcd__writeByte(EmCharacterLcd_t self, uint8_t data)
{
	if((self->config->d0.gpioX != NULL)&(self->config->d1.gpioX != NULL)&(self->config->d2.gpioX != NULL)&(self->config->d3.gpioX != NULL))
	{
		EmCharacterLcd__write8Bit(self, data);
	}
	else
	{
		EmCharacterLcd__write4Bit(self, data >> 4);
		EmCharacterLcd__write4Bit(self, data);
	}
}
static void EmCharacterLcd__sendData(EmCharacterLcd_t self, uint8_t data)
{
	HAL_GPIO_WritePin(self->config->rs.gpioX, self->config->rs.gpioPin, GPIO_PIN_SET);
	EmCharacterLcd__writeByte(self, data);
}
static void EmCharacterLcd__sendCommand(EmCharacterLcd_t self, uint8_t data)
{
	HAL_GPIO_WritePin(self->config->rs.gpioX, self->config->rs.gpioPin, GPIO_PIN_RESET);
	EmCharacterLcd__writeByte(self, data);
}
static void EmCharacterLcd__setCursor(EmCharacterLcd_t self, uint8_t x, uint8_t y)
{
	static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	EmCharacterLcd__sendCommand(self,  0x80 |(x + row_offsets[y]));
}


static void EmCharacterLcd__setup(EmCharacterLcd_t self)
{
	/* COMMANDS */
	#define LCD_CLEARDISPLAY 0x01
	#define LCD_RETURNHOME 0x02
	#define LCD_ENTRYMODESET 0x04
	#define LCD_DISPLAYCONTROL 0x08
	#define LCD_CURSORSHIFT 0x10
	#define LCD_FUNCTIONSET 0x20
	#define LCD_SETCGRAMADDR 0x40
	#define LCD_SETDDRAMADDR 0x80

	// flags for display entry mode
	#define LCD_ENTRYRIGHT 0x00
	#define LCD_ENTRYLEFT 0x02
	#define LCD_ENTRYSHIFTINCREMENT 0x01
	#define LCD_ENTRYSHIFTDECREMENT 0x00

	// flags for display on/off control
	#define LCD_DISPLAYON 0x04
	#define LCD_DISPLAYOFF 0x00
	#define LCD_CURSORON 0x02
	#define LCD_CURSOROFF 0x00
	#define LCD_BLINKON 0x01
	#define LCD_BLINKOFF 0x00

	// flags for function set
	#define LCD_8BITMODE 0x10
	#define LCD_4BITMODE 0x00
	#define LCD_2LINE 0x08
	#define LCD_1LINE 0x00
	#define LCD_5x10DOTS 0x04
	#define LCD_5x8DOTS 0x00

	uint8_t display_settings;


	HAL_GPIO_WritePin(self->config->rs.gpioX, self->config->rs.gpioPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(self->config->en.gpioX, self->config->en.gpioPin, GPIO_PIN_RESET);

	//1. Wait at least 15ms
	HAL_Delay(20);
	//2. Attentions sequence
	EmCharacterLcd__sendCommand(self, 0x03);
	HAL_Delay(5);
	EmCharacterLcd__sendCommand(self, 0x03);
	HAL_Delay(5);
	EmCharacterLcd__sendCommand(self, 0x03);
	HAL_Delay(5);
	EmCharacterLcd__sendCommand(self, 0x02);
	HAL_Delay(5);

	display_settings = LCD_2LINE | LCD_5x8DOTS;

	if((self->config->d0.gpioX != NULL)&(self->config->d1.gpioX != NULL)&(self->config->d2.gpioX != NULL)&(self->config->d3.gpioX != NULL))
	{
		display_settings |= LCD_8BITMODE;
	}
	else
	{
		display_settings |= LCD_4BITMODE;
	}

	EmCharacterLcd__sendCommand(self, LCD_FUNCTIONSET | display_settings);
	HAL_Delay(5);

	display_settings = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	EmCharacterLcd__sendCommand(self, LCD_DISPLAYCONTROL | display_settings);
	HAL_Delay(5);

	EmCharacterLcd__sendCommand(self, LCD_CLEARDISPLAY);
	HAL_Delay(5);

	display_settings =  LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	EmCharacterLcd__sendCommand(self, LCD_ENTRYMODESET | display_settings);
	HAL_Delay(5);

	EmCharacterLcd__clear(self);
}

EmCharacterLcd_t EmCharacterLcd(const EmCharacterLcdConfiguration_t* config)
{
	assert(config);
	assert(config->colSize);
	assert(config->rowSize);
	assert(config->en.gpioX);
	assert(config->rs.gpioX);
	assert(config->d7.gpioX);
	assert(config->d6.gpioX);
	assert(config->d5.gpioX);
	assert(config->d4.gpioX);
	assert(config->delayUs);

	EmCharacterLcd_t instance;
	instance = (EmCharacterLcd_t)malloc(sizeof(*instance));
	if(instance)
	{
		instance->config = config;
		instance->buffer = (uint8_t*)malloc(config->colSize * config->rowSize);
		if(instance->buffer)
		{
			EmCharacterLcd__setup(instance);
		}
		else
		{
			free(instance);
		}
	}
	return instance;
}

void _EmCharacterLcd(EmCharacterLcd_t self)
{
	free(self);
}

void EmCharacterLcd__clear(EmCharacterLcd_t self)
{
	assert(self);

	const uint32_t size = self->config->colSize* self->config->rowSize;

	for(uint32_t i = 0 ; i < size ; ++i)
	{
		self->buffer[i] = ' ';
	}
}

void EmCharacterLcd__printf(EmCharacterLcd_t self, uint8_t x, uint8_t y, const char *fmt, ... )
{
	assert(self);

//	int32_t address = x + self->config->colSize * y;
//	int32_t size = self->config->colSize * self->config->rowSize;
//	size -= address;
//
//	va_list args;
//	va_start(args, fmt);
//	vsnprintf((char*)(self->buffer + address), size, fmt, args);
//	va_end(args);


	int32_t address = x + self->config->colSize * y;
	const int32_t size = self->config->colSize * self->config->rowSize;
	uint8_t  buffer[80];
	uint32_t count = 0;

	va_list args;
	va_start(args, fmt);
	const uint32_t convertedSize = vsnprintf((char*)buffer, size, fmt, args);
	va_end(args);

    while((count < convertedSize)&(address < size))
    {
    	if(buffer[count] == '\n' || buffer[count] == '\r')
    	{
    		address += self->config->colSize - (address % self->config->colSize);
    	}
    	else if(buffer[count] == '\t')
        {
    		address += 4;
        }
        else
    	{
    		*(self->buffer + address) = buffer[count];
    		++address;
    	}
    	++count;
    }
}

void EmCharacterLcd__update(EmCharacterLcd_t self)
{
	assert(self);

	for(uint32_t y = 0 ; y < self->config->rowSize ; ++y)
	{
		EmCharacterLcd__setCursor(self, 0, y);
		for(uint32_t x = 0 ; x < self->config->colSize ; ++x)
		{
			EmCharacterLcd__sendData(self, self->buffer[x + (self->config->colSize * y)]);
		}
	}
}

void EmCharacterLcd__createChar(EmCharacterLcd_t self, uint8_t address, const uint8_t* characterTable)
{
	assert(self);

	address &= 0x7;
	EmCharacterLcd__sendCommand(self, 0x40 | (address << 3));
	for (uint32_t i = 0 ; i < 8 ; ++i)
	{
		EmCharacterLcd__sendData(self, characterTable[i]);
	}
}


