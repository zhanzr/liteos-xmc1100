/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_event.inc"

#include "los_priqueue.ph"
#include "los_task.ph"

#include "los_hw.h"
#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


LITE_OS_SEC_TEXT_INIT uint32_t LOS_EventInit(PEVENT_CB_S pstEventCB)
{
    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }
    pstEventCB->uwEventID = 0;
    LOS_ListInit(&pstEventCB->stEventList);
    return LOS_OK;
}

LITE_OS_SEC_TEXT uint32_t LOS_EventPoll(uint32_t *uwEventID, uint32_t uwEventMask, uint32_t uwMode)
{
     uint32_t     uwRet = 0;
     uint32_t*    uvIntSave;

     uvIntSave = LOS_IntLock();
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

     LOS_IntRestore(uvIntSave);
     return uwRet;
}

LITE_OS_SEC_TEXT uint32_t LOS_EventRead(PEVENT_CB_S pstEventCB, uint32_t uwEventMask, uint32_t uwMode, uint32_t uwTimeOut)
{
    uint32_t      uwRet = 0;
    uint32_t*     uvIntSave;
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

    uvIntSave = LOS_IntLock();
    uwRet = LOS_EventPoll(&(pstEventCB->uwEventID), uwEventMask, uwMode);

    if (uwRet == 0)
    {
        if (uwTimeOut == 0){
            (void)LOS_IntRestore(uvIntSave);
            return uwRet;
        }

        if (g_usLosTaskLock)
        {
            (void)LOS_IntRestore(uvIntSave);
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
            (void)LOS_IntRestore(uvIntSave);
            LOS_Schedule();
        }
        else
        {
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            (void)LOS_IntRestore(uvIntSave);
            LOS_Schedule();
        }
        if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            uvIntSave = LOS_IntLock();
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            (void)LOS_IntRestore(uvIntSave);
            return LOS_ERRNO_EVENT_READ_TIMEOUT;
        }

        uvIntSave = LOS_IntLock();
        uwRet = LOS_EventPoll(&pstEventCB->uwEventID,uwEventMask,uwMode);
        (void)LOS_IntRestore(uvIntSave);
    }
    else
    {
        (void)LOS_IntRestore(uvIntSave);
    }

    return uwRet;
}

LITE_OS_SEC_TEXT uint32_t LOS_EventWrite(PEVENT_CB_S pstEventCB, uint32_t uwEvents)
{
    LOS_TASK_CB *pstResumedTask;
    LOS_TASK_CB *pstNextTask = (LOS_TASK_CB *)NULL;
    uint32_t*     uvIntSave;
    uint8_t       ucExitFlag = 0;

    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (uwEvents & LOS_ERRTYPE_ERROR)
    {
        return LOS_ERRNO_EVENT_SETBIT_INVALID;
    }

    uvIntSave = LOS_IntLock();


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
            (void)LOS_IntRestore(uvIntSave);
            LOS_Schedule();
        }
        else
        {
            (void)LOS_IntRestore(uvIntSave);
        }
    }
    else
    {
        (void)LOS_IntRestore(uvIntSave);
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT uint32_t LOS_EventDestory(PEVENT_CB_S pstEventCB)
{
    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    pstEventCB->stEventList.pstNext = (LOS_DL_LIST *)NULL;
    pstEventCB->stEventList.pstPrev = (LOS_DL_LIST *)NULL;
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR uint32_t LOS_EventClear(PEVENT_CB_S pstEventCB, uint32_t uwEvents)
{
    uint32_t* uvIntSave;

    if (pstEventCB == NULL)
    {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }
    uvIntSave = LOS_IntLock();
    pstEventCB->uwEventID &= uwEvents;
    (void)LOS_IntRestore(uvIntSave);

    return LOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
