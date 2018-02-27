;Tiny OS Hardware driver.
;This file is only for Cortex M0 or binary compatible cores, for others cores, use conditional directive to use other drivers.
;
;This IS a part of the kernel.
;
;Author: zhanzr<zhanzr@foxmail.com>
;Date	:	2/21/2018

        PRESERVE8

        EXPORT  LOS_IntLock
        EXPORT  LOS_IntUnLock
        EXPORT  LOS_IntRestore
        EXPORT  LOS_StartToRun
        EXPORT  osTaskSchedule
        EXPORT  PendSV_Handler
        
        IMPORT  g_stLosTask
        IMPORT  g_bTaskScheduled

OS_NVIC_INT_CTRL            EQU     0xE000ED04
OS_NVIC_SYSPRI2             EQU     0xE000ED20
OS_NVIC_PENDSV_PRI          EQU     0xF0F00000
OS_NVIC_PENDSVSET           EQU     0x10000000
OS_TASK_STATUS_RUNNING      EQU     0x0010

    AREA |.text|, CODE, READONLY
    THUMB

LOS_StartToRun
    LDR     R4, =OS_NVIC_SYSPRI2
    LDR     R5, =OS_NVIC_PENDSV_PRI
    STR     R5, [R4]

    LDR     R0, =g_bTaskScheduled
    MOVS     R1, #1
    STR     R1, [R0]

    MOVS     R0, #2
    MSR     CONTROL, R0


    LDR     R0, =g_stLosTask
    LDR     R2, [R0, #4]
    LDR     R0, =g_stLosTask
    STR     R2, [R0]

    LDR     R3, =g_stLosTask
    LDR     R0, [R3]
    LDRH    R7, [R0 , #4]
    MOVS     R6,  #OS_TASK_STATUS_RUNNING
    ORRS     R7,  R7,R6
    STRH    R7,  [R0 , #4]

    LDR     R3, [R0]
    ADDS     R3, R3, #36

    LDMFD   R3!, {R0-R2}
    ADDS    R3,R3,#4
    LDMFD   R3!,{R4-R7}
    MSR     PSP, R3
    SUBS    R3,R3,#20
    LDR     R3,[R3]

    MOV     LR, R5
    MSR     xPSR, R7

    CPSIE   I
    BX      R6
    NOP
	
    ALIGN
    AREA KERNEL, CODE, READONLY
    THUMB

LOS_IntLock
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

LOS_IntUnLock
    MRS     R0, PRIMASK
    CPSIE   I
    BX      LR

LOS_IntRestore
    MSR     PRIMASK, R0
    BX      LR

osTaskSchedule
    LDR     R0, =OS_NVIC_INT_CTRL
    LDR     R1, =OS_NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

PendSV_Handler
    MRS     R12, PRIMASK
    CPSID   I
_TaskSwitch
    MRS     R0, PSP

    SUBS    R0, #36
    STMIA   R0!, {R4-R7}
    MOV     R3,  R8
    MOV     R4,  R9
    MOV     R5,  R10
    MOV     R6,  R11
    MOV     R7,  R12
    STMIA   R0!,{R3-R7}
    
    SUBS    R0,#36
    
    LDR     R5, =g_stLosTask
    LDR     R6, [R5]
    STR     R0, [R6]

    LDRH    R7, [R6 , #4]
    MOVS     R3,#OS_TASK_STATUS_RUNNING
    BICS     R7, R7, R3
    STRH    R7, [R6 , #4]


    LDR     R0, =g_stLosTask
    LDR     R0, [R0, #4]
    STR     R0, [R5]


    LDRH    R7, [R0 , #4]
    MOVS     R3,  #OS_TASK_STATUS_RUNNING
    ORRS     R7, R7,R3
    STRH    R7,  [R0 , #4]

    LDR     R1,   [R0]
    ADDS    R1,   #16
    LDMFD   R1!,  {R3-R7}
    MOV     R8,   R3
    MOV     R9,   R4
    MOV     R10,  R5
    MOV     R11,  R6
    MOV     R12,  R7
    SUBS    R1,   #36
    LDMFD   R1!, {R4-R7}
    
    ADDS    R1,   #20
    MSR     PSP,  R1

    MSR     PRIMASK, R12
    BX      LR
    
    NOP
    ALIGN
    END
