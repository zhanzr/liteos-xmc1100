//NeMOS memory statistic relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018


#include "los_memstat.inc"

#include "los_task.h"

#include "los_config.h"
#include "los_hwi.h"

typedef struct {
    uint32_t uwMemUsed;
} TSK_MEM_USED_INFO;

 TSK_MEM_USED_INFO g_TskMemUsedInfo[LOSCFG_BASE_CORE_TSK_LIMIT + 1];

 void osTaskMemUsedInc(uint32_t uwUsedSize)
{
    uint32_t taskId;

    if (NULL == g_stLosTask.pstRunTask)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    taskId = (uint32_t) g_stLosTask.pstRunTask->uwTaskID;

    if (taskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return;
    }

    g_TskMemUsedInfo[taskId].uwMemUsed += uwUsedSize;
}

 void osTaskMemUsedDec(uint32_t uwUsedSize)
{
    uint32_t taskId;

    taskId = (uint32_t) g_stLosTask.pstRunTask->uwTaskID;

    if (taskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    g_TskMemUsedInfo[taskId].uwMemUsed -= uwUsedSize;
}

 uint32_t osTaskMemUsage(uint32_t uwTaskId)
{
    if ((uint32_t)uwTaskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return OS_NOK;
    }

    return g_TskMemUsedInfo[(uint32_t)uwTaskId].uwMemUsed;
}
