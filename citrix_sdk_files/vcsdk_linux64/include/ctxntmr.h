/*****************************************************************************
*
*   ctxntmr.h
*
*   Timer Abstraction Interface
*
*   $Id$
*
*   Copyright 1998-2013 Citrix Systems, Inc.  All Rights Reserved.
*
******************************************************************************/

#ifndef __CTXTMR_H_
#define __CTXTMR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TMR_SUCCESS             0
#define TMR_OBJ_CREATE_FAILED   1

/******************************************************************************
 *
 *  Forward Declarations for opaque TMR structure
 *
 *****************************************************************************/

typedef struct _TMR TMR, *PTMR;

/******************************************************************************
 *
 *  Exported Functions
 *
 *    Tmr_create
 *    Tmr_setEnabled
 *    Tmr_setPeriod
 *    Tmr_destroy
 *
 *****************************************************************************/


/******************************************************************************
 *
 *  Tmr_create
 *
 *    Creates a timer object and returns its handle.
 *    The initial state is disabled.
 *    When the timer fires, the "deliver" function is called:
 *      (*pfnDeliver)(pvSubscriber, *phTimer).
 *
 *****************************************************************************/
VPSTATUS Tmr_create(void       *hTC, 
                    UINT32      uiPeriod,       /* milliSeconds */
                    void       *pvSubscriber,   /* callback argument */
                    PFNDELIVER  pfnDeliver,     /* callback function */
                    PTMR       *phTimer);       /* return timer handle */


/******************************************************************************
 *
 *  Tmr_setEnabled
 *
 *    Enables or disables a timer object.
 *    Enabling a disabled timer restarts the timing period; re-enabling
 *    an enabled timer does nothing.
 *
 *****************************************************************************/
VPSTATUS Tmr_setEnabled(PTMR    hTimer,
                        BOOLEAN fEnabled);


/******************************************************************************
 *
 *  Tmr_setPeriod
 *
 *    Sets the timeout period for a timer.
 *    Setting the period of an enabled timer restarts it with the new period.
 *
 *****************************************************************************/
VPSTATUS Tmr_setPeriod(PTMR     hTimer,
                       UINT32   uiPeriod);

/******************************************************************************
 *
 *  Tmr_destroy
 *
 *    Destroys a timer object given a pointer to its handle and
 *    sets the handle to NULL.
 *
 ****************************************************************************/
VPSTATUS Tmr_destroy(PTMR      *phTimer);

#ifdef __cplusplus
}
#endif

#endif /* __CTXTMR_H_ */

