#include "los_sys.h"
#include "los_tick.h"
#include "los_task.ph"
#include "los_config.h"

#include "los_bsp_led.h"
//#include "los_bsp_key.h"
#include "los_bsp_uart.h"
#include "los_inspect_entry.h"
#include "los_demo_entry.h"
#include "los_bsp_adapter.h"

#ifdef LOS_XMC1100
	#include <stdint.h>
	#include <stdlib.h>
	#include <assert.h>

	#include <XMC1100.h>
	#include <xmc_scu.h>
	#include <xmc_rtc.h>
	#include <xmc_uart.h>
	#include <xmc_gpio.h>
	#include <xmc_flash.h>
	
	#include "lcd2004.h"
	#include "XMC1000_TSE.h"
#endif

#ifdef LOS_CMBACKTRACE_COMPILE
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"
#endif

__IO uint32_t g_Ticks;

XMC_RTC_CONFIG_t rtc_config =
{
  .time.seconds = 5U,
  .prescaler = 0x7fffU
};     

XMC_RTC_TIME_t init_rtc_time = 
{
	.year = 2018,
	.month = XMC_RTC_MONTH_FEBRUARY,
	.daysofweek = XMC_RTC_WEEKDAY_TUESDAY,
	.days = 20,
	.hours = 15,
	.minutes = 32,
	.seconds = 59	
};

int stdout_putchar (int ch)
{
	XMC_UART_CH_Transmit(XMC_UART0_CH1, (uint8_t)ch);
	
	return ch;
}

static UINT32 g_uwboadTaskID;
static LITE_OS_SEC_TEXT VOID LOS_BoardExampleTskfunc(VOID)
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
					__WFI();
		(void)LOS_TaskDelay(5000);
	}
}

void LOS_BoardExampleEntry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
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
LITE_OS_SEC_TEXT_INIT
int main(void)
{
  UINT32 uwRet;

	LOS_EvbSetup();
	
	/* Enable DTS */
	XMC_SCU_StartTempMeasurement();
	LCD_Initialize();
	
  printf ("\nXMC1100 %s @%u Hz\n",
	VER,
	SystemCoreClock);
	
	//RTC
  XMC_RTC_Init(&rtc_config);
	
	XMC_RTC_SetTime(&init_rtc_time);
  XMC_RTC_EnableEvent(XMC_RTC_EVENT_PERIODIC_MINUTES);
  XMC_SCU_INTERRUPT_EnableEvent(XMC_RTC_EVENT_PERIODIC_MINUTES);
  NVIC_SetPriority(SCU_1_IRQn, 3);
  NVIC_EnableIRQ(SCU_1_IRQn);
  XMC_RTC_Start();
		
#ifdef LOS_CMBACKTRACE_COMPILE
    cm_backtrace_init("LOS_CmBacktrace", HARDWARE_VERSION, SOFTWARE_VERSION);
#endif

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
