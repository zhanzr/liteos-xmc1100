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
#include <string.h>

#include "los_membox.inc"
#include "los_base.ph"
#include "los_hwi.h"

#define OS_MEMBOX_NEXT(addr, uwBlkSize) (LOS_MEMBOX_NODE *)((uint8_t *)(addr) + (uwBlkSize))

#ifdef LOS_MEMBOX_CHECK
#define OS_MEMBOX_MAGIC 0xa55a5aa5
#define OS_MEMBOX_SET_MAGIC(addr) *((uint32_t *)(addr)) = OS_MEMBOX_MAGIC
#define OS_MEMBOX_CHECK_MAGIC(addr) ((*((uint32_t *)(addr)) == OS_MEMBOX_MAGIC) ? LOS_OK : LOS_NOK)
#else
#define OS_MEMBOX_SET_MAGIC(addr)
#define OS_MEMBOX_CHECK_MAGIC(addr) LOS_OK
#endif

#define OS_MEMBOX_USER_ADDR(addr) ((void *)((uint8_t *)(addr) + LOS_MEMBOX_MAGIC_SIZE))
#define OS_MEMBOX_NODE_ADDR(addr) ((LOS_MEMBOX_NODE *)((uint8_t *)(addr) - LOS_MEMBOX_MAGIC_SIZE))


inline uint32_t osCheckBoxMem(const LOS_MEMBOX_INFO *pstBoxInfo, const void *pNode)
{
    uint32_t uwOffSet;

    if (pstBoxInfo->uwBlkSize == 0)
    {
        return LOS_NOK;
    }

    uwOffSet = ((uint32_t)pNode - (uint32_t)(pstBoxInfo + 1));
    if ((uwOffSet % pstBoxInfo->uwBlkSize) != 0)
    {
        return LOS_NOK;
    }

    if ((uwOffSet / pstBoxInfo->uwBlkSize) >= pstBoxInfo->uwBlkNum)
    {
        return LOS_NOK;
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
        return LOS_NOK;
    }

    if (uwBlkSize == 0)
    {
        return LOS_NOK;
    }

    if (uwBoxSize < sizeof(LOS_MEMBOX_INFO))
    {
        return LOS_NOK;
    }

    uwIntSave = LOS_IntLock();
    pstBoxInfo->uwBlkSize = LOS_MEMBOX_ALLIGNED(uwBlkSize + LOS_MEMBOX_MAGIC_SIZE);
    pstBoxInfo->uwBlkNum = ((uwBoxSize - sizeof(LOS_MEMBOX_INFO)) /pstBoxInfo->uwBlkSize);

    if (pstBoxInfo->uwBlkNum == 0)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_NOK;
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

    return LOS_OK;
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
    uint32_t uwRet = LOS_NOK;
    uint32_t uwIntSave;

    if (pPool == NULL || pBox == NULL)
    {
        return LOS_NOK;
    }

    uwIntSave = LOS_IntLock();
    do
    {
        LOS_MEMBOX_NODE *pstNode = OS_MEMBOX_NODE_ADDR(pBox);

        if (osCheckBoxMem(pstBoxInfo, pstNode) != LOS_OK)
        {
            break;
        }

        pstNode->pstNext = pstBoxInfo->stFreeList.pstNext;
        pstBoxInfo->stFreeList.pstNext = pstNode;
        uwRet = LOS_OK;
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
