#include "retarget_io_drv.h"

#if defined(__ARMCC_VERSION)
int stdout_putchar (int ch)
{
	uint8_t c = ch;
	HAL_UART_Transmit(&huart1, &c, 1, 1);
	return ch;
}
#else
int _write (int fd, const void *buf, size_t count)
{
	for(uint32_t i=0; i<count; ++i)
	{
		HAL_UART_Transmit(&huart1, buf+i, 1, 1);
	}
	return count;
}
#endif
