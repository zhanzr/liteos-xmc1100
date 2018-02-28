//TODO:Need Rewrite

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h> 
	
#include "los_demo_entry.h"
#include "los_task.h"

static uint32_t g_uwDemoTaskID;
    
static  void LOS_Demo_Tskfunc(void)
{
#ifdef LOS_KERNEL_TEST_ALL
#else /* LOS_KERNEL_TEST_ALL */

/* only test some function */
#ifdef LOS_KERNEL_TEST_TASK
    Example_TskCaseEntry();
#endif
#ifdef LOS_KERNEL_TEST_MEM_DYNAMIC
     Example_Dyn_Mem();
#endif
#ifdef LOS_KERNEL_TEST_MEM_STATIC
    Example_StaticMem();
#endif
#ifdef LOS_KERNEL_TEST_INTRRUPT
    Example_Interrupt();
#endif
#ifdef LOS_KERNEL_TEST_QUEUE
    Example_MsgQueue();
#endif
#ifdef LOS_KERNEL_TEST_EVENT
    Example_SndRcvEvent();
#endif 
#ifdef LOS_KERNEL_TEST_MUTEX
    Example_MutexLock();
#endif
#ifdef LOS_KERNEL_TEST_SEMPHORE
    Example_Semphore();
#endif
#ifdef LOS_KERNEL_TEST_SYSTICK
    Example_GetTick();
#endif
#ifdef LOS_KERNEL_TEST_SWTIMER
    Example_swTimer();
#endif
#ifdef LOS_KERNEL_TEST_LIST
    Example_list();
#endif
#endif/* LOS_KERNEL_TEST_ALL */

    while (1)
    {
        (void)LOS_TaskDelay(100);
    }
}

void LOS_Demo_Entry(void)
{
    uint32_t uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_Demo_Tskfunc;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "ApiDemo";
    stTaskInitParam.usTaskPrio = 30;
    uwRet = LOS_TaskCreate(&g_uwDemoTaskID, &stTaskInitParam);

    if (uwRet != OS_OK)
    {
        dprintf("Api demo test task create failed\n");
        return;
    }
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

