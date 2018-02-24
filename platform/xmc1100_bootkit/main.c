//Tiny OS Demo
//
//This IS NOT a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "los_sys.h"
#include "los_tick.h"
#include "los_task.ph"
#include "los_config.h"

#include "los_bsp_led.h"
#include "los_bsp_uart.h"
#include "los_inspect_entry.h"
#include "los_demo_entry.h"

#include <XMC1100.h>
#include <xmc_scu.h>
#include <xmc_rtc.h>
#include <xmc_uart.h>
#include <xmc_gpio.h>
#include <xmc_flash.h>	

#include "lcd2004.h"
#include "XMC1000_TSE.h"
#include "rtc.h"

static uint32_t g_uwboadTaskID;
static  void LOS_BoardExampleTskfunc(void)
{
	__IO XMC_RTC_TIME_t now_rtc_time;
	int32_t old_temp_C;
	int32_t temp_C;
	int32_t temp_K;
	char testBuf[21];

	while (1)
	{		
	/* Calculate temperature of the chip in Kelvin */
		temp_K = XMC1000_CalcTemperature();
		/* Convert temperature to Celcius */
		temp_C = temp_K - ZERO_TEMP_KELVIN;		
				
		if(old_temp_C != temp_C)
		{
			sprintf(testBuf,"CoreTemp:%d 'C ", (int32_t)temp_C);	
			old_temp_C = temp_C;
			printf(testBuf);
			LCD_displayL(2, 0, (uint8_t*)testBuf);
		}
		
		XMC_RTC_GetTime((XMC_RTC_TIME_t *)&now_rtc_time);
//		printf("%02d:%02d:%02d\n", now_rtc_time.hours, now_rtc_time.minutes, now_rtc_time.seconds);			
		sprintf(testBuf, "%02d:%02d:%02d", now_rtc_time.hours, now_rtc_time.minutes, now_rtc_time.seconds);	
		printf(testBuf);
		LCD_displayL(0, 0, (uint8_t*)testBuf);
		
		LED_Toggle(0);	
		LED_Toggle(1);	
		LED_Toggle(2);	
		LED_Toggle(3);	
		LED_Toggle(4);	
		(void)LOS_TaskDelay(5000);
		
	}
}

void LOS_BoardExampleEntry(void)
{
    uint32_t uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_BoardExampleTskfunc;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "BoardDemo";
    stTaskInitParam.usTaskPrio = 10;
    uwRet = LOS_TaskCreate(&g_uwboadTaskID, &stTaskInitParam);

    if (uwRet != LOS_OK)
    {
        return;
    }
    return;
}

/*****************************************************************************
Function    : main
Description : Main function entry
Input       : None
Output      : None
Return      : None
 *****************************************************************************/

int main(void)
{
  uint32_t uwRet;

	LOS_EvbUartInit();
	LED_Initialize();
	
	DTS_Initialize();
	
	LCD_Initialize();
	
  printf ("\nXMC1100 Cortex M%u,%s @%u Hz\n",
	__CORTEX_M,
	VER,
	SystemCoreClock);
	
	RTC_Initialize();
		
    /*Init LiteOS kernel */
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK) {
        return LOS_NOK;
    }
    /* Enable LiteOS system tick interrupt */
    uwRet = LOS_EnableTick();
    if (uwRet != LOS_OK) {
        return LOS_NOK;
    }
   
    /*
        Notice: add your code here
        here you can create task for your function 
        do some hw init that need after systemtick init
     */

    LOS_Demo_Entry();

    LOS_Inspect_Entry();

    LOS_BoardExampleEntry();

    /* Kernel start to run */
    (void)LOS_Start();

		while(1)
		{
			//Should never come here
			assert(true);
		}

}
