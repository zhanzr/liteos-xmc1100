
#include "los_sys.h"
#include "los_tick.h"
#include "los_task.h"
#include "los_config.h"

#include "los_demo_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma pack(8)

uint8_t *m_aucSysMem0;
uint32_t g_sys_mem_addr_end = 0;
	
volatile bool g_osTickStarted;
	
extern uint8_t g_ucMemStart[];
extern uint32_t osTickInit(uint32_t uwSystemClock, uint32_t uwTickPerSecond);
extern uint32_t   g_uwTskMaxNum;

extern const unsigned char g_use_ram_vect;
	
/*****************************************************************************
 Function    : osRegister
 Description : Configuring the maximum number of tasks
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
 void osRegister(void)
{
    g_uwTskMaxNum = LOSCFG_BASE_CORE_TSK_LIMIT + 1; /* Reserved 1 for IDLE */
    g_sys_mem_addr_end = (uint32_t)g_ucMemStart + OS_SYS_MEM_SIZE;
    return;
}

/*****************************************************************************
 Function    : LOS_EnableTick
 Description : enable system  start function
 Input       : None
 Output      : None
 Return      : OS_OK
 *****************************************************************************/
 uint32_t LOS_EnableTick(void)
{
    uint32_t uwRet;

    uwRet = osTickStart();

    if (uwRet != OS_OK)
    {
        PRINT_ERR("osTickStart error\n");
    }
		
		g_osTickStarted = true;
    return uwRet;
}
/*****************************************************************************
 Function    : LOS_Start
 Description : Task start function
 Input       : None
 Output      : None
 Return      : OS_OK
 *****************************************************************************/
 uint32_t LOS_Start(void)
{
    uint32_t uwRet = OS_OK;

    LOS_StartToRun();

    return uwRet;
}

/*****************************************************************************
 Function    : osMain
 Description : System kernel initialization function, configure all system modules
 Input       : None
 Output      : None
 Return      : OS_OK
 *****************************************************************************/
 int osMain(void)
{
    uint32_t uwRet;

    osRegister();

    uwRet = osMemSystemInit();
    if (uwRet != OS_OK)
    {
        PRINT_ERR("osMemSystemInit error %d\n", uwRet);
        return uwRet;
    }

#if (LOSCFG_PLATFORM_HWI == YES)
    {
        if (g_use_ram_vect)
        {
            osHwiInit();
        }
    }
#endif

    uwRet =osTaskInit();
    if (uwRet != OS_OK)
    {
        PRINT_ERR("osTaskInit error\n");
        return uwRet;
    }

#if (LOSCFG_BASE_IPC_SEM == YES)
    {
        uwRet = osSemInit();
        if (uwRet != OS_OK)
        {
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_IPC_MUX == YES)
    {
        uwRet = osMuxInit();
        if (uwRet != OS_OK)
        {
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_IPC_QUEUE == YES)
    {
        uwRet = osQueueInit();
        if (uwRet != OS_OK)
        {
            PRINT_ERR("osQueueInit error\n");
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_CORE_SWTMR == YES)
    {
        uwRet = osSwTmrInit();
        if (uwRet != OS_OK)
        {
            PRINT_ERR("osSwTmrInit error\n");
            return uwRet;
        }
    }
#endif

    #if(LOSCFG_BASE_CORE_TIMESLICE == YES)
    osTimesliceInit();
    #endif

    uwRet = osIdleTaskCreate();
    if (uwRet != OS_OK) {
        return uwRet;
    }

    return OS_OK;
}

 int LOS_KernelInit(void)
{
    uint32_t uwRet;
    uwRet = osMain();
    if (uwRet != OS_OK) {
        return OS_NOK;
    }
    return OS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
