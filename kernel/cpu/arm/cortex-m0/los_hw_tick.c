//NeMOS Hardware driver.
//This file is only for Cortex M0 core, for others cores, use conditional directive to use other drivers.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

#include "los_tick.h"

#include "los_base.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

uint32_t  g_uwCyclesPerTick;

/*lint -save -e40 -e10 -e26 -e1013*/
/*****************************************************************************
Function   : LOS_TickHandler
Description: los system tick handler 
Input   : none
output  : none
return  : none
*****************************************************************************/
void LOS_TickHandler(void)
{
    uint32_t uwIntSave;

    uwIntSave = LOS_IntLock();
    g_vuwIntCount++;
    LOS_IntRestore(uwIntSave);

    osTickHandler();
    
    uwIntSave = LOS_IntLock();
    g_vuwIntCount--;
    LOS_IntRestore(uwIntSave);
}

/*****************************************************************************
Function   : LOS_SetTickSycle
Description: set g_uwCyclesPerTick value
Input   : ticks, the cpu Sycles per tick
output  : none
return  : none
*****************************************************************************/
void LOS_SetTickSycle(uint32_t ticks)
{
    g_uwCyclesPerTick = ticks;
    return ;
}

/*****************************************************************************
Function   : LOS_GetCpuCycle
Description: Get System cycle count
Input   : none
output  : puwCntHi  --- CpuTick High 4 byte
          puwCntLo  --- CpuTick Low 4 byte
return  : none
*****************************************************************************/
 void LOS_GetCpuCycle(uint32_t *puwCntHi, uint32_t *puwCntLo)
{
    uint64_t ullSwTick;
    uint64_t ullCycle;
    uint32_t uwIntSta;
    uint32_t uwHwCycle;
    uint32_t uwIntSave;

    uwIntSave = LOS_IntLock();

    ullSwTick = g_ullTickCount;

    uwHwCycle = *(volatile uint32_t*)OS_SYSTICK_CURRENT_REG;
    uwIntSta  = *(volatile uint32_t*)OS_NVIC_INT_CTRL;

    /*tick has come, but may interrupt environment, not counting the Tick interrupt response, to do +1 */
    if (((uwIntSta & 0x4000000) != 0))
    {
        uwHwCycle = *(volatile uint32_t*)OS_SYSTICK_CURRENT_REG;
        ullSwTick++;
    }

    ullCycle = (((ullSwTick) * g_uwCyclesPerTick) + (g_uwCyclesPerTick - uwHwCycle));
    *puwCntHi = ullCycle >> 32;
    *puwCntLo = ullCycle & 0xFFFFFFFFU;

    LOS_IntRestore(uwIntSave);

    return;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
