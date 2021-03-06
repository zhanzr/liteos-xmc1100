/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
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
        dprintf("osTickStart error\n");
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
        dprintf("osMemSystemInit error %d\n", uwRet);
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
        dprintf("osTaskInit error\n");
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
            dprintf("osQueueInit error\n");
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_CORE_SWTMR == YES)
    {
        uwRet = osSwTmrInit();
        if (uwRet != OS_OK)
        {
            dprintf("osSwTmrInit error\n");
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
