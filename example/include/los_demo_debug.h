//Need Rewrite 

#ifndef _LOS_DEMO_DEBUG_H
#define _LOS_DEMO_DEBUG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "los_config.h"

#define VERBOSE_QUIET_LEVEL   0
#define VERBOSE_COMMOM_LEVEL   (VERBOSE_QUIET_LEVEL + 1)
#define VERBOSE_ERR_LEVEL   (VERBOSE_COMMOM_LEVEL + 1)
#define VERBOSE_WARN_LEVEL  (VERBOSE_ERR_LEVEL + 1)
#define VERBOSE_INFO_LEVEL  (VERBOSE_WARN_LEVEL + 1)
#define VERBOSE_DEBUG_LEVEL (VERBOSE_INFO_LEVEL + 1)

#define dprintf printf

#if VERBOSE_LEVEL < VERBOSE_DEBUG_LEVEL
#define PRINT_DEBUG(fmt, args...)
#else
#define PRINT_DEBUG(fmt, args...)   do{(dprintf("[DEBUG] "), dprintf(fmt, ##args));}while(0)
#endif

#if VERBOSE_LEVEL < VERBOSE_INFO_LEVEL
#define PRINT_INFO(fmt, args...)
#else
#define PRINT_INFO(fmt, args...)    do{(dprintf("[INFO] "), dprintf(fmt, ##args));}while(0)
#endif

#if VERBOSE_LEVEL < VERBOSE_WARN_LEVEL
#define PRINT_WARN(fmt, args...)
#else
#define PRINT_WARN(fmt, args...)    do{(dprintf("[WARN] "), dprintf(fmt, ##args));}while(0)
#endif

#if VERBOSE_LEVEL < VERBOSE_ERR_LEVEL
#define PRINT_ERR(fmt, args...)
#else
#define PRINT_ERR(fmt, args...)     do{(dprintf("[ERR] "), dprintf(fmt, ##args));}while(0)
#endif

#endif



