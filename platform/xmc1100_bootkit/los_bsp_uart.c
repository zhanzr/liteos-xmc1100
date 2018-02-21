//TODO: Need Rewrite according to the CMSIS Standard
//The kernel should not impose any naming rule on the user part code.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <XMC1100.h>
#include <xmc_uart.h>
#include <xmc_gpio.h>

#include "los_bsp_uart.h"

void LOS_EvbUartInit(void)
{
	#define UART_RX P1_3
	#define UART_TX P1_2

	XMC_GPIO_CONFIG_t uart_tx;
	XMC_GPIO_CONFIG_t uart_rx;

	/* UART configuration */
	const XMC_UART_CH_CONFIG_t uart_config = 
	{	
		.data_bits = 8U,
		.stop_bits = 1U,
		.baudrate = 256000
	};
	/*Initialize the UART driver */
	uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7;
	uart_rx.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
 /* Configure UART channel */
  XMC_UART_CH_Init(XMC_UART0_CH1, &uart_config);
  XMC_UART_CH_SetInputSource(XMC_UART0_CH1, XMC_UART_CH_INPUT_RXD,USIC0_C1_DX0_P1_3);
  
	/* Start UART channel */
  XMC_UART_CH_Start(XMC_UART0_CH1);

  /* Configure pins */
	XMC_GPIO_Init(UART_TX, &uart_tx);
  XMC_GPIO_Init(UART_RX, &uart_rx);
}
