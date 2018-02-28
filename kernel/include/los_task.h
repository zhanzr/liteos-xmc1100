
/**@defgroup los_task Task
 * @ingroup kernel
 */

#ifndef _LOS_TASK_H
#define _LOS_TASK_H

#include "los_base.h"
#include "los_list.h"
#include "los_sys.h"
#include "los_tick.h"
#include "los_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task control block is unused.
 */
#define OS_TASK_STATUS_UNUSED                       0x0001

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is suspended.
 */
#define OS_TASK_STATUS_SUSPEND                      0x0002

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is ready.
 */
#define OS_TASK_STATUS_READY                        0x0004

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is blocked.
 */
#define OS_TASK_STATUS_PEND                         0x0008

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is running.
 */
#define OS_TASK_STATUS_RUNNING                      0x0010

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is delayed.
 */
#define OS_TASK_STATUS_DELAY                        0x0020

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The time for waiting for an event to occur expires.
 */
#define OS_TASK_STATUS_TIMEOUT                      0x0040

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is automatically deleted.
 */
#define OS_TASK_STATUS_DETACHED                     0x0080

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is waiting for an event to occur.
 */
#define OS_TASK_STATUS_EVENT                        0x0400

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is reading an event.
 */
#define OS_TASK_STATUS_EVENT_READ                   0x0800

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * A software timer is waiting for an event to occur.
 */
#define OS_TASK_STATUS_SWTMR_WAIT                   0x1000

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is blocked on a queue.
 */
#define OS_TASK_STATUS_PEND_QUEUE                   0x2000

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is blocked on a mutex.
 */
#define OS_TASK_STATUS_PEND_MUT                     0x4000

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is blocked on a semaphore.
 */
#define OS_TASK_STATUS_PEND_SEM                     0x8000

/**
 * @ingroup los_task
 * Boundary on which the stack size is aligned.
 *
 */
#define OS_TASK_STACK_SIZE_ALIGN                    16

/**
 * @ingroup los_task
 * Boundary on which the stack address is aligned.
 *
 */
#define OS_TASK_STACK_ADDR_ALIGN                    8

/**
 * @ingroup los_task
 * Task stack top magic number.
 *
 */
#define OS_TASK_MAGIC_WORD                          0xCCCCCCCC

/**
 * @ingroup los_task
 * Initial task stack value.
 *
 */
#define OS_TASK_STACK_INIT                          0xCACACACA

/**
 * @ingroup los_task
 * Number of usable task priorities.
 */
#define OS_TSK_PRINUM                               (OS_TASK_PRIORITY_LOWEST - OS_TASK_PRIORITY_HIGHEST + 1)

/**
* @ingroup  los_task
* @brief Check whether a task ID is valid.
*
* @par Description:
* This API is used to check whether a task ID, excluding the idle task ID, is valid.
* @attention None.
*
* @param  uwTaskID [IN] Task ID.
*
* @retval 0 or 1. One indicates that the task ID is invalid, whereas zero indicates that the task ID is valid.
* @par Dependency:
* <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
* @see
* @since Huawei LiteOS V100R001C00
*/
#define OS_TSK_GET_INDEX(uwTaskID)                      (uwTaskID)

/**
* @ingroup  los_task
* @brief Obtain the pointer to a task control block.
*
* @par Description:
* This API is used to obtain the pointer to a task control block using a corresponding parameter.
* @attention None.
*
* @param  ptr [IN] Parameter used for obtaining the task control block.
*
* @retval Pointer to the task control block.
* @par Dependency:
* <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
* @see
* @since Huawei LiteOS V100R001C00
*/
#define OS_TCB_FROM_PENDLIST(ptr)                       LOS_DL_LIST_ENTRY(ptr, LOS_TASK_CB, stPendList)

/**
* @ingroup  los_task
* @brief Obtain the pointer to a task control block.
*
* @par Description:
* This API is used to obtain the pointer to a task control block that has a specified task ID.
* @attention None.
*
* @param  TaskID [IN] Task ID.
*
* @retval Pointer to the task control block.
* @par Dependency:
* <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
* @see
* @since Huawei LiteOS V100R001C00
*/
#define OS_TCB_FROM_TID(TaskID)                       (((LOS_TASK_CB *)g_pstTaskCBArray) + (TaskID))

/**
* @ingroup  los_task
* @brief Define the type of a task entrance function.
*
* @par Description:
* This API is used to define the type of a task entrance function and call it after a task is created and triggered.
* @attention None.
*
* @param  uwParam1 [IN] Type #uint32_t The first parameter passed to the task handling function.
* @param  uwParam2 [IN] Type #uint32_t The second parameter passed to the task handling function.
* @param  uwParam3 [IN] Type #uint32_t The third parameter passed to the task handling function.
* @param  uwParam4 [IN] Type #uint32_t The fourth parameter passed to the task handling function.
*
* @retval None.
* @par Dependency:
* <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
* @see
* @since Huawei LiteOS V100R001C00
*/
typedef void *(*TSK_ENTRY_FUNC)(uint32_t uwParam1,
                        uint32_t uwParam2,
                        uint32_t uwParam3,
                        uint32_t uwParam4);

/**
 * @ingroup los_task
 * Define the task control block structure.
 */
typedef struct tagTaskCB
{
    void                        *pStackPointer;             /**< Task stack pointer                 */
    uint16_t                      usTaskStatus;
    uint16_t                      usPriority;
    uint32_t                      uwStackSize;                /**< Task stack size                 */
    uint32_t                      uwTopOfStack;               /**< Task stack top               */
    uint32_t                      uwTaskID;                   /**< Task ID                     */
    TSK_ENTRY_FUNC              pfnTaskEntry;               /**< Task entrance function               */
    void                        *pTaskSem;                  /**< Task-held semaphore           */
    void                        *pThreadJoin;               /**< pthread adaption            */
    void                        *pThreadJoinRetval;         /**< pthread adaption            */
    void                        *pTaskMux;                  /**< Task-held mutex           */
    uint32_t                      auwArgs[4];                 /**< Parameter, of which the maximum number is 4          */
    char                        *pcTaskName;                /**< Task name                     */
    LOS_DL_LIST                 stPendList;
    LOS_DL_LIST                 stTimerList;
    uint32_t                      uwIdxRollNum;
    EVENT_CB_S                  uwEvent;
    uint32_t                      uwEventMask;                /**< Event mask               */
    uint32_t                      uwEventMode;                /**< Event mode               */
    void                        *puwMsg;                    /**< Memory allocated to queues          */
} LOS_TASK_CB;

typedef struct stLosTask
{
    LOS_TASK_CB   *pstRunTask;
    LOS_TASK_CB   *pstNewTask;
} ST_LOS_TASK;

extern ST_LOS_TASK          g_stLosTask;

/**
 * @ingroup los_task
 * Task lock flag.
 *
 */
extern uint16_t               g_usLosTaskLock;

/**
 * @ingroup los_task
 * Maximum number of tasks.
 *
 */
extern uint32_t               g_uwTskMaxNum;

/**
 * @ingroup los_task
 * Idle task ID.
 *
 */
extern uint32_t               g_uwIdleTaskID;

/**
 * @ingroup los_task
 * Software timer task ID.
 *
 */
extern uint32_t               g_uwSwtmrTaskID;

/**
 * @ingroup los_task
 * Starting address of a task.
 *
 */
extern LOS_TASK_CB          *g_pstTaskCBArray;

/**
 * @ingroup los_task
 * Delayed task linked list.
 *
 */
extern LOS_DL_LIST          g_stTaskTimerList;

/**
 * @ingroup los_task
 * Free task linked list.
 *
 */
extern LOS_DL_LIST          g_stLosFreeTask;

/**
 * @ingroup los_task
 * Circular linked list that stores tasks that are deleted automatically.
 *
 */
extern LOS_DL_LIST          g_stTskRecyleList;

/**
 * @ingroup los_task
 * Time slice structure.
 */
typedef struct tagTaskTimeSlice
{
    LOS_TASK_CB             *pstTask;                       /**< Current running task   */
    uint16_t                  usTime;                         /**< Expiration time point           */
    uint16_t                  usTout;                         /**< Expiration duration             */
} OS_TASK_ROBIN_S;

extern void osTaskSchedule(void);
extern void osTaskScan(void);
extern void osIdleTask(void);
extern uint32_t osIdleTaskCreate(void);
extern uint32_t osTaskInit(void);

/**
 * @ingroup  los_task
 * @brief Modify the priority of task.
 *
 * @par Description:
 * This API is used to modify the priority of task.
 *
 * @attention
 * <ul>
 * <li>The pstTaskCB should be a correct pointer to task control block structure.</li>
 * <li>the usPriority should be in [0, OS_TASK_PRIORITY_LOWEST].</li>
 * </ul>
 *
 * @param  pstTaskCB [IN] Type #LOS_TASK_CB * pointer to task control block structure.
 * @param  usPriority  [IN] Type #uint16_t the priority of task.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
extern void osTaskPriModify(LOS_TASK_CB *pstTaskCB, uint16_t usPriority);

/**
 * @ingroup  los_task
 * @brief Add task to sorted delay list.
 *
 * @par Description:
 * This API is used to add task to sorted delay list.
 *
 * @attention
 * <ul>
 * <li>The pstTaskCB should be a correct pointer to task control block structure.</li>
 * </ul>
 *
 * @param  pstTaskCB [IN] Type #LOS_TASK_CB * pointer to task control block structure.
 * @param  uwTimeout  [IN] Type #uint32_t wait time, ticks.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see osTimerListDelete
 * @since Huawei LiteOS V100R001C00
 */
extern void osTaskAdd2TimerList(LOS_TASK_CB *pstTaskCB, uint32_t uwTimeout);

/**
 * @ingroup  los_task
 * @brief delete task from sorted delay list.
 *
 * @par Description:
 * This API is used to delete task from sorted delay list.
 *
 * @attention
 * <ul>
 * <li>The pstTaskCB should be a correct pointer to task control block structure.</li>
 * </ul>
 *
 * @param  pstTaskCB [IN] Type #LOS_TASK_CB * pointer to task control block structure.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see osTaskAdd2TimerList
 * @since Huawei LiteOS V100R001C00
 */
extern void osTimerListDelete(LOS_TASK_CB *pstTaskCB);
extern void osTaskEntry(uint32_t uwTaskID);

/**
 * @ingroup los_task
 * Flag that indicates the task or task control block status.
 *
 * The task is automatically deleted.
 */
#define LOS_TASK_STATUS_DETACHED                    0x0080

/**
 * @ingroup los_task
 * Task error code: Insufficient memory for task creation.
 *
 * Value: 0x03000200
 *
 * Solution: Allocate bigger memory partition to task creation.
 */
#define LOS_ERRNO_TSK_NO_MEMORY                     LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x00)

/**
 * @ingroup los_task
 * Task error code: Null parameter.
 *
 * Value: 0x02000201
 *
 * Solution: Check the parameter.
 */
#define LOS_ERRNO_TSK_PTR_NULL                      LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x01)

/**
 * @ingroup los_task
 * Task error code: The task stack is not aligned.
 *
 * Value: 0x02000202
 *
 * Solution: Align the task stack.
 */
#define LOS_ERRNO_TSK_STKSZ_NOT_ALIGN               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x02)

/**
 * @ingroup los_task
 * Task error code: Incorrect task priority.
 *
 * Value: 0x02000203
 *
 * Solution: Re-configure the task priority by referring to the priority range.
 */
#define LOS_ERRNO_TSK_PRIOR_ERROR                   LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x03)

/**
 * @ingroup los_task
 * Task error code: The task entrance is NULL.
 *
 * Value: 0x02000204
 *
 * Solution: Define the task entrance function.
 */
#define LOS_ERRNO_TSK_ENTRY_NULL                    LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x04)

/**
 * @ingroup los_task
 * Task error code: The task name is NULL.
 *
 * Value: 0x02000205
 *
 * Solution: Set the task name.
 */
#define LOS_ERRNO_TSK_NAME_EMPTY                    LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x05)

/**
 * @ingroup los_task
 * Task error code: The task stack size is too small.
 *
 * Value: 0x02000206
 *
 * Solution: Expand the task stack.
 */
#define LOS_ERRNO_TSK_STKSZ_TOO_SMALL               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x06)

/**
 * @ingroup los_task
 * Task error code: Invalid task ID.
 *
 * Value: 0x02000207
 *
 * Solution: Check the task ID.
 */
#define LOS_ERRNO_TSK_ID_INVALID                    LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x07)

/**
 * @ingroup los_task
 * Task error code: The task is already suspended.
 *
 * Value: 0x02000208
 *
 * Solution: Suspend the task after it is resumed.
 */
#define LOS_ERRNO_TSK_ALREADY_SUSPENDED             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x08)

/**
 * @ingroup los_task
 * Task error code: The task is not suspended.
 *
 * Value: 0x02000209
 *
 * Solution: Suspend the task.
 */
#define LOS_ERRNO_TSK_NOT_SUSPENDED                 LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x09)

/**
 * @ingroup los_task
 * Task error code: The task is not created.
 *
 * Value: 0x0200020a
 *
 * Solution: Create the task.
 */
#define LOS_ERRNO_TSK_NOT_CREATED                   LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0a)

/**
 * @ingroup los_task
 * Task error code: The task is locked when it is being deleted.
 *
 * Value: 0x0300020b
 *
 * Solution: Unlock the task.
 */
#define LOS_ERRNO_TSK_DELETE_LOCKED                 LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x0b)

/**
 * @ingroup los_task
 * Task error code: The task message is nonzero.
 *
 * Value: 0x0200020c
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_MSG_NONZERO                   LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0c)

/**
 * @ingroup los_task
 * Task error code: The task delay occurs during an interrupt.
 *
 * Value: 0x0300020d
 *
 * Solution: Perform this operation after exiting from the interrupt.
 */
#define LOS_ERRNO_TSK_DELAY_IN_INT                  LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x0d)

/**
 * @ingroup los_task
 * Task error code: The task delay occurs when the task is locked.
 *
 * Value: 0x0200020e
 *
 * Solution: Perform this operation after unlocking the task.
 */
#define LOS_ERRNO_TSK_DELAY_IN_LOCK                 LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0e)

/**
 * @ingroup los_task
 * Task error code: The task that is being scheduled is invalid.
 *
 * Value: 0x0200020f
 *
 * Solution: Check the task.
 */
#define LOS_ERRNO_TSK_YIELD_INVALID_TASK            LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0f)

/**
 * @ingroup los_task
 * Task error code: Only one task or no task is available for scheduling.
 *
 * Value: 0x02000210
 *
 * Solution: Increase the number of tasks.
 */
#define LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK         LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x10)

/**
 * @ingroup los_task
 * Task error code: No free task control block is available.
 *
 * Value: 0x02000211
 *
 * Solution: Increase the number of task control blocks.
 */
#define LOS_ERRNO_TSK_TCB_UNAVAILABLE               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x11)

/**
 * @ingroup los_task
 * Task error code: The task hook function is not matchable.
 *
 * Value: 0x02000212
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_HOOK_NOT_MATCH                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x12)

/**
 * @ingroup los_task
 * Task error code: The number of task hook functions exceeds the permitted upper limit.
 *
 * Value: 0x02000213
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_HOOK_IS_FULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x13)

/**
 * @ingroup los_task
 * Task error code: The operation is performed on the idle task.
 *
 * Value: 0x02000214
 *
 * Solution: Check the task ID and do not operate on the idle task.
 */
#define LOS_ERRNO_TSK_OPERATE_IDLE                  LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x14)

/**
 * @ingroup los_task
 * Task error code: The task that is being suspended is locked.
 *
 * Value: 0x03000215
 *
 * Solution: Check the task ID and do not operate on the idle task.
 */
#define LOS_ERRNO_TSK_SUSPEND_LOCKED                LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x15)

/**
 * @ingroup los_task
 * Task error code: The task stack fails to be freed.
 *
 * Value: 0x02000217
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_FREE_STACK_FAILED             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x17)

/**
 * @ingroup los_task
 * Task error code: The task stack area is too small.
 *
 * Value: 0x02000218
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_STKAREA_TOO_SMALL             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x18)

/**
 * @ingroup los_task
 * Task error code: The task fails to be activated.
 *
 * Value: 0x03000219
 *
 * Solution: Perform task switching after creating an idle task.
 */
#define LOS_ERRNO_TSK_ACTIVE_FAILED                 LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x19)

/**
 * @ingroup los_task
 * Task error code: Too many task configuration items.
 *
 * Value: 0x0200021a
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_CONFIG_TOO_MANY               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1a)

/**
 * @ingroup los_task
 * Task error code:
 *
 * Value: 0x0200021b
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_CP_SAVE_AREA_NOT_ALIGN        LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1b)

/**
 * @ingroup los_task
 * Task error code:
 *
 * Value: 0x0200021d
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_MSG_Q_TOO_MANY                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1d)

/**
 * @ingroup los_task
 * Task error code:
 *
 * Value: 0x0200021e
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_CP_SAVE_AREA_NULL             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1e)

/**
 * @ingroup los_task
 * Task error code:
 *
 * Value: 0x0200021f
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_TSK_SELF_DELETE_ERR               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1f)

/**
 * @ingroup los_task
 * Task error code: The task stack size is too large.
 *
 * Value: 0x02000220
 *
 * Solution: shrink the task stack size parameter.
 */
#define LOS_ERRNO_TSK_STKSZ_TOO_LARGE               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x20)

/**
 * @ingroup los_task
 * Task error code: Suspending software timer task is not allowed.
 *
 * Value: 0x02000221
 *
 * Solution: Check the task ID and do not suspend software timer task.
 */
#define LOS_ERRNO_TSK_SUSPEND_SWTMR_NOT_ALLOWED                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x21)

/**
 * @ingroup los_task
 * Define the type of the task switching hook function.
 *
 */
typedef void (*TSKSWITCHHOOK)(void);

/**
 * @ingroup los_task
 * User task switching hook function.
 *
 */
extern TSKSWITCHHOOK g_pfnUsrTskSwitchHook;

/**
 * @ingroup los_task
 * Define the structure of the parameters used for task creation.
 *
 * Information of specified parameters passed in during task creation.
 */
typedef struct tagTskInitParam
{
   TSK_ENTRY_FUNC       pfnTaskEntry;               /**< Task entrance function               */
   uint16_t               usTaskPrio;                 /**< Task priority                 */
   uint32_t               auwArgs[4];                 /**< Task parameters, of which the maximum number is four          */
   uint32_t               uwStackSize;                /**< Task stack size               */
   char                 *pcName;                    /**< Task name                     */
   uint32_t               uwResved;                   /**< Reserved. It is automatically deleted if set to LOS_TASK_STATUS_DETACHED. It is unable to be deleted if set to 0.*/
} TSK_INIT_PARAM_S;

/**
 * @ingroup los_task
 * Task name length
 *
 */
#define LOS_TASK_NAMELEN                            32

/**
 * @ingroup los_task
 * Task information structure.
 *
 */
typedef struct tagTskInfo
{
    char                acName[LOS_TASK_NAMELEN];   /**< Task entrance function               */
    uint32_t              uwTaskID;                   /**< Task ID                     */
    uint16_t              usTaskStatus;               /**< Task status                   */
    uint16_t              usTaskPrio;                 /**< Task priority                 */
    void                *pTaskSem;                  /**< Semaphore pointer             */
    void                *pTaskMux;                  /**< Mutex pointer             */
    EVENT_CB_S          uwEvent;                    /**< Event                   */
    uint32_t              uwEventMask;                /**< Event mask               */
    uint32_t              uwStackSize;                /**< Task stack size                 */
    uint32_t              uwTopOfStack;               /**< Task stack top                   */
    uint32_t              uwBottomOfStack;            /**< Task stack bottom                   */
    uint32_t              uwSP;                       /**< Task SP pointer                 */
    uint32_t              uwCurrUsed;                 /**< Current task stack usage         */
    uint32_t              uwPeakUsed;                 /**< Task stack usage peak             */
    bool                bOvf;                       /**< Flag that indicates whether a task stack overflow occurs         */
} TSK_INFO_S;

/**
 * @ingroup  los_task
 * @brief Create a task and suspend.
 *
 * @par Description:
 * This API is used to create a task and suspend it. This task will not added to the queue of ready tasks before resume it.
 *
 * @attention
 * <ul>
 * <li>During task creation, the task control block and task stack of the task that is previously automatically deleted are deallocated.</li>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #TASK_DEFAULT_STACK_SIZE to specify the default task stack size.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * </ul>
 *
 * @param  puwTaskID   [OUT] Type #uint32_t * Task ID.
 * @param  pstInitParam [IN] Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 *
 * @retval #OS_ERROR                                 -1:        The task fails to be created.
 * @retval #OS_OK                                   0:         The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskDelete
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskCreateOnly(uint32_t *puwTaskID, TSK_INIT_PARAM_S *pstInitParam);

/**
 * @ingroup  los_task
 * @brief Create a task.
 *
 * @par Description:
 * This API is used to create a task. If the priority of the task created after the system is initialized is higher than the current task and the created task is not locked, it is scheduled for running.
 * If not, the created task is added to the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>During task creation, the task control block and task stack of the task that is previously automatically deleted are deallocated.</li>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #TASK_DEFAULT_STACK_SIZE to specify the default task stack size.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * </ul>
 *
 * @param  puwTaskID   [OUT] Type #uint32_t * Task ID.
 * @param  pstInitParam [IN] Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID             0x02000207:invalid Task ID
 * @retval #return LOS_ERRNO_TSK_PTR_NULL      0x02000201:Null parameter.
 * @retval #LOS_ERRNO_TSK_NAME_EMPTY           0x02000205:The task name is NULL.
 * @retval #LOS_ERRNO_TSK_ENTRY_NULL            0x02000204:The task entrance is NULL.
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR          0x02000203:Incorrect task priority.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_LARGE   0x02000220:The task stack size is too large.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_SMALL   0x02000206:The task stack size is too small.
 * @retval #LOS_ERRNO_TSK_TCB_UNAVAILABLE    0x02000211:No free task control block is available.
 * @retval #LOS_ERRNO_TSK_NO_MEMORY            0x03000200:Insufficient memory for task creation.
 * @retval #OS_OK                                   0:         The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskDelete
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskCreate(uint32_t *puwTaskID, TSK_INIT_PARAM_S *pstInitParam);

/**
 * @ingroup  los_task
 * @brief Resume a task.
 *
 * @par Description:
 * This API is used to resume a suspended task.
 *
 * @attention
 * <ul>
 * <li>If the task is delayed or blocked, resume the task without adding it to the queue of ready tasks.</li>
 * </ul>
 *
 * @param  uwTaskID [IN] Type #uint32_t Task ID.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID          0x02000207:invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED      0x0200020a:The task is not created.
 * @retval #LOS_ERRNO_TSK_NOT_SUSPENDED  0x02000209:The task is not suspended.
 * @retval #OS_OK                                   0: The task is successfully resumed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskSuspend
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskResume(uint32_t uwTaskID);

/**
 * @ingroup  los_task
 * @brief Suspend a task.
 *
 * @par Description:
 * This API is used to suspend a specified task, and the task will be removed from the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>The task that is running and locked cannot be suspended.</li>
 * <li>The idle task cannot be suspended.</li>
 * </ul>
 *
 * @param  uwTaskID [IN] Type #uint32_t Task ID.
 *
 * @retval #LOS_ERRNO_TSK_OPERATE_IDLE     0x02000214:Check the task ID and do not operate on the idle task.
 * @retval #LOS_ERRNO_TSK_ID_INVALID         0x02000207:invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED     0x0200020a:The task is not created.
 * @retval #LOS_ERRNO_TSK_ALREADY_SUSPENDED    0x02000208:The task is already suspended.
 * @retval #LOS_ERRNO_TSK_SUSPEND_LOCKED    0x03000215:The task being suspended is locked.
 * @retval #OS_OK                                    0: The task is successfully suspended.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskResume
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskSuspend(uint32_t uwTaskID);

/**
 * @ingroup  los_task
 * @brief Delete a task.
 *
 * @par Description:
 * This API is used to delete a specified task and release the resources for its task stack and task control block.
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  uwTaskID [IN] Type #uint32_t Task ID.
 *
 * @retval #LOS_ERRNO_TSK_OPERATE_IDLE     0x02000214:Check the task ID and do not operate on the idle task.
 * @retval #LOS_ERRNO_TSK_ID_INVALID         0x02000207:invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED     0x0200020a:The task is not created.
 * @retval #LOS_ERRNO_TSK_DELETE_LOCKED  0x0300020b:The task is locked when it is being deleted.

 * @retval #OS_OK                                    0: The task is successfully deleted.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskCreate
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskDelete(uint32_t uwTaskID);

/**
 * @ingroup  los_task
 * @brief Delay a task.
 *
 * @par Description:
 * This API is used to delay the execution of the current task. The task is able to be scheduled after it is delayed for a specified number of Ticks.
 *
 * @attention
 * <ul>
 * <li>The task fails to be delayed if it is being delayed during interrupt processing or it is locked.</li>
 * <li>If 0 is passed in and the task scheduling is not locked, execute the next task in the queue of tasks with the priority of the current task.
 * If no ready task with the priority of the current task is available, the task scheduling will not occur, and the current task continues to be executed.</li>
 * </ul>
 *
 * @param  uwTick [IN] Type #uint32_t Number of Ticks for which the task is delayed.
 *
 * @retval #LOS_ERRNO_TSK_DELAY_IN_INT                         0x0300020d:The task delay occurs during an interrupt.
 * @retval #LOS_ERRNO_TSK_DELAY_IN_LOCK                       0x0200020e:The task delay occurs when the task is locked.
 * @retval #LOS_ERRNO_TSK_ID_INVALID                             0x02000207: invalid Task ID
 * @retval #LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK        0x02000210: No tasks with the same priority is available for scheduling.
 * @retval #OS_OK                                    0: The task is successfully delayed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskDelay(uint32_t uwTick);

/**
 * @ingroup  los_task
 * @brief Lock the task scheduling.
 *
 * @par Description:
 * This API is used to lock the task scheduling. Task switching will not occur if the task scheduling is locked.
 *
 * @attention
 * <ul>
 * <li>If the task scheduling is locked, but interrupts are not disabled, tasks are still able to be interrupted.</li>
 * <li>One is added to the number of task scheduling locks if this API is called. The number of locks is decreased by one if the task scheduling is unlocked. Therefore, this API should be used together with LOS_TaskUnlock.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskUnlock
 * @since Huawei LiteOS V100R001C00
 */
extern void LOS_TaskLock(void);

/**
 * @ingroup  los_task
 * @brief Unlock the task scheduling.
 *
 * @par Description:
 * This API is used to unlock the task scheduling. Calling this API will decrease the number of task locks by one. If a task is locked more than once, the task scheduling will be unlocked only when the number of locks becomes zero.
 *
 * @attention
 * <ul>
 * <li>The number of locks is decreased by one if this API is called. One is added to the number of task scheduling locks if the task scheduling is locked. Therefore, this API should be used together with LOS_TaskLock.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskLock
 * @since Huawei LiteOS V100R001C00
 */
extern void LOS_TaskUnlock(void);

/**
 * @ingroup  los_task
 * @brief Set a task priority.
 *
 * @par Description:
 * This API is used to set the priority of a specified task.
 *
 * @attention
 * <ul>
 * <li>If the set priority is higher than the priority of the current running task, task scheduling probably occurs.</li>
 * <li>Changing the priority of the current running task also probably causes task scheduling.</li>
 * <li>Using the interface to change the priority of software timer task is not allowed.</li>
 * <li>Using the interface in the interrupt is not allowed.</li>
 * </ul>
 *
 * @param  uwTaskID [IN] Type #uint32_t Task ID.
 * @param  usTaskPrio [IN] Type #TSK_PRIOR_T Task priority.
 *
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR     0x02000203: Incorrect task priority.Re-configure the task priority
 * @retval #LOS_ERRNO_TSK_OPERATE_IDLE    0x02000214: Check the task ID and do not operate on the idle task.
 * @retval #LOS_ERRNO_TSK_ID_INVALID        0x02000207: invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED    0x0200020a: The task is not created.
 * @retval #OS_OK                                   0: The task priority is successfully set.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriGet
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskPriSet(uint32_t uwTaskID, uint16_t usTaskPrio);

/**
 * @ingroup  los_task
 * @brief Set the priority of the current running task to a specified priority.
 *
 * @par Description:
 * This API is used to set the priority of the current running task to a specified priority.
 *
 * @attention
 * <ul>
 * <li>Changing the priority of the current running task probably causes task scheduling.</li>
 * <li>Using the interface to change the priority of software timer task is not allowed.</li>
 * <li>Using the interface in the interrupt is not allowed.</li>
 * </ul>
 *
 * @param  usTaskPrio [IN] Type#TSK_PRIOR_T Task priority.
 *
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR     0x02000203: Incorrect task priority.Re-configure the task priority
 * @retval #LOS_ERRNO_TSK_OPERATE_IDLE    0x02000214: Check the task ID and do not operate on the idle task.
 * @retval #LOS_ERRNO_TSK_ID_INVALID        0x02000207: invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED    0x0200020a: The task is not created.
 * @retval #OS_OK                                   0: The priority of the current running task is successfully set to a specified priority.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriSet
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_CurTaskPriSet(uint16_t usTaskPrio);

/**
 * @ingroup  los_task
 * @brief Change the scheduling sequence of tasks with the same priority.
 *
 * @par Description:
 * This API is used to move a task in a queue of tasks with the same priority to the tail of the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>At least two ready tasks need to be included in the queue of ready tasks with the same priority. If the less than two ready tasks are included in the queue, an error is reported.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                             0x02000207: invalid Task ID
 * @retval #LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK        0x02000210: No tasks with the same priority is available for scheduling.
 * @retval #OS_OK                                                          0: The scheduling sequence of tasks with same priority is successfully changed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_TaskYield(void);

/**
 * @ingroup  los_task
 * @brief Obtain a task priority.
 *
 * @par Description:
 * This API is used to obtain the priority of a specified task.
 *
 * @attention None.
 *
 * @param  uwTaskID [IN] Type #uint32_t Task ID.
 *
 * @retval #OS_ERROR The task priority fails to be obtained.
 * @retval # The task priority is successfully returned.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriSet
 * @since Huawei LiteOS V100R001C00
 */
extern uint16_t LOS_TaskPriGet(uint32_t uwTaskID);

/**
 * @ingroup  los_task
 * @brief Obtain current running task ID.
 *
 * @par Description:
 * This API is used to obtain the ID of current running task.
 *
 * @attention None.
 *
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID    0x02000207: invalid Task ID.
 * @retval # Task ID.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_CurTaskIDGet(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_TASK_H */
