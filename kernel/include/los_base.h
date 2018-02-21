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

/**@defgroup los_base Basic definitions
 * @ingroup kernel
 */

#ifndef _LOS_BASE_H
#define _LOS_BASE_H

#include "los_typedef.h"
#include "los_config.h"
#include "los_printf.h"
#include "los_list.h"
#include "los_errno.h"
#ifdef LOSCFG_LIB_LIBCMINI
#include "libcmini.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define LOS_ASSERT_COND(expression)

/**
 * @ingroup los_base
 * Define the timeout interval as LOS_NO_WAIT.
 */
#define LOS_NO_WAIT                                 0

/**
 * @ingroup los_base
 * Define the timeout interval as LOS_WAIT_FOREVER.
 */
#define LOS_WAIT_FOREVER                            0xFFFFFFFF

/**
 * @ingroup los_base
 * Align the beginning of the object with the base address uwAddr, with uwBoundary bytes being the smallest unit of alignment.
 */
#ifndef ALIGN
#define ALIGN(uwAddr, uwBoundary)                   LOS_Align(uwAddr, uwBoundary)
#endif
/**
 * @ingroup los_base
 * Align the tail of the object with the base address uwAddr, with uwSize bytes being the smallest unit of alignment.
 */
#define TRUNCATE(uwAddr, uwSize)                    ((uwAddr) & ~((uwSize) - 1))

/**
 * @ingroup los_base
 * Read a uint8_t value from ucAddr and stroed in ucValue.
 */
#define READ_UINT8(ucValue, ucAddr)                   ((ucValue) = *((volatile uint8_t *)(ucAddr)))
/**
 * @ingroup los_base
 * Read a uint16_t value from usAddr and stroed in usAddr.
 */
#define READ_UINT16(usValue, usAddr)                  ((usValue) = *((volatile uint16_t *)(usAddr)))
/**
 * @ingroup los_base
 * Read a uint32_t value from uwAddr and stroed in uwValue.
 */
#define READ_UINT32(uwValue, uwAddr)                  ((uwValue) = *((volatile uint32_t *)(uwAddr)))
/**
 * @ingroup los_base
 * Read a uint64_t value from ullAddr and stroed in ullValue.
 */
#define READ_UINT64(ullValue, ullAddr)                ((ullValue) = *((volatile uint64_t *)(ullAddr)))


/**
 * @ingroup los_base
 * Get a uint8_t value from ucAddr.
 */
#define GET_UINT8(ucAddr)                            (*((volatile uint8_t *)(ucAddr)))
/**
 * @ingroup los_base
 * Get a uint16_t value from usAddr.
 */
#define GET_UINT16(usAddr)                           (*((volatile uint16_t *)(usAddr)))
/**
 * @ingroup los_base
 * Get a uint32_t value from uwAddr.
 */
#define GET_UINT32(uwAddr)                           (*((volatile uint32_t *)(uwAddr)))
/**
 * @ingroup los_base
 * Get a uint64_t value from ullAddr.
 */
#define GET_UINT64(ullAddr)                          (*((volatile uint64_t *)(ullAddr)))

/**
 * @ingroup los_base
 * Write a uint8_t ucValue to ucAddr.
 */
#define WRITE_UINT8(ucValue, ucAddr)                 (*((volatile uint8_t *)(ucAddr)) = (ucValue))
/**
 * @ingroup los_base
 * Write a uint16_t usValue to usAddr.
 */
#define WRITE_UINT16(usValue, usAddr)                (*((volatile uint16_t *)(usAddr)) = (usValue))
/**
 * @ingroup los_base
 * Write a uint32_t uwValue to uwAddr.
 */
#define WRITE_UINT32(uwValue, uwAddr)                (*((volatile uint32_t *)(uwAddr)) = (uwValue))
/**
 * @ingroup los_base
 * Write a uint64_t ullAddr to ullAddr.
 */
#define WRITE_UINT64(ullValue, ullAddr)              (*((volatile uint64_t *)(ullAddr)) = (ullValue))

/**
 *@ingroup los_base
 *@brief Align the value (uwAddr) by some bytes (uwBoundary) you specify.
 *
 * @par Description:
 * This API is used to align the value (uwAddr) by some bytes (uwBoundary) you specify.
 *
 * @attention
 * <ul>
 * <li>the value of uwBoundary usually is 4,8,16,32.</li>
 * </ul>
 *
 *@param uwAddr     [IN]  The variable what you want to align.
 *@param uwBoundary [IN]  The align size what you want to align.
 *
 *@retval The variable what have been aligned.
 *@par Dependency:
 *<ul><li>los_base.h: the header file that contains the API declaration.</li></ul>
 *@see
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_Align(uint32_t uwAddr, uint32_t uwBoundary);

/**
 *@ingroup los_base
 *@brief Sleep the current task.
 *
 * @par Description:
 * This API is used to delay the execution of the current task. The task is able to be scheduled after it is delayed for a specified number of Ticks.
 *
 * @attention
 * <ul>
 * <li>The task fails to be delayed if it is being delayed during interrupt processing or it is locked.</li>
 * <li>If 0 is passed in and the task scheduling is not locked, execute the next task in the queue of tasks with the priority of the current task.
 * If no ready task with the priority of the current task is available, the task scheduling will not occur, and the current task continues to be executed.</li>
 * </ul>

 *
 *@param uwMsecs [IN] Type #uint32_t Number of MS for which the task is delayed.
 *
 *@retval None.
 *@par Dependency:
 *<ul><li>los_base.h: the header file that contains the API declaration.</li></ul>
 *@see
 *@since Huawei LiteOS V100R001C00
 */
extern void LOS_Msleep(uint32_t uwMsecs);

/**
 * @ingroup los_base
 * Dynamic memory pool address, of which the size is defined by OS_SYS_MEM_SIZE in los_config.h.
 */
extern uint8_t *m_aucSysMem0;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_BASE_H */
