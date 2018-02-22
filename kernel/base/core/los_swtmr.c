//Tiny OS Soft Timer driver.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/22/2018

#include "los_swtmr.inc"
#include "los_base.ph"
#include "los_membox.ph"
#include "los_memory.ph"
#include "los_queue.ph"
#include "los_task.ph"

#include "los_hwi.h"

#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_BASE_CORE_SWTMR == YES)

static  uint32_t    m_uwSwTmrHandlerQueue;        /*Software Timer timeout queue ID*/
 SWTMR_CTRL_S     *m_pstSwtmrCBArray;           /*first address in Timer memory space  */
 SWTMR_CTRL_S     *m_pstSwtmrFreeList;          /*Free list of Softwaer Timer*/
 SWTMR_CTRL_S     *m_pstSwtmrSortList;          /*The software timer count list*/

#if (LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK == YES)
 uint8_t m_aucSwTmrHandlerPool[sizeof(OS_MEMBOX_S) + ((sizeof(SWTMR_HANDLER_ITEM_S) + 4 + 3) & (~3)) * OS_SWTMR_HANDLE_QUEUE_SIZE + 4];
#else
 uint8_t m_aucSwTmrHandlerPool[sizeof(OS_MEMBOX_S) + ((sizeof(SWTMR_HANDLER_ITEM_S) + 3) & (~3)) * OS_SWTMR_HANDLE_QUEUE_SIZE];
#endif

/*****************************************************************************
Function   : osSwTmrTaskCreate
Description: Create high priority task
Input      : None
Output     : None
Return     : None
*****************************************************************************/
 void osSwTmrTask(void)
{
    SWTMR_HANDLER_ITEM_P pstSwtmrHandle = (SWTMR_HANDLER_ITEM_P)NULL;
    SWTMR_HANDLER_ITEM_S stSwtmrHandle;
    uint32_t uwRet;

    for ( ; ; )
    {
        uwRet = LOS_QueueRead(m_uwSwTmrHandlerQueue, &pstSwtmrHandle, sizeof(SWTMR_HANDLER_ITEM_P), LOS_WAIT_FOREVER);
        if (uwRet == LOS_OK)
        {
            if (pstSwtmrHandle != NULL)
            {
                stSwtmrHandle.pfnHandler = pstSwtmrHandle->pfnHandler;
                stSwtmrHandle.uwArg = pstSwtmrHandle->uwArg;
                (void)LOS_MemboxFree(m_aucSwTmrHandlerPool, pstSwtmrHandle);
                if (stSwtmrHandle.pfnHandler != NULL)
                {
                    stSwtmrHandle.pfnHandler(stSwtmrHandle.uwArg);
                }
            }
        }
    }//end of for
}

/*****************************************************************************
Function   : osSwTmrTaskCreate
Description: Create Software Timer
Input      : None
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t osSwTmrTaskCreate(void)
{
    uint32_t uwRet;
    TSK_INIT_PARAM_S stSwTmrTask;

    (void)memset(&stSwTmrTask, 0, sizeof(TSK_INIT_PARAM_S));
    stSwTmrTask.pfnTaskEntry    = (TSK_ENTRY_FUNC)osSwTmrTask;
    stSwTmrTask.uwStackSize     = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stSwTmrTask.pcName          = "Swt_Task";
    stSwTmrTask.usTaskPrio      = 0;
    stSwTmrTask.uwResved        = LOS_TASK_STATUS_DETACHED;
    uwRet = LOS_TaskCreate(&g_uwSwtmrTaskID, &stSwTmrTask);
    return uwRet;
}

/*****************************************************************************
Function   : osSwTmrInit
Description: Initializes Software Timer
Input      : None
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t osSwTmrInit(void)
{
    uint32_t uwSize;
    uint16_t usIndex;
    uint32_t uwRet;
    SWTMR_CTRL_S *pstSwtmr;
    SWTMR_CTRL_S *pstTemp;

    if (0 == LOSCFG_BASE_CORE_SWTMR_LIMIT)  /*lint !e506*/
    {
        return LOS_ERRNO_SWTMR_MAXSIZE_INVALID;
    }

    m_pstSwtmrSortList = (SWTMR_CTRL_S *)NULL;
    uwSize = sizeof(SWTMR_CTRL_S) * LOSCFG_BASE_CORE_SWTMR_LIMIT;
    pstSwtmr = (SWTMR_CTRL_S *)LOS_MemAlloc(m_aucSysMem0, uwSize);
    if (NULL == pstSwtmr)
    {
        return LOS_ERRNO_SWTMR_NO_MEMORY;
    }

    (void)memset((void *)pstSwtmr, 0, uwSize);
    m_pstSwtmrCBArray = pstSwtmr;
    m_pstSwtmrFreeList = pstSwtmr;
    pstSwtmr->usTimerID = 0;
    pstTemp = pstSwtmr;
    pstSwtmr++;
    for (usIndex = 1; usIndex < LOSCFG_BASE_CORE_SWTMR_LIMIT; usIndex++, pstSwtmr++)
    {
        pstSwtmr->usTimerID = usIndex;
        pstTemp->pstNext = pstSwtmr;
        pstTemp = pstSwtmr;
    }

    uwRet = LOS_MemboxInit(m_aucSwTmrHandlerPool, sizeof(m_aucSwTmrHandlerPool), sizeof(SWTMR_HANDLER_ITEM_S));
    if (uwRet != LOS_OK)
    {
        return LOS_ERRNO_SWTMR_HANDLER_POOL_NO_MEM;
    }

    uwRet = LOS_QueueCreate((char *)NULL, OS_SWTMR_HANDLE_QUEUE_SIZE, &m_uwSwTmrHandlerQueue, 0, sizeof(SWTMR_HANDLER_ITEM_P));
    if (uwRet != LOS_OK)
    {
        return LOS_ERRNO_SWTMR_QUEUE_CREATE_FAILED;
    }

    uwRet = osSwTmrTaskCreate();
    if (LOS_OK != uwRet)
    {
        return LOS_ERRNO_SWTMR_TASK_CREATE_FAILED;
    }

    return LOS_OK;
}

/*****************************************************************************
Function   : osSwTmrStart
Description: Start Software Timer
Input      : pstSwtmr ---------- Need to start Software Timer
Output     : None
Return     : None
*****************************************************************************/
 void osSwTmrStart(SWTMR_CTRL_S *pstSwtmr)
{
    SWTMR_CTRL_S *pstPrev = (SWTMR_CTRL_S *)NULL;
    SWTMR_CTRL_S *pstCur = (SWTMR_CTRL_S *)NULL;

    if (pstSwtmr->ucOverrun == 0 && (pstSwtmr->ucMode == LOS_SWTMR_MODE_ONCE || pstSwtmr->ucMode == LOS_SWTMR_MODE_OPP))
    {
        pstSwtmr->uwCount = pstSwtmr->uwExpiry;
    }
    else
    {
        pstSwtmr->uwCount = pstSwtmr->uwInterval;
    }
    pstCur = m_pstSwtmrSortList;

    while (pstCur != NULL)
    {
        if (pstCur->uwCount > pstSwtmr->uwCount)
        {
            break;
        }

        pstSwtmr->uwCount -= pstCur->uwCount;
        pstPrev = pstCur;
        pstCur = pstCur->pstNext;
    }

    pstSwtmr->pstNext = pstCur;

    if (pstCur != NULL)
    {
        pstCur->uwCount -= pstSwtmr->uwCount;
    }

    if (pstPrev == NULL)
    {
        m_pstSwtmrSortList = pstSwtmr;
    }
    else
    {
        pstPrev->pstNext = pstSwtmr;
    }

    pstSwtmr->ucState = OS_SWTMR_STATUS_TICKING;

    return;
}

/*****************************************************************************
Function   : osSwTmrDelete
Description: Delete Software Timer
Input      : pstSwtmr --- Need to delete Software Timer, When using, Ensure that it can't be NULL.
Output     : None
Return     : None
*****************************************************************************/
void osSwtmrDelete(SWTMR_CTRL_S *pstSwtmr)
{
    /**insert to free list **/
    pstSwtmr->pstNext = m_pstSwtmrFreeList;
    m_pstSwtmrFreeList = pstSwtmr;
    pstSwtmr->ucState = OS_SWTMR_STATUS_UNUSED;
}

/*****************************************************************************
Function   : osSwTmrTimeoutHandle
Description: Software Timer time out handler
Input      : None
Output     : None
Return     : None
*****************************************************************************/
 static void osSwTmrTimeoutHandle(void)
{
    SWTMR_HANDLER_ITEM_P pstSwtmrHandler;
    SWTMR_CTRL_S *pstSwtmr = m_pstSwtmrSortList;

    while (pstSwtmr != NULL && pstSwtmr->uwCount == 0)
    {
        m_pstSwtmrSortList = pstSwtmr->pstNext;
        pstSwtmrHandler = (SWTMR_HANDLER_ITEM_P)LOS_MemboxAlloc(m_aucSwTmrHandlerPool);

        if (pstSwtmrHandler != NULL)
        {
            pstSwtmrHandler->pfnHandler = pstSwtmr->pfnHandler;
            pstSwtmrHandler->uwArg = pstSwtmr->uwArg;

            if (LOS_QueueWrite(m_uwSwTmrHandlerQueue, pstSwtmrHandler, sizeof(uint32_t), LOS_NO_WAIT))
            {
                (void)LOS_MemboxFree(m_aucSwTmrHandlerPool, pstSwtmrHandler);
            }
        }

        if (pstSwtmr->ucMode == LOS_SWTMR_MODE_ONCE)
        {
            osSwtmrDelete(pstSwtmr);

            if (pstSwtmr->usTimerID < OS_SWTMR_MAX_TIMERID - LOSCFG_BASE_CORE_SWTMR_LIMIT)
                pstSwtmr->usTimerID += LOSCFG_BASE_CORE_SWTMR_LIMIT;
            else
                pstSwtmr->usTimerID %= LOSCFG_BASE_CORE_SWTMR_LIMIT;
        }
        else
        {
            pstSwtmr->ucOverrun++;
            osSwTmrStart(pstSwtmr);
        }

        pstSwtmr = m_pstSwtmrSortList;
    }
}

/*****************************************************************************
Function   : osSwtmrScan
Description: Tick interrupt interface module of Software Timer
Input      : None
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t osSwtmrScan(void)
{
    if (m_pstSwtmrSortList != NULL)
    {
        if (--(m_pstSwtmrSortList->uwCount) == 0)
        {
            osSwTmrTimeoutHandle();
        }
    }
    return LOS_OK;
}

/*****************************************************************************
Function   : osSwTmrGetNextTimeout
Description: Get next timeout
Input      : None
Output     : None
Return     : Count of the Timer list
*****************************************************************************/
 uint32_t osSwTmrGetNextTimeout(void)
{
    if (m_pstSwtmrSortList == NULL)
    {
        return 0xFFFFFFFF;
    }
    return m_pstSwtmrSortList->uwCount;
}


/*****************************************************************************
Function   : osSwTmrAdjust
Description: Adjust Software Timer list
Input      : sleep_time
Output     : None
Return     : None
*****************************************************************************/
 void osSwTmrAdjust(uint32_t uwsleep_time)
{
    if (uwsleep_time > m_pstSwtmrSortList->uwCount)
    {
        uwsleep_time = m_pstSwtmrSortList->uwCount;
    }

    m_pstSwtmrSortList->uwCount -= uwsleep_time;

    if (m_pstSwtmrSortList->uwCount == 0)
    {
        osSwTmrTimeoutHandle();
    }
}

/*****************************************************************************
Function   : osSwtmrStop
Description: Stop of Software Timer interface
Input      : pstSwtmr
Output     : None
Return     : None
*****************************************************************************/
 void osSwtmrStop(SWTMR_CTRL_S *pstSwtmr)
{
    SWTMR_CTRL_S *pstPrev = (SWTMR_CTRL_S *)NULL;
    SWTMR_CTRL_S *pstCur = (SWTMR_CTRL_S *)NULL;

    pstCur = m_pstSwtmrSortList;

    while (pstCur != pstSwtmr)
    {
        pstPrev = pstCur;
        pstCur = pstCur->pstNext;
    }

    if (pstCur->pstNext != NULL)
    {
        pstCur->pstNext->uwCount += pstCur->uwCount;
    }

    if (pstPrev == NULL)
    {
        m_pstSwtmrSortList = pstCur->pstNext;
    }
    else
    {
        pstPrev->pstNext = pstCur->pstNext;
    }

    pstCur->pstNext = (SWTMR_CTRL_S *)NULL;
    pstCur->ucState = OS_SWTMR_STATUS_CREATED;
    pstCur->ucOverrun = 0;
}

/*****************************************************************************
Function   : LOS_SwtmrCreate
Description: Create software timer
Input      : uwInterval
             usMode
             pfnHandler
             uwArg
Output     : pusSwTmrID
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t LOS_SwtmrCreate(uint32_t  uwInterval,
                                        uint8_t           ucMode,
                                        SWTMR_PROC_FUNC pfnHandler,
                                        uint16_t          *pusSwTmrID,
                                        uint32_t          uwArg)
{
    SWTMR_CTRL_S  *pstSwtmr;
    uint32_t uwIntSave;

    if (0 == uwInterval)
    {
        return LOS_ERRNO_SWTMR_INTERVAL_NOT_SUITED;
    }

    if ((LOS_SWTMR_MODE_ONCE != ucMode) && (LOS_SWTMR_MODE_PERIOD != ucMode))
    {
        return LOS_ERRNO_SWTMR_MODE_INVALID;
    }

    if (NULL == pfnHandler)
    {
        return LOS_ERRNO_SWTMR_PTR_NULL;
    }

    if (NULL == pusSwTmrID)
    {
        return LOS_ERRNO_SWTMR_RET_PTR_NULL;
    }

    uwIntSave = LOS_IntLock();
    if (NULL == m_pstSwtmrFreeList)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_SWTMR_MAXSIZE;
    }

    pstSwtmr = m_pstSwtmrFreeList;
    m_pstSwtmrFreeList = pstSwtmr->pstNext;
    LOS_IntRestore(uwIntSave);
    pstSwtmr->pfnHandler    = pfnHandler;
    pstSwtmr->ucMode        = ucMode;
    pstSwtmr->ucOverrun     = 0;
    pstSwtmr->uwInterval    = uwInterval;
    pstSwtmr->uwExpiry      = uwInterval;
    pstSwtmr->pstNext       = (SWTMR_CTRL_S *)NULL;
    pstSwtmr->uwCount       = 0;
    pstSwtmr->uwArg         = uwArg;
    pstSwtmr->ucState       = OS_SWTMR_STATUS_CREATED;
    *pusSwTmrID = pstSwtmr->usTimerID;

    return LOS_OK;
}

/*****************************************************************************
Function   : LOS_SwtmrStart
Description: Start software timer
Input      : usSwTmrID ------- Software timer ID
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t LOS_SwtmrStart(uint16_t usSwTmrID)
{
    SWTMR_CTRL_S  *pstSwtmr;
    uint32_t uwIntSave;
    uint32_t uwRet = LOS_OK;
    uint16_t usSwTmrCBID;

    if (usSwTmrID >= OS_SWTMR_MAX_TIMERID)
    {
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    uwIntSave = LOS_IntLock();
    usSwTmrCBID = usSwTmrID % LOSCFG_BASE_CORE_SWTMR_LIMIT;
    pstSwtmr = m_pstSwtmrCBArray + usSwTmrCBID;

    if (pstSwtmr->usTimerID != usSwTmrID)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (pstSwtmr->ucState)
    {
    case OS_SWTMR_STATUS_UNUSED:
        uwRet = LOS_ERRNO_SWTMR_NOT_CREATED;
        break;
    case OS_SWTMR_STATUS_TICKING:
        osSwtmrStop(pstSwtmr);
    case OS_SWTMR_STATUS_CREATED: /*lint !e616*/
        osSwTmrStart(pstSwtmr);
        break;
    default:
        uwRet = LOS_ERRNO_SWTMR_STATUS_INVALID;
        break;
    }

    LOS_IntRestore(uwIntSave);
    return uwRet;
}

/*****************************************************************************
Function   : LOS_SwtmrStop
Description: Stop software timer
Input      : usSwTmrID ------- Software timer ID
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t LOS_SwtmrStop(uint16_t usSwTmrID)
{
    SWTMR_CTRL_S *pstSwtmr;
    uint32_t uwIntSave;
    uint32_t uwRet = LOS_OK;
    uint16_t usSwTmrCBID;

    if (usSwTmrID >= OS_SWTMR_MAX_TIMERID)
    {
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    uwIntSave = LOS_IntLock();
    usSwTmrCBID = usSwTmrID % LOSCFG_BASE_CORE_SWTMR_LIMIT;
    pstSwtmr = m_pstSwtmrCBArray + usSwTmrCBID;

    if (pstSwtmr->usTimerID != usSwTmrID)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (pstSwtmr->ucState)
    {
    case OS_SWTMR_STATUS_UNUSED:
        uwRet = LOS_ERRNO_SWTMR_NOT_CREATED;
        break;
    case OS_SWTMR_STATUS_CREATED:
        uwRet = LOS_ERRNO_SWTMR_NOT_STARTED;
        break;
    case OS_SWTMR_STATUS_TICKING:
        osSwtmrStop(pstSwtmr);
        break;
    default:
        uwRet = LOS_ERRNO_SWTMR_STATUS_INVALID;
        break;
    }

    LOS_IntRestore(uwIntSave);
    return uwRet;
}

/*****************************************************************************
Function   : LOS_SwtmrDelete
Description: Delete software timer
Input      : usSwTmrID ------- Software timer ID
Output     : None
Return     : LOS_OK on success or error code on failure
*****************************************************************************/
 uint32_t LOS_SwtmrDelete(uint16_t usSwTmrID)
{
    SWTMR_CTRL_S  *pstSwtmr;
    uint32_t uwIntSave;
    uint32_t uwRet = LOS_OK;
    uint16_t usSwTmrCBID;

    if (usSwTmrID >= OS_SWTMR_MAX_TIMERID)
    {
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    uwIntSave = LOS_IntLock();
    usSwTmrCBID = usSwTmrID % LOSCFG_BASE_CORE_SWTMR_LIMIT;
    pstSwtmr = m_pstSwtmrCBArray + usSwTmrCBID;

    if (pstSwtmr->usTimerID != usSwTmrID)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (pstSwtmr->ucState)
    {
    case OS_SWTMR_STATUS_UNUSED:
        uwRet = LOS_ERRNO_SWTMR_NOT_CREATED;
        break;
    case OS_SWTMR_STATUS_TICKING:
        osSwtmrStop(pstSwtmr);
    case OS_SWTMR_STATUS_CREATED:  /*lint !e616*/
        osSwtmrDelete(pstSwtmr);
        break;
    default:
        uwRet = LOS_ERRNO_SWTMR_STATUS_INVALID;
        break;
    }

    LOS_IntRestore(uwIntSave);
    return uwRet;
}

#endif /*(LOSCFG_BASE_CORE_SWTMR == YES)*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
