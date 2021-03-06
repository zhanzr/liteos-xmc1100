/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
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

/* USER CODE BEGIN Includes */
//#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cassert>

//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <assert.h>

#include "los_sys.h"
#include "los_tick.h"
#include "los_task.h"
#include "los_config.h"

#include "los_inspect_entry.h"
#include "los_demo_entry.h"

#include "retarget_io_drv.h"

#include "LiquidCrystal.h"

//using namespace std;
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
#define	ADC_CHAN_NO	3
volatile uint16_t g_ADCBuf[ADC_CHAN_NO];

//Re-implement any functions that require re-implementation.
//namespace std {
//  struct __FILE
//  {
//    int handle;
//    /* Whatever you require here. If the only file you are using is */
//    /* standard output using printf() for debugging, no file handling */
//    /* is required. */
//  };
//  FILE __stdout;
//  FILE __stdin;
//  FILE __stderr;
//  int fgetc(FILE *f)
//  {
//    /* Your implementation of fgetc(). */
//    return 0;
//  }
//  int fputc(int c, FILE *stream)
//  {
//		stdout_putchar(c);
//		return c;
//  }
//  int ferror(FILE *stream)
//  {
//    fputc('E', stream);
//    fputc('\n', stream);
//		
//		return -1;
//  }
//  long int ftell(FILE *stream)
//  {
//    fputc('T', stream);
//    fputc('\n', stream);
//		
//		return 0;
//  }
//  int fclose(FILE *f)
//  {
//    /* Your implementation of fclose(). */
//    return 0;
//  }
//  int fseek(FILE *f, long nPos, int nMode)
//  {
//    /* Your implementation of fseek(). */
//    return 0;
//  }
//  int fflush(FILE *f)
//  {
//    /* Your implementation of fflush(). */    
//    return 0;
//  }
//}

static uint32_t g_uwboadTaskID;
static  void LOS_BoardExampleTskfunc(void)
{
//	int32_t old_temp_C;
//	int32_t temp_C;
//	int32_t temp_K;
	char testBuf[21];
	LiquidCrystal lcd;
	lcd.Display(0, 0, (uint8_t*)__DATE__);
	lcd.Display(1, 0, (uint8_t*)__TIME__);
	
	HAL_ADC_Start_DMA(&hadc, (uint32_t*)g_ADCBuf, ADC_CHAN_NO);
	
	while (1)
	{		
		sprintf((char*)testBuf, "%u %u %u",
		//		printf("%u %u %u\n",
				g_ADCBuf[0], g_ADCBuf[1], g_ADCBuf[2]
				);
//		cout<<testBuf<<endl;
		printf(testBuf);
		printf("\n");
			
		LED_Toggle(0);
		LED_Toggle(1);
		(void)LOS_TaskDelay(2000);

		HAL_IWDG_Refresh(&hiwdg);		

		lcd.Display(rand()%LiquidCrystal::LINE_NUM, 0, (uint8_t*)testBuf);
		
		LED_Toggle(2);
		LED_Toggle(3);
		(void)LOS_TaskDelay(2000);
		
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

    if (uwRet != OS_OK)
    {
        return;
    }
    return;
}

extern void LOS_Demo_Entry(void);
extern void LOS_Inspect_Entry(void);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t uwRet;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_CRC_Init();
  MX_I2C2_Init();
  MX_IWDG_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	printf("F072 Disco %s, CM%u @ %u Hz\n %u %u %u\n",
		VER,
		__CORTEX_M,
		SystemCoreClock,
		*(uint16_t*)(0x1FFFF7B8),
		*(uint16_t*)(0x1FFFF7C2),
		*(uint16_t*)(0x1FFFF7BA)
		);
	printf("%08X, %08X\n", SCB->CPUID, (1UL << SCB_AIRCR_ENDIANESS_Pos));
			
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	
  /* USER CODE END 2 */

    /*Init LiteOS kernel */
    uwRet = LOS_KernelInit();
    if (uwRet != OS_OK) {
        return OS_NOK;
    }
    /* Enable LiteOS system tick interrupt */
    uwRet = LOS_EnableTick();
    if (uwRet != OS_OK) {
        return OS_NOK;
    }

    LOS_Demo_Entry();

    LOS_Inspect_Entry();

    LOS_BoardExampleEntry();

    /* Kernel start to run */
    (void)LOS_Start();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			//Should never come here
			assert(true);		
//		sprintf((char*)testBuf, "%03X %03X %03X",
////		printf("%u %u %u\n",
//		g_ADCBuf[0], g_ADCBuf[1], g_ADCBuf[2]
//		);
//		cout<<testBuf<<endl;
//		
//		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
//		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
//		HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
//		HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
//		
//		HAL_IWDG_Refresh(&hiwdg);		
//		
//		HAL_Delay(5000);	

//		lcd.Display(rand()%LiquidCrystal::LINE_NUM, 0, testBuf);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/SYS_TICK_HZ);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
