//NeMOS time slice source.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_task.h"
#include "los_tick.h"
#include "los_typedef.ph"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if(LOSCFG_BASE_CORE_TIMESLICE == YES)
 OS_TASK_ROBIN_S        g_stTaskTimeSlice;

/*****************************************************************************
 Function     : osTimesliceInit
 Description  : Initialztion Timeslice
 Input        : None
 Output       : None
 Return       : None
 *****************************************************************************/
 void osTimesliceInit(void)
{
    g_stTaskTimeSlice.pstTask = (LOS_TASK_CB *)NULL;
    g_stTaskTimeSlice.usTout = LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT;
}

/*****************************************************************************
 Function     : osTimesliceCheck
 Description  : check Timeslice
 Input        : None
 Output       : None
 Return       : None
 *****************************************************************************/
extern uint64_t g_ullTickCount;
 void osTimesliceCheck(void)
{
    if (g_stTaskTimeSlice.pstTask != g_stLosTask.pstRunTask)
    {
        g_stTaskTimeSlice.pstTask = g_stLosTask.pstRunTask;
        g_stTaskTimeSlice.usTime = (uint16_t)g_ullTickCount + g_stTaskTimeSlice.usTout - 1;
    }

    if (g_stTaskTimeSlice.usTime == (uint16_t)g_ullTickCount)
    {
        g_stTaskTimeSlice.pstTask = (LOS_TASK_CB *)NULL;
        if (LOS_TaskYield() != OS_OK)
        {
					;//TODO: handle this error
        }
    } /*lint !e548*/
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

