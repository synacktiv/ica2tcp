/***************************************************************************
 *
 *  ctxnevt.h
 *
 *  This module contains thread context interface definitions.
 *
 *  $Id$
 *
 *  Copyright 1999-2013 Citrix Systems, Inc.  All Rights Reserved.
 *
 ****************************************************************************/

#ifndef __CTXEVT_H__
#define __CTXEVT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pal_event *PEVT;

/* Error codes returned by the event functions.
 * KVME plugins may return EVT_FD_BUSY if an attempt is made to
 * register a file descriptor/direction combination that is already registered,
 * but must support separate registrations for read and write.
 */

#define EVT_SUCCESS	        0
#define EVT_OBJ_CREATE_FAILED	1
#define EVT_OBJ_DESTROY_FAILED	2
#define EVT_FD_BUSY             3       /* See above. */

/* These functions wrap the select_fd() entry point to KVME plugins. */

/*  
 * Evt_create
 * This interface allocates an event object that can be used to signal events.
 * First argument is a dummy, and should be NULL.
 */  

VPSTATUS
Evt_create(void *hTC, PFNDELIVER pDeliverFunc, void *pSubscriberId, PEVT *out);
   

/*  
 * Evt_destroy  
 * This interface destroys a previously created event object.  
 * The referenced variable will be set to NULL.
 *
 * Outstanding callbacks MUST be removed (Evt_remove_triggers()) before this
 * is called.
 */  

VPSTATUS Evt_destroy(PEVT *pHnd);

/*  
 * Evt_signal  
 * Call the function stored in the target structure.
 */  

VPSTATUS Evt_signal(PEVT hEvt);

/* Associate a file descriptor with an EVT, so that former will be signalled
 * (function called) when the descriptor is ready for input or output.
 * Only one EVT can be coupled to a file descriptor direction at any one time.
 * Passing NULL for the first argument removes any existing coupling.
 */

VPSTATUS Evt_trigger_for_input(PEVT pEvt, int fd);
VPSTATUS Evt_trigger_for_output(PEVT pEvt, int fd);
VPSTATUS Evt_remove_triggers(int fd);

#ifdef __cplusplus
}
#endif

#endif /* __CTXEVT_H__ */
