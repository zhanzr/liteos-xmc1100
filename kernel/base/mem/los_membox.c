//NeMOS memory box relevant function.
//
//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/28/2018

#include <string.h>

#include "los_membox.inc"
#include "los_base.ph"
#include "los_hwi.h"

#define OS_MEMBOX_NEXT(addr, uwBlkSize) (LOS_MEMBOX_NODE *)((uint8_t *)(addr) + (uwBlkSize))

#ifdef LOS_MEMBOX_CHECK
#define OS_MEMBOX_MAGIC 0xa55a5aa5
#define OS_MEMBOX_SET_MAGIC(addr) *((uint32_t *)(addr)) = OS_MEMBOX_MAGIC
#define OS_MEMBOX_CHECK_MAGIC(addr) ((*((uint32_t *)(addr)) == OS_MEMBOX_MAGIC) ? OS_OK : OS_NOK)
#else
#define OS_MEMBOX_SET_MAGIC(addr)
#define OS_MEMBOX_CHECK_MAGIC(addr) OS_OK
#endif

#define OS_MEMBOX_USER_ADDR(addr) ((void *)((uint8_t *)(addr) + LOS_MEMBOX_MAGIC_SIZE))
#define OS_MEMBOX_NODE_ADDR(addr) ((LOS_MEMBOX_NODE *)((uint8_t *)(addr) - LOS_MEMBOX_MAGIC_SIZE))


inline uint32_t osCheckBoxMem(const LOS_MEMBOX_INFO *pstBoxInfo, const void *pNode)
{
    uint32_t uwOffSet;

    if (pstBoxInfo->uwBlkSize == 0)
    {
        return OS_NOK;
    }

    uwOffSet = ((uint32_t)pNode - (uint32_t)(pstBoxInfo + 1));
    if ((uwOffSet % pstBoxInfo->uwBlkSize) != 0)
    {
        return OS_NOK;
    }

    if ((uwOffSet / pstBoxInfo->uwBlkSize) >= pstBoxInfo->uwBlkNum)
    {
        return OS_NOK;
    }

    return OS_MEMBOX_CHECK_MAGIC(pNode);
}

 uint32_t LOS_MemboxInit(void *pPool, uint32_t uwBoxSize, uint32_t uwBlkSize)
 {
    LOS_MEMBOX_INFO *pstBoxInfo = (LOS_MEMBOX_INFO *)pPool;
    LOS_MEMBOX_NODE *pstNode = (LOS_MEMBOX_NODE *)NULL;
    uint32_t i;
    uint32_t uwIntSave;

    if (pPool == NULL)
    {
        return OS_NOK;
    }

    if (uwBlkSize == 0)
    {
        return OS_NOK;
    }

    if (uwBoxSize < sizeof(LOS_MEMBOX_INFO))
    {
        return OS_NOK;
    }

    uwIntSave = LOS_IntLock();
    pstBoxInfo->uwBlkSize = LOS_MEMBOX_ALLIGNED(uwBlkSize + LOS_MEMBOX_MAGIC_SIZE);
    pstBoxInfo->uwBlkNum = ((uwBoxSize - sizeof(LOS_MEMBOX_INFO)) /pstBoxInfo->uwBlkSize);

    if (pstBoxInfo->uwBlkNum == 0)
    {
        LOS_IntRestore(uwIntSave);
        return OS_NOK;
    }

    pstNode = (LOS_MEMBOX_NODE *)(pstBoxInfo + 1);

    pstBoxInfo->stFreeList.pstNext = pstNode;

    for (i = 0; i < pstBoxInfo->uwBlkNum - 1; ++i)
    {
        pstNode->pstNext = OS_MEMBOX_NEXT(pstNode, pstBoxInfo->uwBlkSize);
        pstNode = pstNode->pstNext;
    }

    pstNode->pstNext = (LOS_MEMBOX_NODE *)NULL;

    LOS_IntRestore(uwIntSave);

    return OS_OK;
}

 void *LOS_MemboxAlloc(void *pPool)
{
    LOS_MEMBOX_INFO *pstBoxInfo = (LOS_MEMBOX_INFO *)pPool;
    LOS_MEMBOX_NODE *pstNode = (LOS_MEMBOX_NODE *)NULL;
    LOS_MEMBOX_NODE *pstRet = (LOS_MEMBOX_NODE *)NULL;
    uint32_t uwIntSave;

    if (pPool == NULL)
    {
        return NULL;
    }

    uwIntSave = LOS_IntLock();
    pstNode = &(pstBoxInfo->stFreeList);
    if (pstNode->pstNext != NULL)
    {
        pstRet = pstNode->pstNext;
        pstNode->pstNext = pstRet->pstNext;
        OS_MEMBOX_SET_MAGIC(pstRet);
    }

    LOS_IntRestore(uwIntSave);

    return pstRet == NULL ?  NULL : OS_MEMBOX_USER_ADDR(pstRet);
}

 uint32_t LOS_MemboxFree(void *pPool, void *pBox)
{
    LOS_MEMBOX_INFO *pstBoxInfo = (LOS_MEMBOX_INFO *)pPool;
    uint32_t uwRet = OS_NOK;
    uint32_t uwIntSave;

    if (pPool == NULL || pBox == NULL)
    {
        return OS_NOK;
    }

    uwIntSave = LOS_IntLock();
    do
    {
        LOS_MEMBOX_NODE *pstNode = OS_MEMBOX_NODE_ADDR(pBox);

        if (osCheckBoxMem(pstBoxInfo, pstNode) != OS_OK)
        {
            break;
        }

        pstNode->pstNext = pstBoxInfo->stFreeList.pstNext;
        pstBoxInfo->stFreeList.pstNext = pstNode;
        uwRet = OS_OK;
    } while (0);

    LOS_IntRestore(uwIntSave);

    return uwRet;

}

 void LOS_MemboxClr(void *pPool, void *pBox)
{
    LOS_MEMBOX_INFO *pstBoxInfo = (LOS_MEMBOX_INFO *)pPool;

    if (pPool == NULL || pBox == NULL)
    {
        return;
    }

    (void)memset(pBox, 0, pstBoxInfo->uwBlkSize - LOS_MEMBOX_MAGIC_SIZE);
}
