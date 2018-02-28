//NeMOS miscellaneous function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

//TODO: Remove this obsolete implementation.

#include "los_base.ph"
#include "los_task.h"

#include "los_hwi.h"

 uint32_t LOS_Align(uint32_t uwAddr, uint32_t uwBoundary)
{
    if (uwAddr + uwBoundary - 1 > uwAddr) {
        return (uwAddr + uwBoundary - 1) & ~(uwBoundary - 1);
    } else {
        return uwAddr & ~(uwBoundary - 1);
    }
}

 void LOS_Msleep(uint32_t uwMsecs)
{
    uint32_t uwInterval = 0;

    if (OS_INT_ACTIVE) {
        return;
    }

    if (uwMsecs == 0) {
        uwInterval = 0;
    } else {
        uwInterval = LOS_MS2Tick(uwMsecs);
        if (uwInterval == 0) {
             uwInterval = 1;
        }
    }

    (void)LOS_TaskDelay(uwInterval);
}
