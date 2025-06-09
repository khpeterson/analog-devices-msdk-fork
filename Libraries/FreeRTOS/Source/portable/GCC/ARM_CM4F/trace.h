#ifndef TRACE_H
#define TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmr.h"
#define TRACE_TMR MXC_TMR3
#define TRACE_COUNTER(void) MXC_TMR_GetCount(TRACE_TMR)

#define _NTRACE_POINTS 8192

typedef struct {
    char *taskname;
    char *filename;
    unsigned long linenum;
    unsigned long counter;
} trace_point_t;

extern unsigned long _trace_point_index;
extern trace_point_t _trace_points[_NTRACE_POINTS];

#define TRACE_INIT() \
do {                                                              \
    mxc_tmr_cfg_t tmr = {TMR_PRES_1, TMR_MODE_CONTINUOUS, -1, 0}; \
    MXC_TMR_Init(TRACE_TMR, &tmr);                                \
    MXC_TMR_Start(TRACE_TMR);                                     \
} while (0)

#define TRACE_TASK(_taskname)                                           \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = (_taskname);           \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_TAKE_BLOCK( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_TAKE( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_WAIT_BLOCK( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_WAIT_BLOCK_MAYBE1( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_WAIT_BLOCK_MAYBE2( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_WAIT( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_WAIT_NOT_NOTIFY( uxIndexToWait ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY( uxIndexToNotify ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_FROM_ISR( uxIndexToNotify ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_NOTIFY_GIVE_FROM_ISR( uxIndexToNotify ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#if 0
#define traceTASK_SWITCHED_OUT() \
    TRACE_TASK(pxCurrentTCB->pcTaskName)
#endif

#define traceTASK_SWITCHED_IN() \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_SCHEDULER_SUSPENDED() \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_INCREMENT_TICK( xTickCount ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceTASK_INCREMENT_TICK_DONE( xTickCount ) \
    TRACE_TASK(pxCurrentTCB->pcTaskName)

#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
    TRACE_TASK(pxTCB->pcTaskName)

#define traceCRITICAL() \
    TRACE_TASK("TBD")

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */
    
