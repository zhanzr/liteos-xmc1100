#ifndef __LOS_BSP_LED_H__
#define	__LOS_BSP_LED_H__

#ifdef LOS_XMC1100
	#include <stdint.h>
	#include <assert.h>

	#include <xmc_gpio.h>
#endif

void LED_Initialize (void) ;
void LED_Uninitialize (void) ;

void LED_On (uint8_t num) ;
void LED_Off (uint8_t num);
void LED_Toggle (uint8_t num);

#endif
