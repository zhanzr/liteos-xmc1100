//NeMOS tick / time relevant definition.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

/**@defgroup los_sys System time
 * @ingroup kernel
 */

#ifndef _LOS_SYS_H
#define _LOS_SYS_H

#include "los_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern uint32_t SystemCoreClock;

/**
 *@ingroup los_sys
 *System time basic function error code: Null pointer.
 *
 *Value: 0x02000010
 *
 *Solution: Check whether the input parameter is null.
 */
#define LOS_ERRNO_SYS_PTR_NULL                                  LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x10)

/**
 *@ingroup los_sys
 *System time basic function error code: Invalid system clock configuration.
 *
 *Value: 0x02000011
 *
 *Solution: Configure a valid system clock in los_config.h.
 */
#define LOS_ERRNO_SYS_CLOCK_INVALID                             LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x11)

/**
 *@ingroup los_sys
 *System time basic function error code: This error code is not in use temporarily.
 *
 *Value: 0x02000012
 *
 *Solution: None.
 */
#define LOS_ERRNO_SYS_MAXNUMOFCORES_IS_INVALID                  LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x12)

/**
 *@ingroup los_sys
 *System time error code: This error code is not in use temporarily.
 *
 *Value: 0x02000013
 *
 *Solution: None.
 */
#define LOS_ERRNO_SYS_PERIERRCOREID_IS_INVALID                  LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x13)

/**
 *@ingroup los_sys
 *System time error code: This error code is not in use temporarily.
 *
 *Value: 0x02000014
 *
 *Solution: None.
 */
#define LOS_ERRNO_SYS_HOOK_IS_FULL                              LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x14)

/**
 *@ingroup los_sys
 *@brief Obtain the number of Ticks.
 *
 *@par Description:
 *This API is used to obtain the number of Ticks.
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param  None.
 *
 *@retval The number of Ticks is successfully obtained.
 *@par Dependency:
 *<ul><li>los_sys.h: the header file that contains the API declaration.</li></ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern uint64_t LOS_TickCountGet (void);

/**
 *@ingroup los_sys
 *@brief Obtain the number of cycles in one second.
 *
 *@par Description:
 *This API is used to obtain the number of cycles in one second.
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param  None.
 *
 *@retval Number of cycles obtained through the conversion. The number of cycles in one second is successfully obtained.
 *@par Dependency:
 *<ul><li>los_sys.h: the header file that contains the API declaration.</li></ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_CyclePerTickGet(void);

/**
 *@ingroup los_sys
 *@brief Convert Ticks to milliseconds.
 *
 *@par Description:
 *This API is used to convert Ticks to milliseconds.
 *@attention
 *<ul>
 *<li>The number of milliseconds obtained through the conversion is 32-bit.</li>
 *</ul>
 *
 *@param  tick  [IN] Number of Ticks. The value range is (0,OS_SYS_CLOCK).
 *
 *@retval Number of milliseconds obtained through the conversion. Ticks are successfully converted to milliseconds.
 *@par  Dependency:
 *<ul><li>los_sys.h: the header file that contains the API declaration.</li></ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_Tick2MS(uint32_t uwTick);

/**
 *@ingroup los_sys
 *@brief Convert milliseconds to Ticks.
 *
 *@par Description:
 *This API is used to convert milliseconds to Ticks.
 *@attention
 *<ul>
 *<li>Pay attention to the value to be converted because data possibly overflows.</li>
 *</ul>
 *
 *@param  millisec  [IN] Number of milliseconds.
 *
 *@retval Number of Ticks obtained through the conversion. Milliseconds are successfully converted to Ticks.
 *@par Dependency:
 *<ul><li>los_sys.h: the header file that contains the API declaration.</li></ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_MS2Tick(uint32_t uwMillisec);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_SYS_H */
