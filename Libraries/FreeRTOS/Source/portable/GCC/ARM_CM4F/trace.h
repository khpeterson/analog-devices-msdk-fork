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

#define traceINIT() \
do {                                                              \
    mxc_tmr_cfg_t tmr = {TMR_PRES_1, TMR_MODE_CONTINUOUS, -1, 0}; \
    MXC_TMR_Init(MXC_TMR3, &tmr);                                 \
    MXC_TMR_Start(MXC_TMR3);                                      \
} while (0)

#define traceTASK_NOTIFY_TAKE_BLOCK( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)
    
#define traceTASK_NOTIFY_TAKE( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_WAIT_BLOCK( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_WAIT_BLOCK_MAYBE1( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_WAIT_BLOCK_MAYBE2( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_WAIT( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_WAIT_NOT_NOTIFY( uxIndexToWait ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY( uxIndexToNotify ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_FROM_ISR( uxIndexToNotify ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_NOTIFY_GIVE_FROM_ISR( uxIndexToNotify ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxTCB->pcTaskName;     \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#if 0
#define traceTASK_SWITCHED_OUT() \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)
#endif

#define traceTASK_SWITCHED_IN() \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_SCHEDULER_SUSPENDED() \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_INCREMENT_TICK( xTickCount ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceTASK_INCREMENT_TICK_DONE( xTickCount ) \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = pxCurrentTCB->pcTaskName; \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#define traceCRITICAL() \
do {                                                                    \
    _trace_points[_trace_point_index].taskname = "TBD";                 \
    _trace_points[_trace_point_index].filename = __FILE__;              \
    _trace_points[_trace_point_index].linenum = __LINE__;               \
    _trace_points[_trace_point_index].counter = TRACE_COUNTER();        \
    _trace_point_index += 1;                                            \
    _trace_point_index &= (_NTRACE_POINTS - 1);                         \
} while (0)

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */
    
