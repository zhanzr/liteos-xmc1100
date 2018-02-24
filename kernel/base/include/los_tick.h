#ifndef _LOS_TICK_PH
#define _LOS_TICK_PH

#include <stdint.h>

#include "los_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/**
 * @ingroup los_tick
 * Count of Ticks
 */
extern uint64_t g_ullTickCount;

/**
* @ingroup  los_tick
* @brief Handle the system tick timeout.
*
* @par Description:
* This API is called when the system tick timeout and triggers the interrupt.
*
* @attention
* <ul>
* <li>None.</li>
* </ul>
*
* @param none.
*
* @retval None.
* @par Dependency:
* <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
* @see None.
* @since Huawei LiteOS V100R001C00
*/
extern void osTickHandler(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_TICK_PH */
