//Tiny OS Hardware driver.
//This file is only for Cortex M0 core, for others cores, use conditional directive to use other drivers.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

#include "los_hwi.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*lint -save -e40 -e522 -e533*/

extern void LOS_TickHandler(void);
extern void LosAdapIrqEnable(unsigned int irqnum, unsigned short prior);
extern void LosAdapIrqDisable(unsigned int irqnum);
extern void LosAdapIntInit(void);
extern void SysTick_Handler(void);
/*lint -restore*/
uint32_t  g_vuwIntCount = 0;

__attribute__ ((section(".vector.bss")))
HWI_PROC_FUNC m_pstHwiForm[OS_M0_VECTOR_CNT] =
{
  0,                    // [0] Top of Stack
  Reset_Handler,        // [1] reset
  osHwiDefaultHandler,  // [2] NMI Handler
  osHwiDefaultHandler,  // [3] Hard Fault Handler
  osHwiDefaultHandler,  // [4] MPU Fault Handler
  osHwiDefaultHandler,  // [5] Bus Fault Handler
  osHwiDefaultHandler,  // [6] Usage Fault Handler
  0,                    // [7] Reserved
  0,                    // [8] Reserved
  0,                    // [9] Reserved
  0,                    // [10] Reserved
  osHwiDefaultHandler,  // [11] SVCall Handler
  osHwiDefaultHandler,  // [12] Debug Monitor Handler
  0,                    // [13] Reserved
  PendSV_Handler,       // [14] PendSV Handler
  SysTick_Handler,      // [15] SysTick Handler
};
HWI_PROC_FUNC m_pstHwiSlaveForm[OS_M0_VECTOR_CNT] = {0};

/*****************************************************************************
 Function    : osIntNumGet
 Description : Get a interrupt number
 Input       : None
 Output      : None
 Return      : Interrupt Indexes number
 *****************************************************************************/
 uint32_t osIntNumGet(void)
{
    return __get_IPSR();
}

/*****************************************************************************
 Function    : osHwiDefaultHandler
 Description : default handler of the hardware interrupt
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
 void  osHwiDefaultHandler(void)
{
    uint32_t irq_num = osIntNumGet();
    while(1);
}

/*****************************************************************************
 Function    : osInterrupt
 Description : Hardware interrupt entry function
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
 void  osInterrupt(void)
{
    uint32_t uwHwiIndex;
    uint32_t uwIntSave;

    uwIntSave = LOS_IntLock();
    g_vuwIntCount++;
    LOS_IntRestore(uwIntSave);

    uwHwiIndex = osIntNumGet();

    if (m_pstHwiSlaveForm[uwHwiIndex] !=0)
    {
        m_pstHwiSlaveForm[uwHwiIndex]();
    }

    uwIntSave = LOS_IntLock();
    g_vuwIntCount--;
    LOS_IntRestore(uwIntSave);
}

/*****************************************************************************
 Function    : osHwiInit
 Description : initialization of the hardware interrupt
 Input       : None
 Output      : None
 Return      : OS_SUCCESS
 *****************************************************************************/
uint32_t osGetVectorAddr(void)
{
    return (uint32_t)m_pstHwiForm;
}

/*****************************************************************************
 Function    : osHwiInit
 Description : initialization of the hardware interrupt
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
 void osHwiInit()
{
    uint32_t uwIndex;
    for(uwIndex = OS_M0_SYS_VECTOR_CNT; uwIndex < OS_M0_VECTOR_CNT; uwIndex++)
    {
        m_pstHwiForm[uwIndex] = osHwiDefaultHandler;
    }
    LosAdapIntInit();

}

/*****************************************************************************
 Function    : LOS_HwiCreate
 Description : create hardware interrupt
 Input       : uwHwiNum   --- hwi num to create
               usHwiPrio  --- priority of the hwi
               usMode     --- unused
               pfnHandler --- hwi handler
               uwArg      --- param of the hwi handler
 Output      : None
 Return      : OS_SUCCESS on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_HwiCreate( uint32_t  uwHwiNum,
                                      uint16_t   usHwiPrio,
                                      uint16_t    usMode,
                                      HWI_PROC_FUNC pfnHandler,
                                      uint32_t     uwArg )
{
    uint32_t uwIntSave;

    if (NULL == pfnHandler)
    {
        return OS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    if (uwHwiNum >= OS_M0_IRQ_VECTOR_CNT)
    {
        return OS_ERRNO_HWI_NUM_INVALID;
    }
    if (m_pstHwiForm[uwHwiNum + OS_M0_SYS_VECTOR_CNT] != osHwiDefaultHandler)
    {
        return OS_ERRNO_HWI_ALREADY_CREATED;
    }
    if (usHwiPrio > OS_HWI_PRIO_LOWEST)
    {
        return OS_ERRNO_HWI_PRIO_INVALID;
    }

    uwIntSave = LOS_IntLock();

    osSetVector(uwHwiNum, pfnHandler);

    LosAdapIrqEnable(uwHwiNum, usHwiPrio);

    LOS_IntRestore(uwIntSave);

    return LOS_OK;

}

/*****************************************************************************
 Function    : LOS_HwiDelete
 Description : Delete hardware interrupt
 Input       : uwHwiNum   --- hwi num to delete
 Output      : None
 Return      : LOS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_HwiDelete(uint32_t uwHwiNum)
{
    uint32_t uwIntSave;

    if (uwHwiNum >= OS_M0_IRQ_VECTOR_CNT)
    {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    LosAdapIrqDisable(uwHwiNum);

    uwIntSave = LOS_IntLock();

    m_pstHwiForm[uwHwiNum + OS_M0_SYS_VECTOR_CNT] = (HWI_PROC_FUNC)osHwiDefaultHandler;

    LOS_IntRestore(uwIntSave);

    return LOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


