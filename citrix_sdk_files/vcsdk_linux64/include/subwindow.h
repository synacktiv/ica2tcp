/******************************************************************************
*
*   subwindow.h
*
*   Interface definitions for embedded subwindows for video output..
*
*   $Id$
*
*   Copyright 2008-2013 Citrix Systems, Inc. All Rights Reserved.
*
******************************************************************************/
#ifndef _SUBWINDOW_H_
#define _SUBWINDOW_H_

/* The functions defined here are intended to support virtual channel
 * implementations that need to insert their own graphics into a 
 * session window, either the single session window or a "Seamless" one.
 *
 * It might appear that this simply requires access to the local window
 * identifier, given one sent from the server, so that virtual channel
 * code can create a subwindow for its own output.  In a Seamless session
 * the possibility of races make things more complicated: a request to
 * insert graphics into a Seamless window may arrive before the client
 * has created the window; and a Seamless window may be destroyed before
 * the client code that is using a subwindow is informed by the server.
 *
 * There are essentially two interfaces here, high and low-level.
 * The high-level interface handles unexpected deletion of the parent
 * window by reparenting the child, but when MM_get_window() fails,
 * the user must set-up and respond to window creation callbacks
 * to deal with window creation races.
 *
 * We do not currently know of any applications that would need to use
 * the low-level functions.
 */

/* Note: hwin parameters are server-provided window identifiers, not local
 * handles.  Local (X Windows) window identifiers are xwin parameters.
 */

/* *** High-level functions. ***/

/* Create an OS window (*xwinp) that is a subwindow of an existing
 * session window with server handle hwin.
 *
 * Pass 0 as the server handle in a non-Seamless (single window) session.
 *
 * There may be an existing window (*xwinp) when this is called, in which case
 * it will be reparented.  Either way, the subwindow will be unmapped.
 *
 * If the parent is Seamless, *xwinp is protected by unmapping it and
 * reparenting to the root just before its parent is deleted.
 *
 * The return value indicates whether the parent (hwin) exists.
 * When it does not, this function still does its work
 * (including window creation), but uses the root window as a temporary parent.
 * A later call to MM_get_window() or MM_set_geometry() can be used
 * to reparent to any existing Seamless window.
 * MM_TWI_set_new_window_function() can be used to handle cases where
 * this function fails.
 *
 * If an invalid window identifier is returned (the special value None (zero)
 * for X11, it means that there is no supported window system.
 * Callers may then choose to fall back to MM_get_overlay_buffer().
 */

BOOLEAN MM_get_window(UINT32 hwin, UINT32 *xwinp);

/* This is essentially a clone of the previous function, but the window
 * that is created will be child of a client-owned subwindow, and the XEMBED
 * protocol can be used to communicate between the two.  The only current use
 * of this is to allow the embedded application to receive keyboard input.
 *
 * For any given embedded window, only one of these two functions may
 * be used - no mixing!
 *
 * The window id of the client-owned embedder is returned in *xparentp
 * to be used as the destination for XEMBED client messages from
 * the embedded application.  If this function is used for reparenting,
 * the embedding window is also reparented and returned as *xparentp.
 *
 * If xwinp is NULL, an empty socket is created, which can later be
 * occupied by creating a new subwindow of *xparentp or reparenting
 * into it.
 *
 * If an existing window is passed as *xwinp and that window is already
 * embedded, the embedding will be reparented to the window identified
 * by hwin.  Otherwise, if *xwinp does not carry a correct
 * _XEMBED_INFO property, the embedding is rejected - the function
 * returns FALSE and *xparentp is set to None.  
 */

BOOLEAN MM_get_XEMBED_window(UINT32 hwin, UINT32 *xwinp, UINT32 *xparentp);

/* Create a substitute for a window that can also be used in non-X11
 * environments.   The returned value for *xwinp is an opaque handle that
 * can be passed to other functions in this interface but has no other use.
 * As before, *xwinp may be non-zero on entry, causing "reparenting".
 *
 * A value of zero indicates that overlay buffers are not available for the
 * current session.  Otherwise, the contents of a mapped overlay buffer
 * will be blended into the session image either on demand,
 * or immediately following a call to the prepare function.
 * See the description of MM_push_overlay() for more information.
 *
 * The prepare argument may be NULL if double buffering is not used and
 * there is no need for the client to lock the buffer,
 * because updates occur only when the virtual channel code is executing.
 */

BOOLEAN MM_get_overlay_buffer(UINT32 hwin, UINT32 *xwinp, UINT32 buffer_flags,
                              int (*prepare)(UINT32 xwin, BOOLEAN lock));

/* Values for buffer_flags, above.
 *
 * When the MM_DOUBLE_BUFFER flag is set, twice as much memory as required by
 * the width and height is allocated, the second sub-buffer immediately follows
 * the first.
 *
 * When MM_BUF_STRIDE_OK, the width of the returned buffer may be larger
 * than requested: this may offer improved performance.
 *
 * When shared memory is requested, it should be marked for deletion as soon
 * as possible, typically once any external processes have attached.
 */

#define MM_DOUBLE_BUFFER     0x01       /* Double buffered. */
#define MM_BUF_STRIDE_OK     0x10       /* Buffer need not be contiguous. */
#define MM_BUF_NEED_SHM      0x11       /* Need a shared memory buffer. */

/* Set the size and position of an existing subwindow (xwin) of a session
 * window with server handle hwin.  The SIGNED_RECT window rectangle is in
 * session coordinates, not window-relative and consists of four signed
 * 32-bit ints for left, top, right, bottom.  The return value indicates
 * whether the parent (hwin) exists.  If the parent exists, the subwindow
 * will have the correct geometry and parent, and be mapped on return.
 */

BOOLEAN MM_set_geometry(UINT32 hwin, UINT32 xwin, SIGNED_RECT *rt);


/* Set the shape of the OS window xwin from a list of sorted rectangles.
 * Sorting is by first Y then X of top left corner, "YXSorted" in X11-speak.
 * The last argument should be FALSE to start a multi-part update,
 * TRUE for the following parts.
 */

void
MM_clip(UINT32 xwin, UINT32 count, SIGNED_RECT *rects, BOOLEAN extended);


/* Destroy a window created by MM_get_window() or MM_get_XEMBED_window(). */

void MM_destroy_window(UINT32 hwin, UINT32 xwin);

/* Control window visibility. */

void MM_map_window(UINT32 xwin, BOOLEAN visible);


/* Set and clear a callback function for Seamless window creation.
 * The callback argument will be the server window handle.
 * Typically this need only be used to handle the case where a command
 * arrives containing the handle of a Seamless window that does not yet exist,
 * so that MM_get_window() fails and processing must be deferred until the
 * window has been created.  It is good practice to establish a handler only
 * when needed and remove it as soon as possible.
 */

void MM_TWI_set_new_window_function(void (*)(UINT32));
void MM_TWI_clear_new_window_function(void (*)(UINT32));


/* Track the geometry of a Seamless window.  The client will call the
 * specified function whenever the window's size or position changes,
 * passing the argument specified for the callback and the new window rectangle.
 *
 * MM_TWI_set_geometry_callback() returns FALSE when the session is not
 * Seamless or the window does not (yet) exist.
 */

typedef void (*TWI_Geom_Fn)(UINT32 arg, const SIGNED_RECT *rt);

BOOLEAN
MM_TWI_set_geometry_callback(UINT32 hwin, TWI_Geom_Fn callback, UINT32 arg);
void
MM_TWI_clear_geometry_callback(UINT32 hwin, TWI_Geom_Fn callback, UINT32 arg);


/* *** Functions for overlay buffers only. ***/

struct special_buffer;     /* See mainloop.h (Plugin SDK.) */

/* Get the details of the allocated buffer. */

const struct special_buffer *MM_get_buffer(UINT32 xwin);

/* Supply the caller's own pixel buffer.  This is optional, as the client
 * normally allocates the buffer and may yield poorer performance.
 *
 * The structure supplied in the second argument will be accessed whenever the
 * buffer contents are rendered to screen.  (Union member
 * details->params.overlay_buffer, must contain valid data, with the shared
 * memory identifier set to -1 if the memory is not accessible via shmat().
 *
 * A NULL value for "details" may be used when an existing allocation
 * becomes invalid.
 */

void MM_set_buffer(UINT32 xwin, struct special_buffer *details);

/* Push the current contents of the pixel buffer to the screen.
 * The return value indicates whether that actually occurred, otherwise,
 * expect a call to the overlay buffer's prepare() function soon.
 * (The function is an optional argument to MM_get_overlay_buffer.)
 *
 * The frame argument will be zero, unless multiple buffers are in use,
 * then it is the current buffer index.
 *
 * The prepare function will be called twice, the first time the lock
 * argument will be TRUE.  When double-buffering has been requested,
 * the return value from the call is the index (0 or 1) of the sub-buffer
 * to be drawn.  The image will be rendered immediately after
 * the function returns, and the function will be called again with the
 * lock argument FALSE.  The buffer must not be modified between the two calls.
 *
 * If the buffer is not in a consistent state when the prepare function,
 * is called with (lock == TRUE), it may return -1.  Once consistent buffer
 * contents are available, MM_push_overlay() should be called.
 */

BOOLEAN MM_push_overlay(UINT32 xwin, UINT32 frame);

/* *** Low-level functions. ***/

/* Return the OS (X windows) handle for a window with
 * server handle hwin.  If hwin is zero, the XID of the session window
 * is returned.  Otherwise, the return value will be the XID of either
 * the single session window (non-Seamless case) or a Seamless window.
 * Optionally, translate a rectangle in session coordinates to window-relative.
 */

UINT32 MM_translate_window(UINT32 hwin, SIGNED_RECT *rt);


/* Alternative way to get an X Window ID from the server handle.
 * If it is a Seamless window, create a subwindow to carry the focus - this is
 * to do what the XEMBED spec calls "... beat X11 with its own weapons."
 * The idea is to ensure that keyboard events are delivered to the client,
 * not an embedded application, even when the pointer is inside the embedded
 * window.
 */

UINT32 MM_prepare_window(UINT32 hwin);


/* Unprotected version of MM_get_window(), otherwise identical. */

BOOLEAN MM_get_unprotected_window(UINT32 hwin, UINT32 *xwinp);


/* Enable monitoring of a Seamless window with server handle hwin for deletion.
 * The second argument will be passed to the callback function.
 */

void
MM_TWI_set_deletion_call(UINT32 hwin, void (*cb_func)(UINT32), UINT32 arg);


/* Stop monitoring.  Unneccessary before MM_Destroy_window. */

void
MM_TWI_clear_deletion_call(UINT32 hwin, void (*cb_func)(UINT32), UINT32 arg);


/* Clear all deletion callbacks with matching parameters. */

void MM_TWI_clear_deletion_calls(void (*cb_func)(UINT32), UINT32 arg);

#endif /* _SUBWINDOW_H_ */
