/**@defgroup los_typedef Type define
 * @ingroup kernel
*/

#ifndef _LOS_TYPEDEF_H
#define _LOS_TYPEDEF_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define YES                                                 (1)
#define NO                                                  (0)

#ifndef OS_OK
#define OS_OK                                              (0)
#endif

#ifndef OS_NOK
#define OS_NOK                                             (1)
#endif

#define OS_ERROR                                            (uint32_t)(-1)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_TYPEDEF_H */
