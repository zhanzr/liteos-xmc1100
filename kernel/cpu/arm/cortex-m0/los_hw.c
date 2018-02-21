//Tiny OS Hardware driver.
//This file is only for Cortex M0 core, for others cores, use conditional directive to use other drivers.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

#include "los_base.h"
#include "los_task.ph"
#include "los_hw.h"
#include "los_sys.ph"
#include "los_priqueue.ph"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*****************************************************************************
 Function    : osSchedule
 Description : task scheduling
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void osSchedule(void)
{
    osTaskSchedule();
}

/*****************************************************************************
 Function    : LOS_Schedule
 Description : Function to determine whether task scheduling is required
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_Schedule(void)
{
    uint32_t uwIntSave;
	
    uwIntSave = LOS_IntLock();

    /* Find the highest task */
    g_stLosTask.pstNewTask = LOS_DL_LIST_ENTRY(LOS_PriqueueTop(), LOS_TASK_CB, stPendList);

    /* In case that running is not highest then reschedule */
    if (g_stLosTask.pstRunTask != g_stLosTask.pstNewTask)
    {
        if ((!g_usLosTaskLock))
        {
            (void)LOS_IntRestore(uwIntSave);

            osTaskSchedule();

            return;
        }
    }

    (void)LOS_IntRestore(uwIntSave);
}

/*****************************************************************************
 Function    : osTaskExit
 Description : Task exit function
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
 void osTaskExit(void)
{
    __disable_irq();
	
    while(1);
}

/*****************************************************************************
 Function    : osTskStackInit
 Description : Task stack initialization function
 Input       : uwTaskID     --- TaskID
               uwStackSize  --- Total size of the stack
               pTopStack    --- Top of task's stack
 Output      : None
 Return      : Context pointer
 *****************************************************************************/
 void *osTskStackInit(uint32_t uwTaskID, uint32_t uwStackSize, void *pTopStack)
{
    uint32_t uwIdx;
    TSK_CONTEXT_S  *pstContext;

    /*initialize the task stack, write magic num to stack top*/
    for (uwIdx = 1; uwIdx < (uwStackSize/sizeof(uint32_t)); uwIdx++)
    {
        *((uint32_t *)pTopStack + uwIdx) = OS_TASK_STACK_INIT;
    }
    *((uint32_t *)(pTopStack)) = OS_TASK_MAGIC_WORD;

    pstContext    = (TSK_CONTEXT_S *)(((uint32_t)pTopStack + uwStackSize) - sizeof(TSK_CONTEXT_S));

    pstContext->uwR4  = 0x04040404L;
    pstContext->uwR5  = 0x05050505L;
    pstContext->uwR6  = 0x06060606L;
    pstContext->uwR7  = 0x07070707L;
    pstContext->uwR8  = 0x08080808L;
    pstContext->uwR9  = 0x09090909L;
    pstContext->uwR10 = 0x10101010L;
    pstContext->uwR11 = 0x11111111L;
    pstContext->uwPriMask = 0;
    pstContext->uwR0  = uwTaskID;
    pstContext->uwR1  = 0x01010101L;
    pstContext->uwR2  = 0x02020202L;
    pstContext->uwR3  = 0x03030303L;
    pstContext->uwR12 = 0x12121212L;
    pstContext->uwLR  = (uint32_t)osTaskExit;
    pstContext->uwPC  = (uint32_t)osTaskEntry;
    pstContext->uwxPSR = 0x01000000L;

    return (void *)pstContext;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


