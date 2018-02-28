//NeMOS tick / time relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_tick.h"

#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*****************************************************************************
Function   : LOS_TickCountGet
Description: get current tick
Input      : None
Output     : None
Return     : current tick
*****************************************************************************/
extern uint64_t g_ullTickCount;
 uint64_t LOS_TickCountGet (void)
{
    return g_ullTickCount;
}

/*****************************************************************************
Function   : LOS_CyclePerTickGet
Description: Get System cycle number corresponding to each tick
Input      : None
Output     : None
Return     : cycle number corresponding to each tick
*****************************************************************************/
 uint32_t LOS_CyclePerTickGet(void)
{
    return SystemCoreClock / LOSCFG_BASE_CORE_TICK_PER_SECOND;/*lint !e160*/
}

/*****************************************************************************
Function   : LOS_MS2Tick
Description: milliseconds convert to Tick
Input      : milliseconds
Output     : None
Return     : Tick
*****************************************************************************/
uint32_t LOS_MS2Tick(uint32_t uwMillisec)
{
    if (0xFFFFFFFF == uwMillisec)
    {
        return 0xFFFFFFFF;
    }

    return ((uint64_t)uwMillisec * LOSCFG_BASE_CORE_TICK_PER_SECOND) / 1000;
}

/*****************************************************************************
Function   : LOS_Tick2MS
Description: Tick convert to milliseconds
Input      : TICK
Output     : None
Return     : milliseconds
*****************************************************************************/
 uint32_t LOS_Tick2MS(uint32_t uwTick)
{
    return ((uint64_t)uwTick * 1000) / LOSCFG_BASE_CORE_TICK_PER_SECOND;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
