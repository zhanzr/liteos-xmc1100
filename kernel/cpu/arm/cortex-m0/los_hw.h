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

 /**@defgroup los_hw hardware
   *@ingroup kernel
 */

#ifndef _LOS_HW_H
#define _LOS_HW_H

#include "los_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup los_hw
 * The initialization value of stack space.
 */
#define EMPTY_STACK                 0xCACA

/**
 * @ingroup los_hw
 * Trigger a task.
 */
#define osTaskTrap()                asm("   TRAP    #31")

/**
 * @ingroup los_hw
 * Check task schedule.
 */
#define LOS_CHECK_SCHEDULE          ((!g_usLosTaskLock))

/**
 * @ingroup los_hw
 * Define the type of a task context control block.
 */
typedef struct tagTskContext
{
    uint32_t uwR4;
    uint32_t uwR5;
    uint32_t uwR6;
    uint32_t uwR7;
    uint32_t uwR8;
    uint32_t uwR9;
    uint32_t uwR10;
    uint32_t uwR11;
    uint32_t uwPriMask;
    uint32_t uwR0;
    uint32_t uwR1;
    uint32_t uwR2;
    uint32_t uwR3;
    uint32_t uwR12;
    uint32_t uwLR;
    uint32_t uwPC;
    uint32_t uwxPSR;
} TSK_CONTEXT_S;



/**
 * @ingroup  los_hw
 * @brief: Task stack initialization.
 *
 * @par Description:
 * This API is used to initialize the task stack.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  uwTaskID     [IN] Type#uint32_t: TaskID.
 * @param  uwStackSize  [IN] Type#uint32_t: Total size of the stack.
 * @param  pTopStack    [IN] Type#void *: Top of task's stack.
 *
 * @retval: pstContext Type#TSK_CONTEXT_S *.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void * osTskStackInit(uint32_t uwTaskID, uint32_t uwStackSize, void *pTopStack);



/**
 * @ingroup  los_hw
 * @brief: Task scheduling Function.
 *
 * @par Description:
 * This API is used to scheduling task.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void osSchedule(void);


/**
 * @ingroup  los_hw
 * @brief: Function to determine whether task scheduling is required.
 *
 * @par Description:
 * This API is used to Judge and entry task scheduling.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void LOS_Schedule(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif /* _LOS_HW_H */

