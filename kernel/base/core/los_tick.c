//NeMOS tick relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_base.ph"
#include "los_swtmr.ph"
#include "los_task.h"
#include "los_timeslice.ph"
#include "los_demo_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


 uint64_t      g_ullTickCount;
 uint32_t      g_uwTicksPerSec;
 uint32_t      g_uwCyclePerSec;

/*****************************************************************************
 Description : Tick interruption handler
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
extern void hal_clock_irqclear(void);
 void osTickHandler(void)
{
    g_ullTickCount ++;

    #if(LOSCFG_BASE_CORE_TIMESLICE == YES)
    osTimesliceCheck();
    #endif

    osTaskScan();   //task timeout scan

    #if (LOSCFG_BASE_CORE_SWTMR == YES)
    if (osSwtmrScan() != OS_OK){
        PRINT_ERR("%s, %d\n", __FUNCTION__, __LINE__);
    }
    #endif
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
