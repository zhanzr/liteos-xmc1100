#include "retarget_io_drv.h"

#include "los_config.h"

#if defined(__ARMCC_VERSION)
int stdout_putchar (int ch)
{
	uint8_t c = ch;
	HAL_UART_Transmit(&huart2, &c, 1, 1);
	return ch;
}
#else
int _write (int fd, const void *buf, size_t count)
{
	for(uint32_t i=0; i<count; ++i)
	{
		HAL_UART_Transmit(&huart2, buf+i, 1, 1);
	}
	return count;
}
#endif

//TODO: Move to other place
void LED_On(uint8_t num)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

void LED_Off(uint8_t num)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

void LED_Toggle(uint8_t num)
{
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
}


/*****************************************************************************
    global var
 *****************************************************************************/
/*
    if g_use_ram_vect == 1, we should use sct file ram_vect.sct
    and we can use LOS_HwiCreate(), LOS_HwiDelete() dynamically regist a irq func
    if g_use_ram_vect == 0, we use default vector table in rom start at address 0x00000000
*/
const unsigned char g_use_ram_vect = 0;

/*****************************************************************************
    LOS function extern 
 *****************************************************************************/
extern void LOS_SetTickSycle(unsigned int);
extern void LOS_TickHandler(void);
extern unsigned int osGetVectorAddr(void);

/*****************************************************************************
 Function    : osTickStart
 Description : Set system tick reqister, and start system tick exception
              Note that This function is called by LOS_Start() In LiteOS kernel.
              so, please don't init system tick before LiteOS kernel Inited
              in your own system.
 Input       : None
 Output      : None
 Return      : OS_OK
 *****************************************************************************/
unsigned int osTickStart(void)
{
    unsigned int uwRet = 0;
	
    LOS_SetTickSycle(SystemCoreClock / LOSCFG_BASE_CORE_TICK_PER_SECOND);
    
    return uwRet;
}

/*****************************************************************************
 Function    : LosAdapIntInit
 Description : config vector table in ram. please see g_use_ram_vect illustration
               this is used for arm cortex-M3/M4
 Input       : None
 Output      : None
 Return      : OS_OK
 *****************************************************************************/
void LosAdapIntInit(void)
{
    /*
        1:ReLoad vector table address at ram . if do nothing , vector table is
        located in rom 0x00000000
        2:set nvic irq priority group
        
        Note: here can be replaced by some function , for example in Stm32 bsp
        you can just call SCB->VTOR = osGetVectorAddr(); and
        NVIC_SetPriorityGrouping(OS_NVIC_AIRCR_PRIGROUP);
    */
//    if (g_use_ram_vect)
//    {
			//TODO: Relocate the vector table, for the core types which doesn't have vector offset feature(such as Cortex M0),
			//other methods should be used
//        *(volatile UINT32 *)OS_NVIC_VTOR = osGetVectorAddr();
//        *(volatile UINT32 *)OS_NVIC_AIRCR = (0x05FA0000 | OS_NVIC_AIRCR_PRIGROUP << 8);
//    }

    return;
}

/*****************************************************************************
 Function    : LosAdapIrqEnable
 Description : external interrupt enable, and set priority
               this function is called by LOS_HwiCreate(),
               so here can use bsp func to inplemente it 
               Note : if don't use LOS_HwiCreate(), leave it empty
 Input       : irqnum: external interrupt number
               prior: priority of this interrupt
 Output      : None
 Return      : None
 *****************************************************************************/
void LosAdapIrqEnable(unsigned int irqnum, unsigned short prior)
{       
	NVIC_EnableIRQ((IRQn_Type)irqnum);
    
	NVIC_SetPriority((IRQn_Type)irqnum, prior);

	return;
}

/*****************************************************************************
 Function    : LosAdapIrqDisable
 Description : external interrupt disable
               this function is called by LOS_HwiDelete(), so use bsp func
               to inplemente it
               Note : if don't use LOS_HwiDelete(), leave it empty
 Input       : irqnum: external interrupt number
 Output      : None
 Return      : None
 *****************************************************************************/
void LosAdapIrqDisable(unsigned int irqnum)
{
    NVIC_DisableIRQ((IRQn_Type)irqnum);
    return;
}

