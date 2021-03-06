//NeMOS mutex relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include "los_mux.inc"

#include "los_memory.ph"
#include "los_priqueue.ph"
#include "los_task.h"
#include "los_hw.h"
#include "los_hwi.h"

#include "los_demo_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#if (LOSCFG_BASE_IPC_MUX == YES)

 MUX_CB_S             *g_pstAllMux;
 LOS_DL_LIST    g_stUnusedMuxList;


/*****************************************************************************
 Funtion      : osMuxInit,
 Description  : Initializes the mutex,
 Input        : None
 Output       : None
 Return       : OS_OK on success ,or error code on failure
 *****************************************************************************/
 uint32_t osMuxInit(void)
{
    MUX_CB_S *pstMuxNode;
    uint32_t   uwIndex;

    LOS_ListInit(&g_stUnusedMuxList);
    if (LOSCFG_BASE_IPC_MUX_LIMIT > 0)   /*lint !e506*/
    {
        g_pstAllMux = (MUX_CB_S *)LOS_MemAlloc(m_aucSysMem0, (LOSCFG_BASE_IPC_MUX_LIMIT * sizeof(MUX_CB_S)));
        if (NULL == g_pstAllMux)
        {
            return LOS_ERRNO_MUX_NO_MEMORY;
        }

        for (uwIndex = 0; uwIndex < LOSCFG_BASE_IPC_MUX_LIMIT; uwIndex++)
        {
            pstMuxNode              = ((MUX_CB_S *)g_pstAllMux) + uwIndex;
            pstMuxNode->ucMuxID     = uwIndex;
            pstMuxNode->ucMuxStat   = OS_MUX_UNUSED;
            LOS_ListTailInsert(&g_stUnusedMuxList, &pstMuxNode->stMuxList);
        }
    }
    return OS_OK;
}

/*****************************************************************************
 Function     : LOS_MuxCreate
 Description  : Create a mutex,
 Input        : None
 Output       : puwMuxHandle ------ Mutex operation handle
 Return       : OS_OK on success ,or error code on failure
 *****************************************************************************/
  uint32_t  LOS_MuxCreate (uint32_t *puwMuxHandle)
{
    uint32_t uwIntSave;
    MUX_CB_S    *pstMuxCreated;
    LOS_DL_LIST *pstUnusedMux;

    if (NULL == puwMuxHandle)
    {
        return LOS_ERRNO_MUX_PTR_NULL;
    }

    uwIntSave = LOS_IntLock();
    if (LOS_ListEmpty(&g_stUnusedMuxList))
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_ALL_BUSY;
    }

    pstUnusedMux                = LOS_DL_LIST_FIRST(&(g_stUnusedMuxList));
    LOS_ListDelete(pstUnusedMux);
    pstMuxCreated               = (GET_MUX_LIST(pstUnusedMux)); /*lint !e413*/
    pstMuxCreated->usMuxCount   = 0;
    pstMuxCreated->ucMuxStat    = OS_MUX_USED;
    pstMuxCreated->usPriority   = 0;
    pstMuxCreated->pstOwner     = (LOS_TASK_CB *)NULL;
    LOS_ListInit(&pstMuxCreated->stMuxList);
    *puwMuxHandle               = (uint32_t)pstMuxCreated->ucMuxID;
    LOS_IntRestore(uwIntSave);
    return OS_OK;
}

/*****************************************************************************
 Function     : LOS_MuxDelete
 Description  : Delete a mutex,
 Input        : uwMuxHandle------Mutex operation handle
 Output       : None
 Return       : OS_OK on success ,or error code on failure
 *****************************************************************************/
 uint32_t LOS_MuxDelete(uint32_t uwMuxHandle)
{
    uint32_t uwIntSave;
    MUX_CB_S *pstMuxDeleted;

   pstMuxDeleted = GET_MUX(uwMuxHandle);
   uwIntSave = LOS_IntLock();
   if ((uwMuxHandle >= (uint32_t)LOSCFG_BASE_IPC_MUX_LIMIT) ||
        (OS_MUX_UNUSED == pstMuxDeleted->ucMuxStat))
   {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_INVALID;
    }

   if (!LOS_ListEmpty(&pstMuxDeleted->stMuxList) || pstMuxDeleted->usMuxCount)
   {
       LOS_IntRestore(uwIntSave);
       return LOS_ERRNO_MUX_PENDED;
    }

   LOS_ListAdd(&g_stUnusedMuxList, &pstMuxDeleted->stMuxList);
   pstMuxDeleted->ucMuxStat = OS_MUX_UNUSED;

   LOS_IntRestore(uwIntSave);

    return OS_OK;
}

/*****************************************************************************
 Function     : LOS_MuxPend
 Description  : Specify the mutex P operation,
 Input        : uwMuxHandle ------ Mutex operation handleone,
                uwTimeOut  ------- waiting time,
 Output       : None
 Return       : OS_OK on success ,or error code on failure
 *****************************************************************************/
 uint32_t LOS_MuxPend(uint32_t uwMuxHandle, uint32_t uwTimeout)
{
    uint32_t uwIntSave;
    MUX_CB_S  *pstMuxPended;
    uint32_t     uwRetErr;
    LOS_TASK_CB  *pstRunTsk;
    LOS_DL_LIST  *pstPendObj;

    pstMuxPended = GET_MUX(uwMuxHandle);
    uwIntSave = LOS_IntLock();
    if ((uwMuxHandle >= (uint32_t)LOSCFG_BASE_IPC_MUX_LIMIT)
    || (OS_MUX_UNUSED == pstMuxPended->ucMuxStat))
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_INVALID;
    }

    if (OS_INT_ACTIVE)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_PEND_INTERR;
    }

    pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
    if (pstMuxPended->usMuxCount == 0)
    {
        pstMuxPended->usMuxCount++;
        pstMuxPended->pstOwner = pstRunTsk;
        pstMuxPended->usPriority = pstRunTsk->usPriority;
        LOS_IntRestore(uwIntSave);
        return OS_OK;
    }

    if (pstMuxPended->pstOwner == pstRunTsk)
    {
        pstMuxPended->usMuxCount++;
        LOS_IntRestore(uwIntSave);
        return OS_OK;
    }

    if (!uwTimeout)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_UNAVAILABLE;
    }

    if (g_usLosTaskLock)
    {
        uwRetErr = LOS_ERRNO_MUX_PEND_IN_LOCK;
        PRINT_ERR("!!!LOS_ERRNO_MUX_PEND_IN_LOCK!!!\n");
        goto errre_uniMuxPend;
    }

    LOS_PriqueueDequeue(&pstRunTsk->stPendList);
    pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_READY);
    pstRunTsk->pTaskMux = (void *)pstMuxPended;
    pstPendObj = &pstRunTsk->stPendList;
    pstRunTsk->usTaskStatus |= OS_TASK_STATUS_PEND;
    if ((pstMuxPended->pstOwner->usPriority) > pstRunTsk->usPriority)
    {
        osTaskPriModify(pstMuxPended->pstOwner, pstRunTsk->usPriority);
    }

    LOS_ListTailInsert(&pstMuxPended->stMuxList, pstPendObj);
    if (uwTimeout != LOS_WAIT_FOREVER)
    {
        pstRunTsk->usTaskStatus |= OS_TASK_STATUS_TIMEOUT;
        osTaskAdd2TimerList((LOS_TASK_CB *)pstRunTsk, uwTimeout);
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
        uwRetErr = LOS_ERRNO_MUX_TIMEOUT;
        goto error_uniMuxPend;
    }

    return OS_OK;

errre_uniMuxPend:
    (void)LOS_IntRestore(uwIntSave);
error_uniMuxPend:
    return (uwRetErr);
}

/*****************************************************************************
 Function     : LOS_MuxPost
 Description  : Specify the mutex V operation,
 Input        : uwMuxHandle ------ Mutex operation handle,
 Output       : None
 Return       : OS_OK on success ,or error code on failure
 *****************************************************************************/
 uint32_t LOS_MuxPost(uint32_t uwMuxHandle)
{
    uint32_t uwIntSave;
    MUX_CB_S    *pstMuxPosted = GET_MUX(uwMuxHandle);
    LOS_TASK_CB *pstResumedTask;
    LOS_TASK_CB *pstRunTsk;

    uwIntSave = LOS_IntLock();

    if ((uwMuxHandle >= (uint32_t)LOSCFG_BASE_IPC_MUX_LIMIT) ||
        (OS_MUX_UNUSED == pstMuxPosted->ucMuxStat))
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_INVALID;
    }

    if (OS_INT_ACTIVE)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_PEND_INTERR;
    }

    pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
    if (pstMuxPosted->usMuxCount == 0 || pstMuxPosted->pstOwner != pstRunTsk)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_MUX_INVALID;
    }

    if (--(pstMuxPosted->usMuxCount) != 0)
    {
        LOS_IntRestore(uwIntSave);
        return OS_OK;
    }

    if ((pstMuxPosted->pstOwner->usPriority) != pstMuxPosted->usPriority)
    {
        osTaskPriModify(pstMuxPosted->pstOwner, pstMuxPosted->usPriority);
    }

    if (!LOS_ListEmpty(&pstMuxPosted->stMuxList))
    {
        pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(pstMuxPosted->stMuxList))); /*lint !e413*/
        LOS_ListDelete(LOS_DL_LIST_FIRST(&(pstMuxPosted->stMuxList)));
        LOS_ASSERT_COND(pstResumedTask->usTaskStatus & OS_TASK_STATUS_PEND);
        pstResumedTask->usTaskStatus &= (~OS_TASK_STATUS_PEND);
        if (pstResumedTask->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
        {
            osTimerListDelete(pstResumedTask);
            pstResumedTask->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
        }

        pstMuxPosted->usMuxCount    = 1;
        pstMuxPosted->pstOwner      = pstResumedTask;
        pstMuxPosted->usPriority    = pstResumedTask->usPriority;
        pstResumedTask->pTaskMux    = NULL;
        if (!(pstResumedTask->usTaskStatus & OS_TASK_STATUS_SUSPEND))
        {
            pstResumedTask->usTaskStatus |= OS_TASK_STATUS_READY;
            LOS_PriqueueEnqueue(&pstResumedTask->stPendList, pstResumedTask->usPriority);
        }

        (void)LOS_IntRestore(uwIntSave);
        LOS_Schedule();
    }
    else
    {
        (void)LOS_IntRestore(uwIntSave);
    }

    return OS_OK;
}
#endif /*(LOSCFG_BASE_IPC_MUX == YES)*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
