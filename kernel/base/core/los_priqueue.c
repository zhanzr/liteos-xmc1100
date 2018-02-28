//NeMOS Priority queue relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_priqueue.inc"

#include "los_base.ph"
#include "los_task.h"

#include "los_memory.h"

LOS_DL_LIST *g_pstLosPriorityQueueList;

void osPriqueueInit(void)
{
    uint32_t uwPri = 0;
    uint32_t uwSize = 0;

    uwSize = LOS_PRIORITY_QUEUE_PRIORITYNUM * sizeof(LOS_DL_LIST);
    g_pstLosPriorityQueueList = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, uwSize);
    if (NULL == g_pstLosPriorityQueueList)
    {
        return;
    }

    for (uwPri = 0; uwPri < LOS_PRIORITY_QUEUE_PRIORITYNUM; ++uwPri)
    {
        LOS_ListInit(&g_pstLosPriorityQueueList[uwPri]);
    }
}

void LOS_PriqueueEnqueue(LOS_DL_LIST *ptrPQItem, uint32_t uwPri)
{
    LOS_ListTailInsert(&g_pstLosPriorityQueueList[uwPri], ptrPQItem);
}

void LOS_PriqueueDequeue(LOS_DL_LIST *ptrPQItem)
{
    LOS_ListDelete(ptrPQItem);
}

LOS_DL_LIST *LOS_PriqueueTop(void)
{
    uint32_t uwPri = 0;

    for (uwPri = 0; uwPri < LOS_PRIORITY_QUEUE_PRIORITYNUM; ++uwPri)
    {
        if (!LOS_ListEmpty(&g_pstLosPriorityQueueList[uwPri]))
        {
            return LOS_DL_LIST_FIRST(&g_pstLosPriorityQueueList[uwPri]);
        }
    }

    return (LOS_DL_LIST *)NULL;
}

uint32_t LOS_PriqueueSize(uint32_t uwPri)
{
    uint32_t      uwItemCnt = 0;
    LOS_DL_LIST *pstCurPQNode = (LOS_DL_LIST *)NULL;

    LOS_DL_LIST_FOR_EACH(pstCurPQNode, &g_pstLosPriorityQueueList[uwPri])
    {
        ++uwItemCnt;
    }

    return uwItemCnt;
}

uint32_t LOS_PriqueueTotalSize(void)
{
    uint32_t uwPri = 0;
    uint32_t uwTotalSize = 0;

    for (uwPri = 0; uwPri < LOS_PRIORITY_QUEUE_PRIORITYNUM; ++uwPri)
    {
        uwTotalSize += LOS_PriqueueSize(uwPri);
    }

    return uwTotalSize;
}
