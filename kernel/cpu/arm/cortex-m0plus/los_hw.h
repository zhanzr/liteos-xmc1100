//Tiny OS Hardware driver.
//This file is only for Cortex M0 core, for others cores, use conditional directive to use other drivers.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

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

