#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

extern uint32_t SystemCoreClock;

/* Scheduler */
#define configUSE_PREEMPTION                 1
#define configCPU_CLOCK_HZ                   (SystemCoreClock)
#define configTICK_RATE_HZ                   1000
#define configMAX_PRIORITIES                 5
#define configUSE_TIME_SLICING               1
#define configIDLE_SHOULD_YIELD              1

/* Memory */
#define configMINIMAL_STACK_SIZE             128
#define configTOTAL_HEAP_SIZE                (8 * 1024)
#define configMAX_TASK_NAME_LEN              16
#define configSUPPORT_DYNAMIC_ALLOCATION     1
#define configSUPPORT_STATIC_ALLOCATION      0

/* Tick type: 32-bit */
#define configTICK_TYPE_WIDTH_IN_BITS        TICK_TYPE_WIDTH_32_BITS

/* Kernel features */
#define configUSE_IDLE_HOOK                  0
#define configUSE_TICK_HOOK                  0
#define configUSE_MUTEXES                    1
#define configUSE_TIMERS                     0
#define configUSE_TRACE_FACILITY             0

/* Error detection */
#define configCHECK_FOR_STACK_OVERFLOW       2
#define configUSE_MALLOC_FAILED_HOOK         1

#define configASSERT(x)                      \
    if ((x) == 0) {                          \
        taskDISABLE_INTERRUPTS();            \
        for (;;) {}                          \
    }

/* API */
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskSuspend                 1

/* Cortex-M3 interrupt priority */
#define configPRIO_BITS                      4

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5

#define configKERNEL_INTERRUPT_PRIORITY \
    (15 << (8 - configPRIO_BITS))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (5 << (8 - configPRIO_BITS))

/* Exception handlers */
#define vPortSVCHandler      SVC_Handler
#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler

#include "FreeRTOS.h"

#endif