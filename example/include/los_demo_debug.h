//Need Rewrite 

#ifndef _LOS_DEMO_DEBUG_H
#define _LOS_DEMO_DEBUG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define	DEBUG_LEVEL 0

#if(DEBUG_LEVEL==1)
#define dprintf printf
#else
#define dprintf printf
#endif

#endif



