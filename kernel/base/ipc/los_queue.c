//NeMOS queue relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_queue.inc"
#include "los_membox.ph"
#include "los_memory.ph"
#include "los_priqueue.ph"
#include "los_task.h"
#include "los_hwi.h"
#include "los_hw.h"

#include <string.h>
#include <stdint.h>


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#if (LOSCFG_BASE_IPC_QUEUE == YES)

/*lint -save -e64*/
      QUEUE_CB_S       *g_pstAllQueue;

/**************************************************************************
 Function    : osQueueInit
 Description : queue initial
 Input       : usMaxQueue  --- Maximum queue count
 Output      : None
 Return      : OS_OK on success or error code on failure
**************************************************************************/
 uint32_t osQueueInit()
{

    if (0 == LOSCFG_BASE_IPC_QUEUE_LIMIT)   /*lint !e506*/
    {
        return LOS_ERRNO_QUEUE_MAXNUM_ZERO;
    }

    g_pstAllQueue = (QUEUE_CB_S *)LOS_MemAlloc(m_aucSysMem0, LOSCFG_BASE_IPC_QUEUE_LIMIT * sizeof(QUEUE_CB_S));
    if (NULL == g_pstAllQueue)
    {
        return LOS_ERRNO_QUEUE_NO_MEMORY;
    }

    memset(g_pstAllQueue, 0, LOSCFG_BASE_IPC_QUEUE_LIMIT * sizeof(QUEUE_CB_S));

    return OS_OK;
}

/**************************************************************************
 Function    : osQueueCreate
 Description :  Create a queue
 Input       : usLen           --- Queue length
               puwQueueID      --- Queue ID
               usMaxMsgSize    --- Maximum message size in byte
 Output      : ppstQueueCBOut
 Return      : OS_OK on success or error code on failure
**************************************************************************/
 uint32_t osQueueCreate(uint16_t usLen,
                                      uint32_t *puwQueueID,
                                      uint16_t usMaxMsgSize,
                                      QUEUE_CB_S **ppstQueueCBOut)
{
    uint32_t      uwIndex;
    QUEUE_CB_S    *pstQueueCB;

    /* get a unused queue */
    pstQueueCB = g_pstAllQueue;
    for (uwIndex = 0; uwIndex < LOSCFG_BASE_IPC_QUEUE_LIMIT; uwIndex++, pstQueueCB++)
    {
        if (OS_QUEUE_UNUSED == pstQueueCB->usQueueState)
        {
            *puwQueueID = uwIndex + 1;
            break;
        }
    }

    if (uwIndex == LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return LOS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }

    (void)memset(pstQueueCB, 0, sizeof(QUEUE_CB_S));
    pstQueueCB->pucQueue = (uint8_t *)LOS_MemAlloc(m_aucSysMem0, usLen * usMaxMsgSize);
    if (NULL == pstQueueCB->pucQueue)
    {
        return LOS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }
    pstQueueCB->usQueueLen = usLen;
    pstQueueCB->usQueueSize = usMaxMsgSize;
    pstQueueCB->usQueueState = OS_QUEUE_INUSED;
    *ppstQueueCBOut = pstQueueCB;

    return OS_OK;
}

/**************************************************************************
 Function    : osQueuePend
 Description : pend a task
 Input       : pstRunTsk
               pstPendList
               uwTimeOut
 Output      : pstRunTsk
 Return      : none
**************************************************************************/
 static void osQueuePend(LOS_TASK_CB *pstRunTsk, LOS_DL_LIST *pstPendList, uint32_t  uwTimeOut)
{
    LOS_DL_LIST *pstPendObj = (LOS_DL_LIST *)NULL;
    LOS_TASK_CB *pstTskCB = (LOS_TASK_CB *)NULL;

    LOS_PriqueueDequeue(&pstRunTsk->stPendList);
    pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_READY);
    pstPendObj = &(pstRunTsk->stPendList);
    pstRunTsk->usTaskStatus |= OS_TASK_STATUS_PEND_QUEUE;
    if (LOS_ListEmpty(pstPendList))
    {
        LOS_ListTailInsert(pstPendList, pstPendObj);
    }
    else
    {
        LOS_DL_LIST_FOR_EACH_ENTRY(pstTskCB, pstPendList, LOS_TASK_CB, stPendList) /*lint !e413*/
        {
            if (pstRunTsk->usPriority < pstTskCB->usPriority)
            {
                break;
            }
        }
        LOS_ListAdd(pstTskCB->stPendList.pstPrev, pstPendObj);
    }

    if (uwTimeOut != LOS_WAIT_FOREVER)
    {
        pstRunTsk->usTaskStatus |= OS_TASK_STATUS_TIMEOUT;
        osTaskAdd2TimerList(pstRunTsk, uwTimeOut);
    }

    return;
}

/**************************************************************************
 Function    : osQueueWakeUp
 Description : wake up the first task in the pending queue
 Input       : pstPendList
 Output      : pstPendList
 Return      : none
**************************************************************************/
 static void osQueueWakeUp(LOS_DL_LIST *pstPendList)
{
    LOS_TASK_CB    *pstResumedTask;

    pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(pstPendList)); /*lint !e413*/
    LOS_ListDelete(LOS_DL_LIST_FIRST(pstPendList));
    pstResumedTask->usTaskStatus &= (~OS_TASK_STATUS_PEND_QUEUE);
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

    return;
}

/*****************************************************************************
 Function    : LOS_QueueCreate
 Description : Create a queue
 Input       : pcQueueName  --- Queue name, less than 4 characters
               usLen        --- Queue lenth
               uwFlags      --- Queue type, FIFO or PRIO
               usMaxMsgSize --- Maximum message size in byte
 Output      : puwQueueID   --- Queue ID
 Return      : OS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_QueueCreate(char *pcQueueName,
                                          uint16_t usLen,
                                          uint32_t *puwQueueID,
                                          uint32_t uwFlags,
                                          uint16_t usMaxMsgSize )
{
    QUEUE_CB_S    *pstQueueCB;
    uint32_t uwIntSave;
    uint32_t      uwRet;

    (void)pcQueueName;
    (void)uwFlags;

    if (NULL == puwQueueID)
    {
        return LOS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }

    if(usMaxMsgSize > UINT16_MAX -4)
    {
        return LOS_ERRNO_QUEUE_SIZE_TOO_BIG;
    }

    if ((0 == usLen) || (0 == usMaxMsgSize))
    {
        return LOS_ERRNO_QUEUE_PARA_ISZERO;
    }

    uwIntSave = LOS_IntLock();
    uwRet = osQueueCreate(usLen, puwQueueID, (usMaxMsgSize + sizeof(uint32_t)), &pstQueueCB);
    if(OS_OK != uwRet)
    {
        LOS_IntRestore(uwIntSave);
        return uwRet;
    }
    LOS_ListInit(&pstQueueCB->stWriteList);
    LOS_ListInit(&pstQueueCB->stReadList);
    LOS_ListInit(&pstQueueCB->stMemList);
    pstQueueCB->usWritableCnt = usLen;

    LOS_IntRestore(uwIntSave);

    return OS_OK;
}

/*****************************************************************************
 Function    : LOS_QueueRead
 Description : read queue
 Input       : uwQueueID
               uwBufferSize
               uwTimeOut
 Output      : pBufferAddr
 Return      : OS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_QueueRead(uint32_t  uwQueueID,
                    void *  pBufferAddr,
                    uint32_t  uwBufferSize,
                    uint32_t  uwTimeOut)
{
    QUEUE_CB_S    *pstQueueCB;
    uint8_t       *pucQueueNode;
    LOS_TASK_CB  *pstRunTsk;
    uint32_t uwIntSave;
    uint32_t      uwRet = OS_OK;
    uint32_t uwInnerID = uwQueueID - 1;

    if ( uwInnerID >= LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return LOS_ERRNO_QUEUE_READ_INVALID;
    }

    if (NULL == pBufferAddr)
    {
        return LOS_ERRNO_QUEUE_READ_PTR_NULL;
    }

    if (0 == uwBufferSize)
    {
        return LOS_ERRNO_QUEUE_READSIZE_ISZERO;
    }

    if (LOS_NO_WAIT != uwTimeOut)
    {
        if (OS_INT_ACTIVE)
        {
            return LOS_ERRNO_QUEUE_READ_IN_INTERRUPT;
        }
    }

    uwIntSave = LOS_IntLock();
    pstQueueCB = (QUEUE_CB_S *)GET_QUEUE_HANDLE(uwInnerID);

    if (OS_QUEUE_UNUSED == pstQueueCB->usQueueState)
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_READ_NOT_CREATE);
    }

    if (0 == pstQueueCB->usReadableCnt)
    {
        if (LOS_NO_WAIT == uwTimeOut)
        {
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_ISEMPTY);
        }

        if (g_usLosTaskLock)
        {
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_PEND_IN_LOCK);
        }

        pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
        osQueuePend(pstRunTsk, &pstQueueCB->stReadList, uwTimeOut);
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();

        uwIntSave = LOS_IntLock();

        if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_TIMEOUT);
        }
    }
    else
    {
        pstQueueCB->usReadableCnt--;
    }

    pucQueueNode = &(pstQueueCB->pucQueue[((pstQueueCB->usQueueHead) * (pstQueueCB->usQueueSize))]);
   *(uint32_t*)pBufferAddr = *(uint32_t*)(pucQueueNode);
    if (++pstQueueCB->usQueueHead == pstQueueCB->usQueueLen)
    {
        pstQueueCB->usQueueHead = 0;
    }

    if (!LOS_ListEmpty(&pstQueueCB->stWriteList))
    {
        osQueueWakeUp(&pstQueueCB->stWriteList);
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();

        return OS_OK;
    }
    else
    {
        pstQueueCB->usWritableCnt++;
    }

QUEUE_END:
    LOS_IntRestore(uwIntSave);
    return uwRet;
}

/*****************************************************************************
 Function    : LOS_QueueWrite
 Description : Write queue
 Input       : uwQueueID
               pBufferAddr
               uwBufferSize
               uwTimeOut
 Output      : None
 Return      : OS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_QueueWrite( uint32_t uwQueueID,
                                     void * pBufferAddr,
                                     uint32_t uwBufferSize,
                                     uint32_t uwTimeOut )
{
    QUEUE_CB_S *pstQueueCB;
    uint8_t    *pucQueueNode;
    LOS_TASK_CB *pstRunTsk;
    uint32_t uwIntSave;
    uint32_t  uwRet = OS_OK;
    uint32_t uwInnerID = uwQueueID - 1;

    if(uwInnerID >= LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return LOS_ERRNO_QUEUE_WRITE_INVALID;
    }

    if (NULL == pBufferAddr)
    {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }

    if(0 == uwBufferSize)
    {
        return LOS_ERRNO_QUEUE_WRITESIZE_ISZERO;
    }

    if (LOS_NO_WAIT != uwTimeOut)
    {
        if (OS_INT_ACTIVE)
        {
            return LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT;
        }
    }

    uwIntSave = LOS_IntLock();

    pstQueueCB = (QUEUE_CB_S *)GET_QUEUE_HANDLE(uwInnerID);

    if (OS_QUEUE_UNUSED == pstQueueCB->usQueueState)
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_WRITE_NOT_CREATE);
    }

    if (uwBufferSize > pstQueueCB->usQueueSize)
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG);
    }

    if (0 == pstQueueCB->usWritableCnt)
    {
        if (LOS_NO_WAIT == uwTimeOut)
        {
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_ISFULL);
        }

        if (g_usLosTaskLock)
        {
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_PEND_IN_LOCK);
        }

        pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
        osQueuePend(pstRunTsk, &pstQueueCB->stWriteList, uwTimeOut);
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();

        uwIntSave = LOS_IntLock();
        if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            GOTO_QUEUE_END(LOS_ERRNO_QUEUE_TIMEOUT);
        }
    }
    else
    {
         pstQueueCB->usWritableCnt--;
    }

    pucQueueNode = &(pstQueueCB->pucQueue[((pstQueueCB->usQueueTail) * (pstQueueCB->usQueueSize))]);
    *((uint32_t *)pucQueueNode) = (uint32_t)pBufferAddr;

    if(++pstQueueCB->usQueueTail == pstQueueCB->usQueueLen)
    {
        pstQueueCB->usQueueTail = 0;
    }

    if (!LOS_ListEmpty(&pstQueueCB->stReadList))
    {
        osQueueWakeUp(&pstQueueCB->stReadList);
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();
        return OS_OK;
    }
    else
    {
        pstQueueCB->usReadableCnt++;
    }
QUEUE_END:
    LOS_IntRestore(uwIntSave);
    return uwRet;
}

/*****************************************************************************
 Function    : osQueueMailAlloc
 Description : Mail allocate memory
 Input       : uwQueueID   --- QueueID
             : pMailPool   --- MailPool
             : uwTimeOut   --- TimeOut
 Output      :
 Return      : pointer if success otherwise NULL
 *****************************************************************************/
 void *osQueueMailAlloc(uint32_t  uwQueueID, void* pMailPool, uint32_t uwTimeOut)
{
    void *pMem = (void *)NULL;
    uint32_t uwIntSave;
    QUEUE_CB_S *pstQueueCB = (QUEUE_CB_S *)NULL;
    LOS_TASK_CB *pstRunTsk = (LOS_TASK_CB *)NULL;
    uint32_t uwInnerID = uwQueueID - 1;

    if (uwInnerID >= LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return NULL;
    }

    if (pMailPool == NULL)
    {
        return NULL;
    }

    if (LOS_NO_WAIT != uwTimeOut)
    {
        if (OS_INT_ACTIVE)
        {
            return NULL;
        }
    }

    uwIntSave = LOS_IntLock();
    pstQueueCB = GET_QUEUE_HANDLE(uwInnerID);
    pMem = LOS_MemboxAlloc(pMailPool);
    if (NULL == pMem)
    {
        if (uwTimeOut == LOS_NO_WAIT)
        {
            goto END;
        }

        pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
        osQueuePend(pstRunTsk, &pstQueueCB->stMemList, uwTimeOut);
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();

        uwIntSave = LOS_IntLock();
        if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
            goto END;
        }
        else
        {
            if (NULL == pstRunTsk->puwMsg)
            {
                //TODO:fault handle
            }
            pMem = pstRunTsk->puwMsg;
            pstRunTsk->puwMsg = NULL;
        }
    }

END:
    LOS_IntRestore(uwIntSave);
    return pMem;
}

/*****************************************************************************
 Function    : osQueueMailFree
 Description : Mail free memory
 Input       : uwQueueID   --- QueueID
             : pMailPool   --- MailPool
 Output      :
 Return      : OS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t osQueueMailFree(uint32_t  uwQueueID, void* pMailPool, void* pMailMem)
{
    void *pMem = (void *)NULL;
    uint32_t uwIntSave;
    QUEUE_CB_S *pstQueueCB = (QUEUE_CB_S *)NULL;
    LOS_TASK_CB *pstResumedTask = (LOS_TASK_CB *)NULL;
    uint32_t uwInnerID = uwQueueID - 1;

    if (uwInnerID >= LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return LOS_ERRNO_QUEUE_MAIL_HANDLE_INVALID;
    }

    if (pMailPool == NULL)
    {
        return LOS_ERRNO_QUEUE_MAIL_PTR_INVALID;
    }

    uwIntSave = LOS_IntLock();

    if (LOS_MemboxFree(pMailPool, pMailMem))
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_QUEUE_MAIL_FREE_ERROR;
    }

    pstQueueCB = GET_QUEUE_HANDLE(uwInnerID);
    if (!LOS_ListEmpty(&pstQueueCB->stMemList))
    {
        pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&pstQueueCB->stMemList)); /*lint !e413*/
        osQueueWakeUp(&pstQueueCB->stMemList);
        pMem = LOS_MemboxAlloc(pMailPool);
        if (NULL == pMem)
        {
            //TODO: fault handle
        }

        pstResumedTask->puwMsg = pMem;
        LOS_IntRestore(uwIntSave);
        LOS_Schedule();
    }
    else
    {
        LOS_IntRestore(uwIntSave);
    }
    return OS_OK;
}

/*****************************************************************************
 Function    : LOS_QueueDelete
 Description : Delete a queue
 Input       : puwQueueID   --- QueueID
 Output      :
 Return      : OS_OK on success or error code on failure
 *****************************************************************************/
 uint32_t LOS_QueueDelete(uint32_t uwQueueID)
{
    QUEUE_CB_S *pstQueueCB;
    uint32_t uwIntSave;
    uint32_t uwRet;
    uint32_t uwInnerID = uwQueueID - 1;

    if (uwInnerID >= LOSCFG_BASE_IPC_QUEUE_LIMIT)
    {
        return LOS_ERRNO_QUEUE_NOT_FOUND;
    }

    uwIntSave = LOS_IntLock();
    pstQueueCB = (QUEUE_CB_S *)GET_QUEUE_HANDLE(uwInnerID);
    if (OS_QUEUE_UNUSED == pstQueueCB->usQueueState)
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_NOT_CREATE);
    }

    if (!LOS_ListEmpty(&pstQueueCB->stReadList))
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_IN_TSKUSE);
    }

    if (!LOS_ListEmpty(&pstQueueCB->stWriteList))
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_IN_TSKUSE);
    }

    if (!LOS_ListEmpty(&pstQueueCB->stMemList))
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_IN_TSKUSE);
    }

    if ((pstQueueCB->usWritableCnt + pstQueueCB->usReadableCnt) != pstQueueCB->usQueueLen)
    {
        GOTO_QUEUE_END(LOS_ERRNO_QUEUE_IN_TSKWRITE);
    }

    uwRet = LOS_MemFree(m_aucSysMem0, (void *)(pstQueueCB->pucQueue));
    if (OS_OK != uwRet)
    {
        GOTO_QUEUE_END(uwRet);
    }

    pstQueueCB->usQueueState = OS_QUEUE_UNUSED;

QUEUE_END:
    LOS_IntRestore(uwIntSave);
    return uwRet;
}

#endif /*(LOSCFG_BASE_IPC_QUEUE == YES)*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
