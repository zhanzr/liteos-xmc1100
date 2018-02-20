#ifndef __LOS_BSP_UART_H__
#define	__LOS_BSP_UART_H__

void LOS_EvbUartInit(void);

void LOS_EvbUartWriteByte(char c);

void LOS_EvbUartReadByte(char* c);

void LOS_EvbUartWriteStr(const char* str);

#endif
