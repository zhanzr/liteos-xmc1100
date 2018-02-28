//NeMOS event relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_event.inc"

#include "los_priqueue.ph"
#include "los_task.h"

#include "los_hw.h"
#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


 uint32_t LOS_EventInit(PEVENT_CB_S pstEventCB)
{
    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }
    pstEventCB->uwEventID = 0;
    LOS_ListInit(&pstEventCB->stEventList);
    return OS_OK;
}

 uint32_t LOS_EventPoll(uint32_t *uwEventID, uint32_t uwEventMask, uint32_t uwMode)
{
     uint32_t     uwRet = 0;
    uint32_t uwIntSave;

     uwIntSave = LOS_IntLock();
     if (uwMode & LOS_WAITMODE_OR)
     {
         if (0 != (*uwEventID & uwEventMask))
         {
            uwRet = *uwEventID & uwEventMask;
         }
     }
     else
     {
         if ((uwEventMask != 0) && (uwEventMask == (*uwEventID & uwEventMask)))
         {
            uwRet = *uwEventID & uwEventMask;
         }
     }

     if (uwRet && (LOS_WAITMODE_CLR & uwMode))
     {
        *uwEventID = *uwEventID & ~(uwRet);
     }

     LOS_IntRestore(uwIntSave);
     return uwRet;
}

 uint32_t LOS_EventRead(PEVENT_CB_S pstEventCB, uint32_t uwEventMask, uint32_t uwMode, uint32_t uwTimeOut)
{
    uint32_t      uwRet = 0;
    uint32_t uwIntSave;
    LOS_TASK_CB *pstRunTsk;
    LOS_DL_LIST *pstPendObj;

    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (uwEventMask == 0)
    {
        return LOS_ERRNO_EVENT_EVENTMASK_INVALID;
    }

    if (uwEventMask & LOS_ERRTYPE_ERROR)
    {
        return LOS_ERRNO_EVENT_SETBIT_INVALID;
    }

    if (((uwMode & LOS_WAITMODE_OR) && (uwMode & LOS_WAITMODE_AND)) ||
        uwMode & ~(LOS_WAITMODE_OR | LOS_WAITMODE_AND | LOS_WAITMODE_CLR) ||
        !(uwMode & (LOS_WAITMODE_OR | LOS_WAITMODE_AND)))
    {
        return LOS_ERRNO_EVENT_FLAGS_INVALID;
    }

    if (OS_INT_ACTIVE)
    {
        return LOS_ERRNO_EVENT_READ_IN_INTERRUPT;
    }

    uwIntSave = LOS_IntLock();
    uwRet = LOS_EventPoll(&(pstEventCB->uwEventID), uwEventMask, uwMode);

    if (uwRet == 0)
    {
        if (uwTimeOut == 0){
            (void)LOS_IntRestore(uwIntSave);
            return uwRet;
        }

        if (g_usLosTaskLock)
        {
            (void)LOS_IntRestore(uwIntSave);
            return LOS_ERRNO_EVENT_READ_IN_LOCK;
        }

        pstRunTsk = g_stLosTask.pstRunTask;
        LOS_PriqueueDequeue(&pstRunTsk->stPendList);
        pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_READY);
        pstPendObj = &pstRunTsk->stPendList;
        pstRunTsk->usTaskStatus |= OS_TASK_STATUS_PEND;
        pstRunTsk->uwEventMask = uwEventMask;
        pstRunTsk->uwEventMode = uwMode;
        LOS_ListTailInsert(&pstEventCB->stEventList,pstPendObj);
        if ((uwTimeOut != 0) && (uwTimeOut != LOS_WAIT_FOREVER))
        {
            pstRunTsk->usTaskStatus |= OS_TASK_STATUS_TIMEOUT;
            osTaskAdd2TimerList((LOS_TASK_CB *)pstRunTsk, uwTimeOut);
            (void)LOS_IntRestore(uwIntSave);
            LOS_Schedule();
        }
        else
        {
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            (void)LOS_IntRestore(uwIntSave);
            LOS_Schedule();
        }
        if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            uwIntSave = LOS_IntLock();
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            (void)LOS_IntRestore(uwIntSave);
            return LOS_ERRNO_EVENT_READ_TIMEOUT;
        }

        uwIntSave = LOS_IntLock();
        uwRet = LOS_EventPoll(&pstEventCB->uwEventID,uwEventMask,uwMode);
        (void)LOS_IntRestore(uwIntSave);
    }
    else
    {
        (void)LOS_IntRestore(uwIntSave);
    }

    return uwRet;
}

 uint32_t LOS_EventWrite(PEVENT_CB_S pstEventCB, uint32_t uwEvents)
{
    LOS_TASK_CB *pstResumedTask;
    LOS_TASK_CB *pstNextTask = (LOS_TASK_CB *)NULL;
    uint32_t uwIntSave;
    uint8_t       ucExitFlag = 0;

    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (uwEvents & LOS_ERRTYPE_ERROR)
    {
        return LOS_ERRNO_EVENT_SETBIT_INVALID;
    }

    uwIntSave = LOS_IntLock();


    pstEventCB->uwEventID |= uwEvents;
    if (!LOS_ListEmpty(&pstEventCB->stEventList))
    {
        for (pstResumedTask = LOS_DL_LIST_ENTRY((&pstEventCB->stEventList)->pstNext, LOS_TASK_CB, stPendList);/*lint !e413*/
            &pstResumedTask->stPendList != (&pstEventCB->stEventList);)
        {
            pstNextTask = LOS_DL_LIST_ENTRY(pstResumedTask->stPendList.pstNext, LOS_TASK_CB, stPendList); /*lint !e413*/

            if (((pstResumedTask->uwEventMode & LOS_WAITMODE_OR) && (pstResumedTask->uwEventMask & uwEvents) != 0) ||
                ((pstResumedTask->uwEventMode & LOS_WAITMODE_AND) && (pstResumedTask->uwEventMask & pstEventCB->uwEventID) == pstResumedTask->uwEventMask))
            {
                ucExitFlag = 1;
                LOS_ListDelete(&(pstResumedTask->stPendList));
                pstResumedTask->usTaskStatus &= (~OS_TASK_STATUS_PEND);
                if (pstResumedTask->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
                {
                    osTimerListDelete(pstResumedTask);
                    pstResumedTask->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
                }

                if (!(pstResumedTask->usTaskStatus & OS_TASK_STATUS_SUSPEND))
                {
                    pstResumedTask->usTaskStatus |= OS_TASK_STATUS_READY;
                    LOS_PriqueueEnqueue(&pstResumedTask->stPendList, pstResumedTask->usPriority);
                }
            }
            pstResumedTask = pstNextTask;
        }

        if (ucExitFlag == 1)
        {
            (void)LOS_IntRestore(uwIntSave);
            LOS_Schedule();
        }
        else
        {
            (void)LOS_IntRestore(uwIntSave);
        }
    }
    else
    {
        (void)LOS_IntRestore(uwIntSave);
    }
    return OS_OK;
}

 uint32_t LOS_EventDestory(PEVENT_CB_S pstEventCB)
{
    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    pstEventCB->stEventList.pstNext = (LOS_DL_LIST *)NULL;
    pstEventCB->stEventList.pstPrev = (LOS_DL_LIST *)NULL;
    return OS_OK;
}

 uint32_t LOS_EventClear(PEVENT_CB_S pstEventCB, uint32_t uwEvents)
{
    uint32_t uwIntSave;

    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }
    uwIntSave = LOS_IntLock();
    pstEventCB->uwEventID &= uwEvents;
    (void)LOS_IntRestore(uwIntSave);

    return OS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
