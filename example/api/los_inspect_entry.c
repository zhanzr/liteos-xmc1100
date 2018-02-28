#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/******************************************************************************
    here include some special hearder file you need
******************************************************************************/
#include <string.h>
#include "los_inspect_entry.h"
#include "los_task.h"
#include "los_demo_debug.h"

#include "los_api_task.h"
#include "los_api_dynamic_mem.h"
#include "los_api_static_mem.h"
#include "los_api_interrupt.h"
#include "los_api_msgqueue.h"
#include "los_api_event.h"
#include "los_api_mutex.h"
#include "los_api_sem.h"
#include "los_api_timer.h"
#include "los_api_systick.h"
#include "los_api_list.h"

#include "retarget_io_drv.h"

/*****************************************************************************
    global var
 *****************************************************************************/
static uint32_t gInspectErrCnt = 0;

static uint32_t g_uwDemoTaskID;


static osInspect_Def gInspect[LOS_INSPECT_BUFF] = {

    {LOS_INSPECT_TASK,LOS_INSPECT_STU_START,Example_TskCaseEntry,"TASK"},

    {LOS_INSPECT_EVENT,LOS_INSPECT_STU_START,Example_SndRcvEvent,"EVENT"},
    
    {LOS_INSPECT_MSG,LOS_INSPECT_STU_START,Example_MsgQueue,"MSG"},
    
    {LOS_INSPECT_SEM,LOS_INSPECT_STU_START,Example_Semphore,"SEM"},
    
    {LOS_INSPECT_MUTEX,LOS_INSPECT_STU_START,Example_MutexLock,"MUTEX"},
    
    {LOS_INSPECT_SYSTIC,LOS_INSPECT_STU_START,Example_GetTick,"SYSTIC"},
    
    {LOS_INSPECT_TIMER,LOS_INSPECT_STU_START,Example_swTimer,"TIMER"},
    
    {LOS_INSPECT_LIST,LOS_INSPECT_STU_START,Example_list,"LIST"},
    
    {LOS_INSPECT_SMEM,LOS_INSPECT_STU_START,Example_StaticMem,"S_MEM"},
    
    {LOS_INSPECT_DMEM,LOS_INSPECT_STU_START,Example_Dyn_Mem,"D_MEM"},
    
    //{LOS_INSPECT_INTERRUPT,LOS_INSPECT_STU_START,Example_Interrupt},
};

/*****************************************************************************
 Function    : LOS_InspectStatusSetByID
 Description : Set Inspect Status ByID
 Input       : enInspectID  InspectID
               enInspectStu InspectStu
 Output      : None
 Return      : None
 *****************************************************************************/
uint32_t LOS_InspectStatusSetByID(enInspectID InspectID,enInspectStu InspectStu)
{
    uint32_t ulIndex = 0;

    if(InspectID >= LOS_INSPECT_BUFF)
    {
        dprintf("\nInspectID = [%d] Err.\n",InspectID);
        return LOS_NOK;
    }

    for(ulIndex = 0;ulIndex < LOS_INSPECT_BUFF;ulIndex++)
    {   
        if(InspectID == gInspect[ulIndex].InspectID)
        {
            gInspect[ulIndex].Status = InspectStu;
            break;
        }
    }

    if(LOS_INSPECT_BUFF == ulIndex)
    {
        dprintf("\nInspectID = [%d] not find.\n",InspectID);
        return LOS_NOK;
    }
    else
    {
        return LOS_OK;
    }
}

/*****************************************************************************
 Function    : LOS_InspectByID
 Description : Inspect function By ID
 Input       : enInspectID  InspectID
 Output      : None
 Return      : LOS_NOK/LOS_OK
 *****************************************************************************/
uint32_t LOS_InspectByID(enInspectID InspectID)
{
    uint32_t ulIndex,ulRet = LOS_OK;
    enInspectStu  enCurStatus = LOS_INSPECT_STU_START;
    
    if(InspectID >= LOS_INSPECT_BUFF)
    {
        dprintf("\nInspectID = [%d] Err.\n",InspectID);
        return LOS_NOK;
    }
    
    for(ulIndex = 0;ulIndex < LOS_INSPECT_BUFF;ulIndex++)
    {
        if(InspectID == gInspect[ulIndex].InspectID)
        {
            if(NULL == gInspect[ulIndex].Inspectfunc)
            {
                dprintf("InspectID = [%d] Err,Inspectfunc is NULL.\n\n",InspectID);
                return LOS_NOK;
            }
            
            ulRet = gInspect[ulIndex].Inspectfunc();
            
            do{
                if((LOS_INSPECT_STU_SUCCESS == gInspect[ulIndex].Status) && (ulRet == LOS_OK))
                {
                    dprintf("Inspect %s success\n\n",gInspect[ulIndex].name);
                    enCurStatus = LOS_INSPECT_STU_SUCCESS;
                }
                else if(LOS_INSPECT_STU_ERROR == gInspect[ulIndex].Status)
                {
                    enCurStatus = LOS_INSPECT_STU_ERROR;
                    dprintf("Inspect %s error,gInspectErrCnt = [%d]\n\n",gInspect[ulIndex].name,gInspectErrCnt);
                }
                
                (void)LOS_TaskDelay(100);
                    
            }while((LOS_INSPECT_STU_START == gInspect[ulIndex].Status)||(enCurStatus != gInspect[ulIndex].Status));
            
            break;
        }
    }
    
    if(LOS_INSPECT_BUFF == ulIndex)
    {
        return LOS_NOK;
    }
    else
    {
        return enCurStatus ? LOS_NOK:LOS_OK;
    }

}

/*****************************************************************************
 Function    : LOS_Inspect_TskDeal
 Description : Inspect Task Deal
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
static void LOS_Inspect_TskDeal(void)
{
    uint32_t ulRet = LOS_OK;
     
    gInspectErrCnt = 0;
      
     /* output a message on hyperterminal using printf function */
    dprintf("\nLos Inspect start.\n");
      
    for(int index = 0;index < LOS_INSPECT_BUFF;index++)
    {
        ulRet = LOS_InspectByID((enInspectID)index);
        if(LOS_OK != ulRet)
        {
            gInspectErrCnt++;
            
            /* turn on LED2 */
            LED_On(2);
        }
    }
     
    dprintf("Inspect completed,gInspectErrCnt = [%d]\n\n",gInspectErrCnt);
}

/*****************************************************************************
 Function    : LOS_Inspect_Entry
 Description : Create Inspect task
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_Inspect_Entry(void)
{
    uint32_t uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (void)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_Inspect_TskDeal;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "InspectTsk";
    stTaskInitParam.usTaskPrio = 9;
    uwRet = LOS_TaskCreate(&g_uwDemoTaskID, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }
    
    return;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

