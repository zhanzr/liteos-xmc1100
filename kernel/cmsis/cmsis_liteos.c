//Tiny OS CMSIS RTOS interface
//
//This IS NOT a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018

#include "cmsis_os.h"

#include "los_event.h"
#include "los_membox.h"
#include "los_hwi.h"

#include "los_mux.ph"
#include "los_queue.ph"
#include "los_sem.ph"
#include "los_swtmr.ph"
#include "los_sys.ph"
#include "los_task.ph"
#include "los_tick.h"

#include "stdio.h"
#include "string.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


struct os_pool_cb
{
    uint32_t poolid;
};

struct os_messageQ_cb
{
    uint32_t msgid;
};

struct os_mailQ_cb
{
    uint32_t mailid;
};

#define PRIORITY_WIN 4u
extern  uint32_t LOS_Start(void);
/// Start the RTOS Kernel with executing the specified thread
osStatus osKernelStart(void)
{
    LOS_Start();
    return osOK;
}

uint32_t osKernelSysTick (void)
{
    //todo: need to use external clock source
    return (uint32_t)g_ullTickCount;
}

osStatus osKernelInitialize (void)
{
    int32_t ret;

    osRegister();

    ret = osMain();
    if (ret != LOS_OK)
    {
        return osErrorOS;
    }
    
    LOS_EnableTick();
    return osOK;
}

// Thread Public API

/// Create a thread and add it to Active Threads and set it to state READY
osThreadId osThreadCreate(const osThreadDef_t *thread_def, void *argument)
{
    osThreadId tskcb;
    TSK_INIT_PARAM_S stTskInitParam;
    uint32_t uwTskHandle;
    uint32_t uwRet;
    if ((thread_def == NULL) ||
        (thread_def->pthread == NULL) ||
        (thread_def->tpriority < osPriorityIdle) ||
        (thread_def->tpriority > osPriorityRealtime)) {
        return (osThreadId)NULL;
    }

    memset(&stTskInitParam, 0, sizeof(TSK_INIT_PARAM_S));
    stTskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)thread_def->pthread;
    stTskInitParam.uwStackSize  = thread_def->stacksize;
    stTskInitParam.pcName       = thread_def->name;
    stTskInitParam.auwArgs[0] = (uint32_t)argument;
    stTskInitParam.usTaskPrio   = (uint16_t)(PRIORITY_WIN - thread_def->tpriority);  /*1~7*/
    stTskInitParam.uwResved   = LOS_TASK_STATUS_DETACHED; /*the cmsis task is detached,the task can deleteself*/

    uwRet = LOS_TaskCreate(&uwTskHandle, &stTskInitParam);

    if(LOS_OK != uwRet )
    {
        return (osThreadId)NULL;
    }

    tskcb = (osThreadId)&g_pstTaskCBArray[uwTskHandle];

    return tskcb;
}

/// Return the thread ID of the current running thread
osThreadId osThreadGetId(void)
{
    return (osThreadId)g_stLosTask.pstRunTask;
}

uint32_t osThreadGetPId(osThreadId thread_id)
{
    return ((LOS_TASK_CB *)thread_id)->uwTaskID;
}
/// Terminate execution of a thread and remove it from ActiveThreads
osStatus osThreadTerminate(osThreadId thread_id)
{
    uint32_t  uwRet;

    if (thread_id == NULL)
        return osErrorParameter;

    if (OS_INT_ACTIVE)
        return osErrorISR;

    uwRet = LOS_TaskDelete(((LOS_TASK_CB *)thread_id)->uwTaskID);

    if (uwRet == LOS_OK)
        return osOK;
    else
        return osErrorOS;
}

/// Pass control to next thread that is in state READY
osStatus osThreadYield(void)
{
    uint32_t  uwRet;

    if (OS_INT_ACTIVE)
        return osErrorISR;

    uwRet = LOS_TaskYield();

    if (uwRet == LOS_OK)
        return osOK;
    else
        return osErrorOS;
}

/// Change prority of an active thread
osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority)
{
    uint32_t  uwRet;
    uint16_t    usPriorityTemp;

    if (thread_id == NULL)
        return osErrorParameter;

    if (OS_INT_ACTIVE)
        return osErrorISR;

    if (priority < osPriorityIdle || priority > osPriorityRealtime)
        return osErrorPriority;

    usPriorityTemp = PRIORITY_WIN - priority;

    uwRet = LOS_TaskPriSet(((LOS_TASK_CB *)thread_id)->uwTaskID, usPriorityTemp);

    if (uwRet == LOS_OK)
        return osOK;
    else
        return osErrorOS;
}

/// Get current prority of an active thread
osPriority osThreadGetPriority(osThreadId thread_id)
{
    uint16_t usPriorityTemp;
    int16_t osPriorityRet;

    if (thread_id == NULL)
        return osPriorityError;

    usPriorityTemp = LOS_TaskPriGet(((LOS_TASK_CB *)thread_id)->uwTaskID);

    osPriorityRet = PRIORITY_WIN - usPriorityTemp;

    if (osPriorityRet < osPriorityIdle || osPriorityRet > osPriorityRealtime)
        return osPriorityError;

    return (osPriority)osPriorityRet;
}

osSemaphoreId osSemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count)
{
#if (LOSCFG_BASE_IPC_SEM == YES)
    uint32_t uwRet;
    uint32_t * SemHandle;

    if (semaphore_def == NULL)
    {
        return (osSemaphoreId)NULL;
    }

    SemHandle = (uint32_t *)(semaphore_def->puwSemHandle);
    uwRet =  LOS_SemCreate (count,  SemHandle);

    if (uwRet == LOS_OK)
    {
        return (osSemaphoreId)GET_SEM(*SemHandle);
    }
    else
    {
        return (osSemaphoreId)NULL;
    }
#endif
}

/// Wait until a Semaphore becomes available
/*
number of available tokens, or -1 in case of incorrect parameters.
*/
int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_SEM == YES)
    uint32_t uwRet;
    uint32_t SemID;

    if (semaphore_id == NULL)
    {
        return -1;
    }

    if (OS_INT_ACTIVE)
    {
        return -1;
    }

    SemID = ((SEM_CB_S *)semaphore_id)->usSemID;

    uwRet = LOS_SemPend(SemID, LOS_MS2Tick(millisec));

    if (uwRet == LOS_OK)
    {
        return ((SEM_CB_S *)semaphore_id)->uwSemCount;
    }
    else
    {
        return -1;
    }
#endif
}

/// Release a Semaphore
/*
osOK: the semaphore has been released.
osErrorResource: all tokens have already been released.
osErrorParameter: the parameter semaphore_id is incorrect.
*/
osStatus osSemaphoreRelease(osSemaphoreId semaphore_id)
{
#if (LOSCFG_BASE_IPC_SEM == YES)
    uint32_t  uwRet;
    uint32_t  SemID;

    if (semaphore_id == NULL)
    {
        return osErrorParameter;
    }

    SemID = ((SEM_CB_S *)semaphore_id)->usSemID;
    uwRet = LOS_SemPost(SemID);

    if (uwRet == LOS_OK)
    {
        return osOK;
    }
    else if (uwRet == LOS_ERRNO_SEM_INVALID)
    {
        return osErrorParameter;
    }
    else
    {
        return osErrorResource;
    }
#endif
}

/*
osOK: the semaphore object has been deleted.
osErrorISR: osSemaphoreDelete cannot be called from interrupt service routines.
osErrorResource: the semaphore object could not be deleted.
osErrorParameter: the parameter semaphore_id is incorrect.
*/
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id)
{
#if (LOSCFG_BASE_IPC_SEM == YES)
    uint32_t  uwRet;
    uint32_t  SemID;

    if (semaphore_id == NULL)
    {
        return osErrorParameter;
    }

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    SemID = ((SEM_CB_S *)semaphore_id)->usSemID;
    uwRet = LOS_SemDelete(SemID);

    if (uwRet == LOS_OK)
    {
        return osOK;
    }
    else if (uwRet == LOS_ERRNO_SEM_INVALID)
    {
        return osErrorParameter;
    }
    else
    {
        return osErrorResource;
    }
#endif
}

//Mutex Public API

/// Create and Initialize a Mutex object.
/// \param[in]     mutex_def     mutex definition referenced with \ref osMutex.
/// \return mutex ID for reference by other functions or NULL in case of error.
/// \note MUST REMAIN UNCHANGED: \b osMutexCreate shall be consistent in every CMSIS-RTOS.
osMutexId osMutexCreate (const osMutexDef_t *mutex_def)
{
#if (LOSCFG_BASE_IPC_MUX == YES)
    uint32_t  uwRet;
    uint32_t* MuxHandle;

    if(mutex_def == NULL)
    {
        return (osMutexId)NULL;
    }

    MuxHandle = (uint32_t*)(mutex_def->puwMuxHandle);
    uwRet =  LOS_MuxCreate (MuxHandle);

    if(uwRet == LOS_OK)
    {
        return (osMutexId)GET_MUX(*MuxHandle);
    }
    else
    {
        return (osMutexId)NULL;
    }
#endif
}

/// Wait until a Mutex becomes available.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
/// \param[in]     millisec      timeout value or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osMutexWait shall be consistent in every CMSIS-RTOS.
/*
osOK: the mutex has been obtain.
osErrorTimeoutResource: the mutex could not be obtained in the given time.
osErrorResource: the mutex could not be obtained when no timeout was specified.
osErrorParameter: the parameter mutex_id is incorrect.
osErrorISR: osMutexWait cannot be called from interrupt service routines.
*/
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_MUX == YES)
    uint32_t  uwRet;
    uint32_t  MutID;

    if (mutex_id == NULL)
    {
        return osErrorParameter;
    }

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    MutID = ((MUX_CB_S*)mutex_id)->ucMuxID;

    uwRet = LOS_MuxPend(MutID, LOS_MS2Tick(millisec));

    if(uwRet == LOS_OK)
    {
        return osOK;
    }
    else if(uwRet == LOS_ERRNO_MUX_TIMEOUT)
    {
        return osErrorTimeoutResource;
    }
    else if(uwRet == LOS_ERRNO_MUX_UNAVAILABLE)
    {
        return osErrorResource;
    }
    else if(uwRet == LOS_ERRNO_MUX_PEND_INTERR)
    {
        return osErrorISR;
    }
    else
    {
        return osErrorParameter;
    }
#endif
}

/// Release a Mutex that was obtained by \ref osMutexWait.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osMutexRelease shall be consistent in every CMSIS-RTOS.
/*
osOK: the mutex has been correctly released.
osErrorResource: the mutex was not obtained before.
osErrorParameter: the parameter mutex_id is incorrect.
osErrorISR: osMutexRelease cannot be called from interrupt service routines.        //
*/
osStatus osMutexRelease (osMutexId mutex_id)
{
#if (LOSCFG_BASE_IPC_MUX == YES)
    uint32_t  uwRet;
    uint32_t  MutID;


    if (mutex_id == NULL)
    {
        return osErrorParameter;
    }

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    MutID = ((MUX_CB_S*)mutex_id)->ucMuxID;
    uwRet = LOS_MuxPost(MutID);

    if(uwRet == LOS_OK)
    {
        return osOK;
    }
    else
    {
        return osErrorResource;
    }
#endif
}

/// Delete a Mutex that was created by \ref osMutexCreate.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osMutexDelete shall be consistent in every CMSIS-RTOS.
/*
osOK: the mutex object has been deleted.
osErrorISR: osMutexDelete cannot be called from interrupt service routines.    //osErrorISR
osErrorResource: all tokens have already been released.
osErrorParameter: the parameter mutex_id is incorrect.
*/
osStatus osMutexDelete (osMutexId mutex_id)
{
#if (LOSCFG_BASE_IPC_MUX == YES)
    uint32_t  uwRet;
    uint32_t  MutID;

    if (mutex_id == NULL)
    {
        return osErrorParameter;
    }

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    MutID = ((MUX_CB_S*)mutex_id)->ucMuxID;
    uwRet = LOS_MuxDelete(MutID);

    if(uwRet == LOS_OK)
    {
        return osOK;
    }
    else if(uwRet == LOS_ERRNO_MUX_INVALID)
    {
        return osErrorResource;
    }
    else
    {
        return osErrorParameter;
    }
#endif
}

osPoolId osPoolCreate (const osPoolDef_t *pool_def)
{
    uint32_t uwBlkSize, uwBoxSize;
    uint32_t uwRet;

    if ((pool_def == NULL) ||
        (pool_def->pool_sz == 0) ||
        (pool_def->item_sz == 0) ||
        (pool_def->pool == NULL)) {
        return (osPoolId)NULL;
    }

    uwBlkSize = (pool_def->item_sz + 3) & ~3;
    uwBoxSize = sizeof(OS_MEMBOX_S) + pool_def->pool_sz * uwBlkSize;
#if (LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK == YES)
    uwBoxSize = uwBoxSize + pool_def->pool_sz * LOS_MEMBOX_MAGIC_SIZE;
#endif

    uwRet = LOS_MemboxInit(pool_def->pool, uwBoxSize, uwBlkSize);
    if(uwRet != LOS_OK)
    {
        return (osPoolId)NULL;
    }

    return (osPoolId)(pool_def->pool);
}

void *osPoolAlloc (osPoolId pool_id)
{
    void *ptr;

    if (pool_id == NULL)
        return NULL;

    ptr = LOS_MemboxAlloc(pool_id);

    return ptr;
}


void *osPoolCAlloc (osPoolId pool_id)
{
    void *ptr;

    if (pool_id == NULL)
        return NULL;

    ptr = LOS_MemboxAlloc(pool_id);

    LOS_MemboxClr(pool_id, ptr);

    return ptr;
}


osStatus osPoolFree (osPoolId pool_id, void *block)
{
    int32_t res;

    if (pool_id == NULL)
        return osErrorParameter;

    res = LOS_MemboxFree(pool_id, block);

    if (res != 0)
        return osErrorValue;

    return osOK;
}

// Message Queue Management Public API

/// Create and Initialize Message Queue
osMessageQId osMessageCreate(osMessageQDef_t *queue_def, osThreadId thread_id)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwQueueID;
    uint32_t uwRet;

    (void)(thread_id);
    if (NULL == queue_def)
    {
        return (osMessageQId)NULL;
    }
    uwRet = LOS_QueueCreate((char *)NULL, (uint16_t)(queue_def->queue_sz), &uwQueueID, 0,(uint16_t)( queue_def->item_sz));
    if (uwRet == LOS_OK)
    {
        return (osMessageQId)uwQueueID;
    }
    else
    {
        return (osMessageQId)NULL;
    }
#endif
}

/// Put a Message to a Queue
osStatus osMessagePut(const osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwRet;

    uwRet = LOS_QueueWrite((uint32_t)queue_id, (void*)info, sizeof(uint32_t), LOS_MS2Tick(millisec));
    if (uwRet == LOS_OK)
    {
        uwRet = osOK;
    }
    else if(uwRet == LOS_ERRNO_QUEUE_WRITE_INVALID || uwRet == LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT)
    {
        uwRet = osErrorParameter;
    }
    else if (uwRet == LOS_ERRNO_QUEUE_TIMEOUT || uwRet == LOS_ERRNO_QUEUE_ISFULL)
    {
        uwRet = osEventTimeout;
    }
    else
    {
        uwRet = osErrorOS;
    }
    return (osStatus)uwRet;
#endif
}

/// Get a Message or Wait for a Message from a Queue
osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwRet;
    osEvent ret;

    memset(&ret, 0, sizeof(osEvent));
    uwRet = LOS_QueueRead((uint32_t)queue_id, &(ret.value.v), sizeof(uint32_t), LOS_MS2Tick(millisec));
    if (uwRet == LOS_OK)
    {
        ret.status = osEventMessage;
    }
    else if (uwRet == LOS_ERRNO_QUEUE_READ_INVALID || uwRet == LOS_ERRNO_QUEUE_READ_IN_INTERRUPT)
    {
        ret.status = osErrorParameter;
    }
    else if (uwRet == LOS_ERRNO_QUEUE_ISEMPTY || uwRet == LOS_ERRNO_QUEUE_TIMEOUT)
    {
        ret.status = osEventTimeout;
    }
    else
    {
        ret.status = osErrorOS;
    }

    return ret;
#endif
}


// Mail Queue Management Public API

/// Create and Initialize mail queue
osMailQId osMailCreate(osMailQDef_t *queue_def, osThreadId thread_id)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwRet;
    uint32_t uwQueueID;
    uint32_t uwBlkSize, uwBoxSize;

    (void)(thread_id);
    if (NULL == queue_def)
    {
        return (osMailQId)NULL;
    }
    uwRet = LOS_QueueCreate((char *)NULL, (uint16_t)(queue_def->queue_sz), &uwQueueID, 0, sizeof(uint32_t));
    if (uwRet == LOS_OK)
    {
        *(uint32_t*)(((void **)queue_def->pool) + 0) = uwQueueID;
        uwBlkSize = (queue_def->item_sz + 3) & (~3);
        uwBoxSize = sizeof(OS_MEMBOX_S) + queue_def->queue_sz * uwBlkSize;
    #if (LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK == YES)
        uwBoxSize = uwBoxSize + queue_def->queue_sz * LOS_MEMBOX_MAGIC_SIZE;
    #endif
        (void)LOS_MemboxInit(*(((void **)queue_def->pool) + 1), uwBoxSize, uwBlkSize);
        return (osMailQId)queue_def->pool;
    }
    return (osMailQId)NULL;
#endif
}

/// Allocate a memory block from a mail
void *osMailAlloc(osMailQId queue_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    void *pool = NULL;
    uint32_t uwQueueID;

    if (queue_id == NULL)
    {
        return NULL;
    }

    uwQueueID = *((uint32_t*)(((void **)queue_id) + 0));
    pool = *((((void **)queue_id) + 1));

    return (void *)osQueueMailAlloc(uwQueueID, pool, LOS_MS2Tick(millisec));
#endif
}

/// Allocate a memory block from a mail and set memory block to zero
void *osMailCAlloc(osMailQId queue_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    void *mem = NULL;
    OS_MEMBOX_S *pool = (OS_MEMBOX_S *)NULL;
    mem = osMailAlloc(queue_id, millisec);

    if (mem != NULL)
    {
        pool = (OS_MEMBOX_S*)(*(((void **)queue_id) + 1));
        memset(mem, 0, pool->uwBlkSize - LOS_MEMBOX_MAGIC_SIZE);
    }

    return mem;
#endif
}

/// Free a memory block from a mail
osStatus osMailFree(osMailQId queue_id, void *mail)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    void *pool = NULL;
    uint32_t uwQueueID;
    uint32_t uwRet;

    if (queue_id == NULL)
    {
        return osErrorParameter;
    }

    uwQueueID = *((uint32_t*)(((void **)queue_id) + 0));
    pool = *((((void **)queue_id) + 1));

    uwRet = osQueueMailFree(uwQueueID, pool, mail);
    if (uwRet == LOS_ERRNO_QUEUE_MAIL_HANDLE_INVALID || uwRet == LOS_ERRNO_QUEUE_MAIL_PTR_INVALID)
    {
        return osErrorParameter;
    }
    else if (uwRet == LOS_ERRNO_QUEUE_MAIL_FREE_ERROR)
    {
        return osErrorOS;
    }
    return osOK;
#endif
}

/// Put a mail to a queue
osStatus osMailPut(osMailQId queue_id, void *mail)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwQueueID;

    if (queue_id == NULL)
    {
        return osErrorParameter;
    }

    if (mail == NULL)
    {
        return osErrorValue;
    }

    uwQueueID = *((uint32_t*)(((void **)queue_id) + 0));

    return osMessagePut((osMessageQId)uwQueueID, (uint32_t)mail, 0);
#endif
}

/// Get a mail from a queue
osEvent osMailGet(osMailQId queue_id, uint32_t millisec)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    uint32_t uwQueueID;
    osEvent ret;

    if (queue_id == NULL)
    {
        ret.status = osErrorParameter;
        return ret;
    }

    uwQueueID = *((uint32_t*)(((void **)queue_id) + 0));
    ret = osMessageGet((osMessageQId)uwQueueID, millisec);

    if (ret.status == osEventMessage)
    {
        ret.status = osEventMail;
    }
    return ret;
#endif
}

osStatus osMailClear(osMailQId queue_id)
{
#if (LOSCFG_BASE_IPC_QUEUE == YES)
    osEvent evt;
#if (__CORTEX_M != 2)
    __disable_irq();
#endif
    while(1)
    {
        evt = osMailGet(queue_id, 0);
        if(evt.status == osEventMail)
        {
            (void)osMailFree(queue_id,evt.value.p);
        }
        else if(evt.status == osEventTimeout)
        {
#if (__CORTEX_M != 2)		
            __enable_irq();
#endif					
            return osOK;
        }
        else
        {
#if (__CORTEX_M != 2)						
            __enable_irq();
#endif					
            return evt.status;
        }
    }
#endif
}

int32_t osSignalSet (osThreadId thread_id, int32_t signals)
{
    EVENT_CB_S sig;
    uint32_t old_sig;
    uint32_t uwRet;

    if (((LOS_TASK_CB *)thread_id) == NULL)
    {
        return 0x80000000;/*lint !e569*/
    }

    if (signals & (~((0x1 << osFeature_Signals)-1)))
    {
        return osErrorValue;
    }

    sig = ((LOS_TASK_CB *)thread_id)->uwEvent;
    old_sig = sig.uwEventID;
    if (sig.uwEventID == 0xFFFFFFFF)
    {
        uwRet = LOS_EventInit(&(((LOS_TASK_CB *)thread_id)->uwEvent));
        if (uwRet != LOS_OK)
        {
            return osErrorOS;
        }
    }
    uwRet = LOS_EventWrite(&(((LOS_TASK_CB *)thread_id)->uwEvent), signals);
    if (uwRet != LOS_OK)
    {
        return osErrorOS;
    }

    return old_sig;
}

int32_t osSignalClear (osThreadId thread_id, int32_t signals)
{
    EVENT_CB_S sig;
    uint32_t old_sig;
    uint32_t uwRet;

    if (((LOS_TASK_CB *)thread_id) == NULL)
    {
        return 0x80000000; /*lint !e569*/
    }

    if (signals & (~((0x1 << osFeature_Signals)-1)))
    {
        return osErrorValue;
    }

    sig = ((LOS_TASK_CB *)thread_id)->uwEvent;
    old_sig = sig.uwEventID;
    uwRet = LOS_EventClear(&(((LOS_TASK_CB *)thread_id)->uwEvent), ~(uint32_t)signals);
    if (uwRet != LOS_OK)
    {
        return osErrorValue;
    }

    return old_sig;
}

osEvent osSignalWait (int32_t signals, uint32_t millisec)
{
    uint32_t uwRet = 0;
    osEvent ret;
    uint32_t uwFlags = 0;
    uint32_t uwTimeOut = osWaitForever;
    EVENT_CB_S sig;
    LOS_TASK_CB  *pstRunTsk;

    if (OS_INT_ACTIVE)
    {
        /* Not allowed in ISR */
        ret.status = osErrorISR;
        return ret;
    }

    if (signals & (~((0x1 << osFeature_Signals)-1)))
    {
        ret.status = osErrorValue;
        return ret;
    }

    if (signals != 0)
    {
        uwFlags |= LOS_WAITMODE_AND;
    }
    else
    {
        signals = 0xFFFFFFFF & ((0x1 << osFeature_Signals)-1);
        uwFlags |= LOS_WAITMODE_OR;
    }

    uwTimeOut = LOS_MS2Tick(millisec);

    pstRunTsk = g_stLosTask.pstRunTask;
    sig = ((LOS_TASK_CB *)pstRunTsk)->uwEvent;
    if (sig.uwEventID == 0xFFFFFFFF)
    {
        uwRet = LOS_EventInit(&(((LOS_TASK_CB *)(g_stLosTask.pstRunTask))->uwEvent));
        if (uwRet != LOS_OK)
        {
            ret.status = osErrorOS;
            return ret;
        }
    }
    uwRet = LOS_EventRead(&(((LOS_TASK_CB *)(g_stLosTask.pstRunTask))->uwEvent), signals, uwFlags | LOS_WAITMODE_CLR, uwTimeOut);
    if (uwRet == LOS_ERRNO_EVENT_READ_TIMEOUT)
    {
        ret.status = osEventTimeout;
        ret.value.signals = 0;
    }
    else if (uwRet == 0)
    {
        ret.status = osOK;
        ret.value.signals = 0;
    }
    else if(uwRet == LOS_ERRNO_EVENT_PTR_NULL ||
            uwRet == LOS_ERRNO_EVENT_EVENTMASK_INVALID ||
            uwRet == LOS_ERRNO_EVENT_READ_IN_LOCK ||
            uwRet == LOS_ERRNO_EVENT_READ_IN_INTERRUPT)
    {
        ret.status = osErrorOS;
        ret.value.signals = 0;
    }
    else
    {
        ret.status = osEventSignal;
        ret.value.signals = uwRet;
    }

    return ret;
}

osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument)
{
    SWTMR_CTRL_S *pstSwtmr = (SWTMR_CTRL_S *)NULL;
#if (LOSCFG_BASE_CORE_SWTMR == YES)
    uint32_t uwRet;
    uint16_t usSwTmrID;

    if ((timer_def == NULL)
        || (timer_def->ptimer == NULL)
        || (timer_def->default_interval == 0)
        || ((type != osTimerOnce) && (type != osTimerPeriodic)))
    {
        return (osTimerId)NULL;
    }

    uwRet = LOS_SwtmrCreate(timer_def->default_interval, type,
                            (SWTMR_PROC_FUNC)(timer_def->ptimer),
                            &usSwTmrID, (uint32_t)argument);

    if (uwRet != LOS_OK)
    {
        return (osTimerId)NULL;
    }

    pstSwtmr = OS_SWT_FROM_SID(usSwTmrID);
#endif
    return pstSwtmr;
}

osStatus osTimerStart (osTimerId timer_id, uint32_t millisec)
{
#if (LOSCFG_BASE_CORE_SWTMR == YES)
    SWTMR_CTRL_S  *pstSwtmr;
    uint32_t   uwInterval;

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    pstSwtmr = (SWTMR_CTRL_S *)timer_id;
    if (pstSwtmr == NULL)
    {
        return osErrorParameter;
    }

    uwInterval = LOS_MS2Tick(millisec);
    if (uwInterval == 0)
    {
        return osErrorValue;
    }

    pstSwtmr->uwInterval = uwInterval;
    pstSwtmr->uwExpiry   = uwInterval;
    if (LOS_SwtmrStart(pstSwtmr->usTimerID))
    {
        return osErrorResource;
    }
#endif
    return osOK;
}

osStatus osTimerStop (osTimerId timer_id)
{
#if (LOSCFG_BASE_CORE_SWTMR == YES)
    SWTMR_CTRL_S  *pstSwtmr;

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    pstSwtmr = (SWTMR_CTRL_S *)timer_id;
    if (pstSwtmr == NULL)
    {
        return osErrorParameter;
    }

    if (LOS_SwtmrStop(pstSwtmr->usTimerID))
    {
        return osErrorResource;
    }
#endif
    return osOK;
}

osStatus osTimerDelete (osTimerId timer_id)
{
#if (LOSCFG_BASE_CORE_SWTMR == YES)
    SWTMR_CTRL_S  *pstSwtmr;

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    pstSwtmr = (SWTMR_CTRL_S *)timer_id;
    if (pstSwtmr == NULL)
    {
        return osErrorParameter;
    }

    if (LOS_SwtmrDelete(pstSwtmr->usTimerID))
    {
        return osErrorResource;
    }
#endif
    return osOK;

}

osStatus osDelay (uint32_t millisec)
{
    uint32_t   uwInterval;
    uint32_t   uwRet = 0;

    if (OS_INT_ACTIVE)
    {
        return osErrorISR;
    }

    if (millisec == 0)
    {
        return osOK;
    }

    uwInterval = LOS_MS2Tick(millisec);

    uwRet = LOS_TaskDelay(uwInterval);

    if (uwRet == LOS_OK)
    {
        return osEventTimeout;
    }
    else
    {
        return osErrorResource;
    }
}

#if (defined (osFeature_Wait)  &&  (osFeature_Wait != 0))
osEvent osWait (uint32_t millisec)
{
    osEvent evt;
    uint32_t   uwInterval;
    uint32_t   uwRet = 0;

    if (OS_INT_ACTIVE)
    {
        evt.status = osErrorISR;
        return evt;
    }

    if (millisec == 0)
    {
        evt.status = osOK;
        return evt;
    }

    /* TODO: osEventSignal, osEventMessage, osEventMail */
    uwInterval = LOS_MS2Tick(millisec);

    uwRet = LOS_TaskDelay(uwInterval);

    if (uwRet == LOS_OK)
    {
        evt.status = osEventTimeout;
    }
    else
    {
        evt.status = osErrorResource;
    }

    return evt;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
