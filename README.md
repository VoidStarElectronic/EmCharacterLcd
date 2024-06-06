# EmCharacterLcd
 Stm32 Character Lcd driver
 
Example 1 :
* 4 bit mode
```
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "EmCharacterLcd.h"

static void delayUs(uint32_t d)
{
	HAL_TIM_Base_Start(&HTIM_US);
	__HAL_TIM_SET_COUNTER(&HTIM_US, 0);
	while(__HAL_TIM_GET_COUNTER(&HTIM_US) < d);
	HAL_TIM_Base_Stop(&HTIM_US);
}

static const EmCharacterLcdConfiguration_t lcd20x4Configuration=
{
	.colSize = 20,
	.rowSize = 4,
	.delayUs = delayUs,

	.rs = {LCD_RS_GPIO_Port, LCD_RS_Pin},
	.en = {LCD_EN_GPIO_Port, LCD_EN_Pin},
	
	.d4 = { LCD_D4_GPIO_Port, LCD_D4_Pin },
	.d5 = { LCD_D5_GPIO_Port, LCD_D5_Pin },
	.d6 = { LCD_D6_GPIO_Port, LCD_D6_Pin },
	.d7 = { LCD_D7_GPIO_Port, LCD_D7_Pin },
};

EmCharacterLcd_t lcd20x4;

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM14_Init();

	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	lcd20x4 = EmCharacterLcd(&lcd20x4Configuration);
	
	EmCharacterLcd__clear(lcd20x4);
	EmCharacterLcd__printf(lcd20x4, 0, 0, " This is Electronic Man.\n");

	while(1)
	{
		EmCharacterLcd__update(lcd20x4);
	}
}
```

Example 1 :
* 8 bit mode
```
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "EmCharacterLcd.h"

static void delayUs(uint32_t d)
{
	HAL_TIM_Base_Start(&HTIM_US);
	__HAL_TIM_SET_COUNTER(&HTIM_US, 0);
	while(__HAL_TIM_GET_COUNTER(&HTIM_US) < d);
	HAL_TIM_Base_Stop(&HTIM_US);
}

static const EmCharacterLcdConfiguration_t lcd20x4Configuration=
{
	.colSize = 20,
	.rowSize = 4,
	.delayUs = delayUs,

	.rs = {LCD_RS_GPIO_Port, LCD_RS_Pin},
	.en = {LCD_EN_GPIO_Port, LCD_EN_Pin},
	
	.d0 = { LCD_D0_GPIO_Port, LCD_D0_Pin },
	.d1 = { LCD_D1_GPIO_Port, LCD_D1_Pin },
	.d2 = { LCD_D2_GPIO_Port, LCD_D2_Pin },
	.d3 = { LCD_D3_GPIO_Port, LCD_D3_Pin },
	.d4 = { LCD_D4_GPIO_Port, LCD_D4_Pin },
	.d5 = { LCD_D5_GPIO_Port, LCD_D5_Pin },
	.d6 = { LCD_D6_GPIO_Port, LCD_D6_Pin },
	.d7 = { LCD_D7_GPIO_Port, LCD_D7_Pin },
};

EmCharacterLcd_t lcd20x4;

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM14_Init();

	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	lcd20x4 = EmCharacterLcd(&lcd20x4Configuration);
	
	EmCharacterLcd__clear(lcd20x4);
	EmCharacterLcd__printf(lcd20x4, 0, 0, " This is Electronic Man.\n");

	while(1)
	{
		EmCharacterLcd__update(lcd20x4);
	}
}
```

Example 3 :
*special character

```
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "EmCharacterLcd.h"

static void delayUs(uint32_t d)
{
	HAL_TIM_Base_Start(&HTIM_US);
	__HAL_TIM_SET_COUNTER(&HTIM_US, 0);
	while(__HAL_TIM_GET_COUNTER(&HTIM_US) < d);
	HAL_TIM_Base_Stop(&HTIM_US);
}

static const EmCharacterLcdConfiguration_t lcd20x4Configuration=
{
	.colSize = 20,
	.rowSize = 4,
	.delayUs = delayUs,

	.rs = {LCD_RS_GPIO_Port, LCD_RS_Pin},
	.en = {LCD_EN_GPIO_Port, LCD_EN_Pin},
	
	.d4 = { LCD_D4_GPIO_Port, LCD_D4_Pin },
	.d5 = { LCD_D5_GPIO_Port, LCD_D5_Pin },
	.d6 = { LCD_D6_GPIO_Port, LCD_D6_Pin },
	.d7 = { LCD_D7_GPIO_Port, LCD_D7_Pin },
};


static const uint8_t charBackSlash [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
static const char loading[8] = {'|', '/', '-', 0};
  
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM14_Init();

	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	EmCharacterLcd_t lcd20x4 = EmCharacterLcd(&lcd20x4Configuration);

	while(1)
	{
		for(uint32_t i = 0 ; i < 4 ; ++i )
		{
			EmCharacterLcd__clear(lcd20x4);
			EmCharacterLcd__printf(lcd20x4, 0, 0, "-Loading %c",loading[i]);
			EmCharacterLcd__update(lcd20x4);
			HAL_Delay(100);
		}
	}
}
```