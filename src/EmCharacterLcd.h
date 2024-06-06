#ifndef EMCHARACTERLCD_H_
#define EMCHARACTERLCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "stdint.h"

typedef struct
{
	GPIO_TypeDef* 	gpioX;
	uint32_t 		gpioPin;
}Pin_t;

typedef struct
{
	Pin_t rs;
	Pin_t en;

	Pin_t d0;
	Pin_t d1;
	Pin_t d2;
	Pin_t d3;
	Pin_t d4;
	Pin_t d5;
	Pin_t d6;
	Pin_t d7;

	uint8_t rowSize;
	uint8_t colSize;

	void(*delayUs)(uint32_t);

}EmCharacterLcdConfiguration_t;

typedef struct
{
	const EmCharacterLcdConfiguration_t* config;
	uint8_t* buffer;
}*EmCharacterLcd_t;

EmCharacterLcd_t EmCharacterLcd(const EmCharacterLcdConfiguration_t* config);
void 			_EmCharacterLcd(EmCharacterLcd_t self);
void 			 EmCharacterLcd__createChar(EmCharacterLcd_t self, uint8_t address, const uint8_t* characterTable);
void 			 EmCharacterLcd__clear(EmCharacterLcd_t self);
void			 EmCharacterLcd__printf(EmCharacterLcd_t self, uint8_t x, uint8_t y, const char *fmt, ... );
void 		     EmCharacterLcd__update(EmCharacterLcd_t self);

#ifdef __cplusplus
}
#endif

#endif /* EMCHARACTERLCD_H_ */
