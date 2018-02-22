//Tiny OS Hardware driver.
//This file is only for Cortex M0 core, for others cores, use conditional directive to use other drivers.
//
//TODO: Modify the code according to the CMSIS and modern ANSI/ISO C Standard

//This IS a part of the kernel.
//
//Author: zhanzr<zhanzr@foxmail.com>
//Date	:	2/21/2018
 /**@defgroup los_hwi Hardware interrupt
   *@ingroup kernel
 */
#ifndef _LOS_HWI_H
#define _LOS_HWI_H

#include <cmsis_compiler.h>

#include "los_base.h"
#include "los_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup  los_hwi
 * Define the type of a hardware interrupt handling function.
 */
typedef void (* HWI_PROC_FUNC)(void);

/**
 * @ingroup los_hwi
 * Count of interrupts.
 */
extern uint32_t  g_vuwIntCount;

/**
 * @ingroup los_hwi
 * An interrupt is active.
 */
#define OS_INT_ACTIVE               (g_vuwIntCount > 0)

/**
 * @ingroup los_hwi
 * An interrupt is inactive.
 */
#define OS_INT_INACTIVE             (!(OS_INT_ACTIVE))

/**
 * @ingroup los_hwi
 * Highest priority of a hardware interrupt.
 */
#define  OS_HWI_PRIO_HIGHEST        0

/**
 * @ingroup los_hwi
 * Lowest priority of a hardware interrupt.
 */
#define  OS_HWI_PRIO_LOWEST         3

/**
 * @ingroup los_hwi
 * Maximum number of hardware support for hardware interrupt.
 */
#define OS_HWI_MAX_NUM              48

/**
 * @ingroup los_hwi
 * Maximum interrupt number.
 */
#define OS_HWI_MAX                  ((OS_HWI_MAX_NUM) - 1)

/**
 * @ingroup los_hwi
 * Minimum interrupt number.
 */
#define OS_HWI_MIN                  0

/**
 * @ingroup los_hwi
 * Count of M0 system interrupt vector.
 */
#define OS_M0_SYS_VECTOR_CNT        16

/**
 * @ingroup los_hwi
 * Count of M0 IRQ interrupt vector.
 */
#define OS_M0_IRQ_VECTOR_CNT        32

/**
 * @ingroup los_hwi
 * Count of M0 interrupt vector.
 */
#define OS_M0_VECTOR_CNT            (OS_M0_SYS_VECTOR_CNT + OS_M0_IRQ_VECTOR_CNT)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt number.
 *
 * Value: 0x02000900
 *
 * Solution: Ensure that the interrupt number is valid. The value range of the interrupt number applicable for a Cortex-M4 platform is [0,240].
 */
#define OS_ERRNO_HWI_NUM_INVALID                            LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x00)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Null hardware interrupt handling function.
 *
 * Value: 0x02000901
 *
 * Solution: Pass in a valid non-null hardware interrupt handling function.
 */
#define OS_ERRNO_HWI_PROC_FUNC_NULL                         LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x01)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient interrupt resources for hardware interrupt creation.
 *
 * Value: 0x02000902
 *
 * Solution: Increase the configured maximum number of supported hardware interrupts.
 */
#define OS_ERRNO_HWI_CB_UNAVAILABLE                         LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x02)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient memory for hardware interrupt initialization.
 *
 * Value: 0x02000903
 *
 * Solution: Expand the configured memory.
 */
#define OS_ERRNO_HWI_NO_MEMORY                              LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x03)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created.
 *
 * Value: 0x02000904
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_ALREADY_CREATED                        LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x04)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt priority.
 *
 * Value: 0x02000905
 *
 * Solution: Ensure that the interrupt priority is valid. The value range of the interrupt priority applicable for a Cortex-M4 platform is [OS_HWI_PRIO_HIGHEST,OS_HWI_PRIO_LOWEST].
 */
#define OS_ERRNO_HWI_PRIO_INVALID                           LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x05)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Incorrect interrupt creation mode.
 *
 * Value: 0x02000906
 *
 * Solution: The interrupt creation mode can be only set to OS_HWI_MODE_COMM or OS_HWI_MODE_FAST of which the value can be 0 or 1.
 */
#define OS_ERRNO_HWI_MODE_INVALID                           LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x06)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created as a fast interrupt.
 *
 * Value: 0x02000907
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_FASTMODE_ALREADY_CREATED               LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x07)

/**
 * @ingroup los_hwi
 * AIRCR register priority group parameter .
 */
#define OS_NVIC_AIRCR_PRIGROUP      7

/**
 * @ingroup los_hwi
 * Boot interrupt vector table.
 */
extern uint32_t _BootVectors[];

/**
 * @ingroup los_hwi
 * SysTick control and status register.
 */
#define OS_SYSTICK_CONTROL_REG      0xE000E010

/**
 * @ingroup los_hw
 * SysTick reload value register.
 */
#define OS_SYSTICK_RELOAD_REG       0xE000E014

/**
 * @ingroup los_hwi
 * interrupt pending register.
 */
#define OS_NVIC_SETPEND_BASE        0xE000E200

/**
 * @ingroup los_hwi
 * interrupt active register.
 */
#define OS_NVIC_INT_ACT_BASE        0xE000E300

/**
 * @ingroup los_hw
 * SysTick current value register.
 */
#define OS_SYSTICK_CURRENT_REG      0xE000E018

/**
 * @ingroup los_hwi
 * Interrupt Priority-Level Registers.
 */
#define OS_NVIC_PRI_BASE            0xE000E400

/**
 * @ingroup los_hwi
 * Interrupt enable register for 0-31.
 */
#define OS_NVIC_SETENA_BASE         0xE000E100

/**
 * @ingroup los_hwi
 * Interrupt disable register for 0-31.
 */
#define OS_NVIC_CLRENA_BASE         0xE000E180

/**
 * @ingroup los_hwi
 * Interrupt control and status register.
 */
#define OS_NVIC_INT_CTRL            0xE000ED04

/**
 * @ingroup los_hwi
 * Vector table offset register.
 */
 //No VTOR in Cortex M0
//#define OS_NVIC_VTOR                0xE000ED08

/**
 * @ingroup los_hwi
 * Application interrupt and reset control register
 */
#define OS_NVIC_AIRCR               0xE000ED0C

/**
 * @ingroup los_hwi
 * System exception priority register.
 */
#define OS_NVIC_EXCPRI_BASE         0xE000ED18

/**
 * @ingroup los_hwi
 * Interrupt No. 1 :reset.
 */
#define OS_EXC_RESET           1

/**
 * @ingroup los_hwi
 * Interrupt No. 2 :Non-Maskable Interrupt.
 */
#define OS_EXC_NMI             2

/**
 * @ingroup los_hwi
 * Interrupt No. 3 :(hard)fault.
 */
#define OS_EXC_HARD_FAULT      3

/**
 * @ingroup los_hwi
 * Interrupt No. 4 :MemManage fault.
 */
#define OS_EXC_MPU_FAULT       4

/**
 * @ingroup los_hwi
 * Interrupt No. 5 :Bus fault.
 */
#define OS_EXC_BUS_FAULT       5

/**
 * @ingroup los_hwi
 * Interrupt No. 6 :Usage fault.
 */
#define OS_EXC_USAGE_FAULT     6

/**
 * @ingroup los_hwi
 * Interrupt No. 11 :SVCall.
 */
#define OS_EXC_SVC_CALL        11

/**
 * @ingroup los_hwi
 * Interrupt No. 12 :Debug monitor.
 */
#define OS_EXC_DBG_MONITOR     12

/**
 * @ingroup los_hwi
 * Interrupt No. 14 :PendSV.
 */
#define OS_EXC_PEND_SV         14

/**
 * @ingroup los_hwi
 * Interrupt No. 15 :SysTick.
 */
#define OS_EXC_SYS_TICK        15

/**
 * @ingroup los_hwi
 * hardware interrupt form mapping handling function array.
 */
extern HWI_PROC_FUNC m_pstHwiForm[OS_M0_VECTOR_CNT];

/**
 * @ingroup los_hwi
 * hardware interrupt Slave form mapping handling function array.
 */
extern HWI_PROC_FUNC m_pstHwiSlaveForm[OS_M0_VECTOR_CNT];

extern void Reset_Handler(void);

/**
 * @ingroup los_hwi
 * Set interrupt vector table.
 */
#define osSetVector(uwNum, pfnVector)       \
    m_pstHwiForm[uwNum + OS_M0_SYS_VECTOR_CNT] = osInterrupt;\
    m_pstHwiSlaveForm[uwNum + OS_M0_SYS_VECTOR_CNT] = pfnVector;


/**
 * @ingroup  los_hwi
 * @brief Create a hardware interrupt.
 *
 * @par Description:
 * This API is used to configure a hardware interrupt and register a hardware interrupt handling function.
 *
 * @attention
 * <ul>
 * <li>The hardware interrupt module is usable only when the configuration item for hardware interrupt tailoring is enabled.</li>
 * <li>Hardware interrupt number value range: [OS_HWI_MIN,OS_HWI_MAX]. The value range applicable for a Cortex-M4 platform is [0,240].</li>
 * <li>OS_HWI_MAX_NUM specifies the maximum number of interrupts that can be created.</li>
 * <li>Before executing an interrupt on a platform, refer to the chip manual of the platform.</li>
 * </ul>
 *
 * @param  uwHwiNum   [IN] Type#uint32_t: hardware interrupt number. The value range applicable for a Cortex-M4 platform is [0,240].
 * @param  usHwiPrio  [IN] Type#uint16_t: hardware interrupt priority. Ignore this parameter temporarily.
 * @param  usMode     [IN] Type#uint16_t: hardware interrupt mode. Ignore this parameter temporarily.
 * @param  pfnHandler [IN] Type#HWI_PROC_FUNC: interrupt handler used when a hardware interrupt is triggered.
 * @param  uwArg      [IN] Type#uint32_t: input parameter of the interrupt handler used when a hardware interrupt is triggered.
 *
 * @retval #OS_ERRNO_HWI_PROC_FUNC_NULL               0x02000901: Null hardware interrupt handling function.
 * @retval #OS_ERRNO_HWI_NUM_INVALID                     0x02000900: Invalid interrupt number.
 * @retval #OS_ERRNO_HWI_NO_MEMORY                     0x02000903: Insufficient memory for hardware interrupt creation.
 * @retval #OS_ERRNO_HWI_ALREADY_CREATED              0x02000904: The interrupt handler being created has already been created.
 * @retval #LOS_OK                                                       0,               : The interrupt is successfully created.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_HwiCreate( uint32_t  uwHwiNum,
                           uint16_t   usHwiPrio,
                           uint16_t    usMode,
                           HWI_PROC_FUNC pfnHandler,
                           uint32_t     uwArg
                           );


/**
 * @ingroup  los_hwi
 * @brief: Hardware interrupt entry function.
 *
 * @par Description:
 * This API is used as all hardware interrupt handling function entry.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void  osInterrupt(void);



/**
 * @ingroup  los_hwi
 * @brief: Get a interrupt number.
 *
 * @par Description:
 * This API is used to get the current interrupt number.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param: None.
 *
 * @retval: Interrupt Indexes number.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t osIntNumGet(void);



/**
 * @ingroup  los_hwi
 * @brief: Reset the vector table.
 *
 * @par Description:
 * This API is used to reset the vector table.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void  osResetVector(void);



/**
 * @ingroup  los_hwi
 * @brief: Default vector handling function.
 *
 * @par Description:
 * This API is used to configure interrupt for null function.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void  osHwiDefaultHandler(void);



/**
 * @ingroup  los_hwi
 * @brief: Pended System Call.
 *
 * @par Description:
 * PendSV can be pended and is useful for an OS to pend an exception
 * so that an action can be performed after other important tasks are completed.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void  PendSV_Handler(void);

 /**
 *@ingroup los_hwi
 *@brief Enable all interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to enable all IRQ and FIQ interrupts in the CPSR.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param None.
 *
 *@retval CPSR value obtained after all interrupts are enabled.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntRestore
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t* LOS_IntUnLock(void);



 /**
 *@ingroup los_hwi
 *@brief Disable all interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to disable all IRQ and FIQ interrupts in the CPSR.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param None.
 *
 *@retval CPSR value obtained before all interrupts are disabled.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntRestore
 *@since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_IntLock(void);



 /**
 *@ingroup los_hwi
 *@brief Restore interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to restore the CPSR value obtained before all interrupts are disabled.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>This API can be called only after all interrupts are disabled, and the input parameter value should be the value returned by calling the all interrupt disabling API.</li>
 *</ul>
 *
 *@param uwIntSave [IN] CPSR value obtained before all interrupts are disabled.
 *
 *@retval None.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntLock
 *@since Huawei LiteOS V100R001C00
 */
extern void LOS_IntRestore(uint32_t uwIntSave);



/**
 * @ingroup  los_hwi
 * @brief Delete hardware interrupt.
 *
 * @par Description:
 * This API is used to delete hardware interrupt.
 *
 * @attention
 * <ul>
 * <li>The hardware interrupt module is usable only when the configuration item for hardware interrupt tailoring is enabled.</li>
 * <li>Hardware interrupt number value range: [OS_HWI_MIN,OS_HWI_MAX]. The value range applicable for a Cortex-M4 platform is [0,240].</li>
 * <li>OS_HWI_MAX_NUM specifies the maximum number of interrupts that can be created.</li>
 * <li>Before executing an interrupt on a platform, refer to the chip manual of the platform.</li>
 * </ul>
 *
 * @param  uwHwiNum   [IN] Type#uint32_t: hardware interrupt number. The value range applicable for a Cortex-M4 platform is [0,240].
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID              0x02000900: Invalid interrupt number.
 * @retval #LOS_OK                                  0: The interrupt is successfully delete.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern uint32_t LOS_HwiDelete(uint32_t uwHwiNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */



#endif /* _LOS_HWI_H */

