#ifndef LiquidCrystal_h
#define LiquidCrystal_h

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "stm32f0xx_hal.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
// commands
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

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal{
public:
	LiquidCrystal(void);

	void init(void);
    
  void SetPos(uint8_t l,uint8_t hori)
	{
		const static uint8_t pos_tab[] = {0x80, 0xc0};
		
		WrCmd_4(pos_tab[l%LINE_NUM] + hori);	
	}
	
	void Display(uint8_t l,uint8_t hori,uint8_t *s)
	{
		SetPos(l, hori);
		while(*s)
		{
			WrDat_4(*s);
			s++;
		}
	}
	
	static void SimpleDelay(uint32_t t);
	static const uint8_t LINE_NUM = 2;
	
private:
	void RS_H(void)
	{	
		HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
	}

	void RS_L(void)
	{
		HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET);
	}

	void RW_H(void)
	{
		HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_SET);
	}

	void RW_L(void)
	{
		HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_RESET);
	}

	void E_H(void)
	{
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
	}

	void E_L(void)
	{
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);
	}

	void D4_H(void)
	{
		HAL_GPIO_WritePin(DB4_GPIO_Port, DB4_Pin, GPIO_PIN_SET);
	}

	void D4_L(void)
	{
		HAL_GPIO_WritePin(DB4_GPIO_Port, DB4_Pin, GPIO_PIN_RESET);
	}

	void D5_H(void)
	{
		HAL_GPIO_WritePin(DB5_GPIO_Port, DB5_Pin, GPIO_PIN_SET);
	}

	void D5_L(void)
	{
		HAL_GPIO_WritePin(DB5_GPIO_Port, DB5_Pin, GPIO_PIN_RESET);
	}

	void D6_H(void)
	{
		HAL_GPIO_WritePin(DB6_GPIO_Port, DB6_Pin, GPIO_PIN_SET);
	}

	void D6_L(void)
	{
		HAL_GPIO_WritePin(DB6_GPIO_Port, DB6_Pin, GPIO_PIN_RESET);
	}

	void D7_H(void)
	{
		HAL_GPIO_WritePin(DB7_GPIO_Port, DB7_Pin, GPIO_PIN_SET);
	}

	void D7_L(void)
	{
		HAL_GPIO_WritePin(DB7_GPIO_Port, DB7_Pin, GPIO_PIN_RESET);
	}
	
	uint32_t ReadD7(void)
	{
		return HAL_GPIO_ReadPin(DB7_GPIO_Port, DB7_Pin);
	}
	
void DB4_Wr(uint8_t dat)
	{
		(0==(dat&0x08))?D7_L():D7_H();
		(0==(dat&0x04))?D6_L():D6_H();
		(0==(dat&0x02))?D5_L():D5_H();
		(0==(dat&0x01))?D4_L():D4_H();
		SimpleDelay(10);
	}

	void WaitAvail(void)
	{
		uint32_t tmpTimeOut = 12000;
		
		DB4_Wr(0xff);
		SimpleDelay(10);

		RS_L(); 
		SimpleDelay(10);
		RW_H();
		SimpleDelay(10);
		E_H();
		SimpleDelay(10);
		while((0!=ReadD7()) && (0!=tmpTimeOut))
		{
			__NOP();
			--tmpTimeOut;
		}
		SimpleDelay(10);
		E_L(); 
	}

	void WrCmd_4 (uint8_t cmd)
	{
		WaitAvail();

		SimpleDelay(10);

		RS_L(); 
		SimpleDelay(10);
		RW_L();   	
		SimpleDelay(10);
		DB4_Wr(cmd>>4);
		SimpleDelay(10);
		E_H();
		SimpleDelay(10);
		E_L();
		SimpleDelay(10);
		DB4_Wr(cmd);
		SimpleDelay(10);
		E_H();
		SimpleDelay(10);
		E_L();	
		SimpleDelay(10);
	}

	void WrDat_4 (uint8_t dat)
	{
		WaitAvail();
		SimpleDelay(10);		
		RS_H(); 
		SimpleDelay(10);		
		RW_L();   	
		SimpleDelay(10);		
		DB4_Wr(dat>>4);
		SimpleDelay(10);		
		E_H();
		SimpleDelay(10);		
		E_L();
		SimpleDelay(10);		
		DB4_Wr(dat);
		SimpleDelay(10);		
		E_H();
		SimpleDelay(10);		
		E_L();		
		SimpleDelay(10);		
	}	
};

#ifdef __cplusplus
}
#endif

#endif
