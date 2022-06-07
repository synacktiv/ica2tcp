/************************************************************************
*
*   mainloop.h
*
*   Interface to plug-in main loop/graphics/UI modules.
*
*   $Id$
*
*   Copyright 2010-2017 Citrix Systems, Inc.  All Rights Reserved.
*
*************************************************************************/

#ifndef _MAINLOOP_H_
#define _MAINLOOP_H_

#include <stdarg.h>     /* For va_list. */

#ifdef X11_SUPPORT
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

enum _EIDialogType;     /* Pre-declaration to suppress warnings. */
struct dialogExtra;
struct image_overlay;

/* This header defines the interface to pluggable graphics and event loop
 * modules used by the ICA engine (wfica) component of the Citrix
 * "Receiver for Linux".  This is currently used in the Linux client but
 * should be fully portable.  The design supports selection of a
 * graphics and/or UI implementation at run time, via dynamic loading
 * of a binary library.
 *
 * During initialisation, the engine will attempt to load a shared library
 * with the default name KVMEPlugin.so, and call its Plugin_init()
 * entry point to set up the connection between program and plugin.
 * 
 * Ideally, the event loop implementation, graphics, interactive input
 * and UI library would all be handled separately, but many UI libraries
 * have their own event loop, and this interface assumes that.
 */

/* ************************ Event loop interfaces. ************************ 
 *
 * The idea here is to provide a generic implementation of the
 * Thrd/Tmt/Evt functions of the client PAL layer, built on a simpler
 * interface to the pluggable event/graphics/UI layer.
 */
 
/* Timers and I/O ready events call back to their originator when triggered. */

struct pal_callback {
    void (*pfnDeliver)(void *, void *);         /* Function to call. */
    void  *pSubscriber;                         /* Argument to pass. */
};

#define CALL_IT(s) \
    (((s)->callback).pfnDeliver(((s)->callback).pSubscriber, (s)))

/* EVT structures represent requests for callback on I/O readiness.
 * It is just a container for callback details with some
 * minimal information to prevent deletion while active.
 * An EVT may be used with only a single file descriptor,
 * for read, write, or both.
 */

struct pal_event {
    struct pal_callback  callback;
    void                *priv;               /* For plugin implementor. */
};


/* Structure passed to the plugin's draw() function,
 * to copy a block of data to the screen.
 * *** Current builds will always set bytes_per_pixel to 4, with
 * *** little-endian, blue/green/red/zero pixel byte order.
 */

struct Draw_rect {
#ifdef X11_SUPPORT
    union {                                  /* LVB - may be None/NULL. */
        Pixmap  x_pixmap;  /* Session image in X server. */
        XImage *x_image;   /* Session image in local memory. */
    }                   source;
    Window              dest_win;            /* Destination window. */
    UINT32              target_x, target_y;  /* Within destination. */
#endif
    UINT32              x, y, width, height; /* Rectangle to draw. */
    void               *pdata;               /* Start of buffer. */
    int                 stride;              /* Row length of data in bytes. */
    int                 bytes_per_pixel;
};

/* Structure used to specify session size and depth (bits/pixel). */

struct session_size {
    unsigned int width, height, depth;
};

/* Structure to describe memory allocated by special_allocate().
 * On return, the allocation may have been increased in some cases.
 * The "priv" member will be null when the function is called and will
 * never be modified by the caller.  The identical structure is passed to
 * special_free() to release the memory.
 */


enum special_mem_type {
    session_image, /* As it appears on-screen; may be the frame buffer.
                    * Size is in pixels.
                    * Width and height may be increased for alignment.
                    * This is also a target for image decompression.
                    */
    image_output,  /* Non-visible decompressed image data, otherwise
                    * like session_image.
                    */
    image_cache,   /* Target for decompressed image data that will be
                    * copied to the session image.  Output is aligned
                    * on 8-byte boundaries.
                    */
    image_input,   /* Compressed image data - no alignment. */
    overlay,       /* Buffer for an image to overlay the session.
                    * Used only if draw_overlay() is defined.
                    * If the MM_DOUBLE_BUFFER flag is set, twice as much
                    * memory as required by the width and height is
                    * allocated, the second sub-buffer immediately follows
                    * the first.
                    */
    small_bitmap   /* Small bitmap image data, used for lossless objects in
                    * H.264 mode. Receiver allocates the buffer and the
                    * decoder is responsible for freeing it.
                    * Only passed if KVME_VERSION >= 1.2.
                    */
};
           
struct special_buffer {
    enum special_mem_type           type;       /* In. */
    void                           *buf_addr;   /* Out. */
    void                           *priv;       /* For plugin implementor's
                                                   private use.
                                                 */
    union {
        /* Types session_image, image_output, small_bitmap allocate an 
         * image buffer. 
         */
        struct image_buffer_params {
            unsigned int        pixel_size;     /* In, bytes. */
            unsigned int        width, height;  /* In/Out, pixels. */
            int                 shm_id;         /* Out - used only with X11
                                                   and type == session_image,
                                                   or type == overlay.
                                                 */
        } image_buffer;

        /* Types image_cache and image_input are simple memory blocks. */
        struct {
            unsigned int        size;       /* In/Out, bytes. */
        } memory_buffer;

        /* Buffer for an overlay image. */

        struct overlay_params {
            struct image_buffer_params  buffer;
            UINT32                      flags;  /* See subwindow.h. */
        } overlay_buffer;
    } params;
};
    
/* Structure used to set hardware colour table entries. */

struct ctx_colour {unsigned char blue, green, red, dummy;};

/* This structure holds the details of the link between program and plugin. */

struct mainloop_plugin {
/* Interface version: the usual convention applies: the ICA engine program
 * will reject a plugin with a different major version or lower minor
 * version than its own.
 */

    UINT32 major_version, minor_version;

#define KVME_MAJOR_VERSION    1
#define KVME_MINOR_VERSION    3

/* Plugin flags, indicating special features of the plugin.
 * There are currently separate X11 and non-X11 builds of the ICA engine.
 * The X11 build will use only plugins that declare X11 support and vice-versa.
 * X11 plugins may choose to support multi-windowed sessions ("Seamless")
 * but need not do so.   Note that X11 plugins currently have very limited
 * function, see Plugin_init() below.
 */

    UINT32 plugin_flags;

#define PLUGIN_SUPPORTS_X11     1       /* For loading by X11 client build. */
#define PLUGIN_MULTI_WINDOWED   2       /* For "Seamless" sessions. */
#define PLUGIN_ANY_OVERLAY      4       /* See draw_overlay(), below. */

#ifdef X11_SUPPORT
    Display             *disp;          /* Target X11 display. */
#endif

/* Initialisation, setup and shutdown entry points. */

    /* DEPRECATED - This function will no longer be called, can be NULL. */
    /* Plugin function to create a detached thread, here because some libraries
     * (SDL is an example) have their own threading interface.
     * Returns success/failure.
     */

    BOOLEAN (*create_thread)(int min_stack,
                             void (*thread_proc)(void *), void *argument);


    /* Plugin function to return maximum session size and depth.
     * The returned depth should be 8, 16, 32, indicating the highest
     * drawing buffer depth that should be used.  This may be called more
     * than once, but will be called at least once before start_video().
     */

    void (*get_screen_size)(struct session_size *info);


    /* Plugin function to start displaying video.
     * It will be called again if the session dimensions have changed;
     * they will not be larger than the maximum.
     * The return value indicates whether the video image needs to be
     * pushed to the screen (double-buffering).  If the function returns
     * FALSE, the plugin is assumed to be exposing the framebuffer directly:
     * special_allocate() must be defined and the draw() function
     * will not be called.
     */

    BOOLEAN (*start_video)(struct session_size *info);


    /* Allocate memory for special purposes, for example the frame buffer or
     * contiguous physical memory to be accessed by a hardware decoder.
     * All input and output memory for decoders can be allocated.
     *
     * This function is optional.
     *
     * A NULL buffer address on return will be treated as a fatal error,
     * so implementations should fall back to malloc() whenever possible.
     *
     * This does not apply to allocations of "overlay" type, where the
     * function may return a NULL buffer pointer if it can not accomodate
     * the options described in the flags.
     *
     * Type session_image is another exception for X11 builds: if NULL is
     * returned the program will fall back appropriately.
     * Otherwise, and if the draw() function is not specified, the
     * allocated memory must be defined as a System V shared memory area,
     * attached at address buf_addr and the identifer returned in shm_id.
     * That enables the use of the MIT shared-memory X11 extension for drawing.
     * The main program will call shmdt() before freeing this memory.
     */

    void (*special_allocate)(struct special_buffer *description);


    /* Free memory allocated by special_allocate(). */

    void (*special_free)(struct special_buffer *description);


    /* Cleanup function called shortly before program exit. */

    void (*close_mainloop)(void);

/* Event loop stuff. */

    /* Plugin function to run the event loop.  It never returns.
     * See the description of Do_work() below for restrictions on when the
     * event loop may block.
     * This is called with appropriate signals (SIGCHLD, SIGUSR1, SIGUSR2)
     * blocked.  The event loop must unblock SIGCHLD periodically, and may
     * use the other two signals for cross-thread wakeups.
     */

    void (*run)(void);


    /* Plugin function to start a single-shot timer.
     * Units are milliseconds.
     * It calls back on completion and the callback's return value is
     * the next event loop timeout period, or zero to stop.
     */

    void (*start_timer)(UINT32 wait, UINT32 (*timer_done)(void));


    /* Plugin function to register and remove callbacks for I/O readiness
     * in the style of select() and poll().
     * Only read and write operations are supported.
     *
     * The first argument supplies the function to be called on I/O ready,
     * and its argument.  If NULL, a previous request is to be removed.
     * The memory referenced by this argument can be assumed to be valid
     * for the lifetime of the callback request.
     *
     * The return values are the EVT_XXXX definitions from ctxnevt.h.
     */

    VPSTATUS (*select_fd)(struct pal_event *evt_p, int fd, BOOLEAN is_write);


    /* Plugin entry point to call a function from the event loop,
     * after user input and I/O ready events have been processed, but before
     * the program blocks for the next event.  The return value indicates
     * whether the function should be called again (TRUE) or the request
     * removed (FALSE).
     * *** This is currently used only by the X11 build. ***
     */

    void (*idle_call)(BOOLEAN (*func)(void *), void *arg);


    /* Plugin function to wake the main loop from another thread.
     * When woken, On_wake() should be called.
     * The plugin may use SIGUSR1 to implement this.
     */

    void (*wake_main)(void);


/* Graphics stuff. */

    /* Plugin function to copy from the session image to screen. May be NULL,
     * particularly when using X11 or direct frame buffer output.
     * The caller guarantees that the rectangle fits in the session.
     * If possible, updates should be batched, and pushed to the screen
     * when flush is TRUE.  If possible, this function should not block.
     */

    void (*draw)(struct Draw_rect *rect, BOOLEAN flush);


    /* This optional function must only be defined when draw() is defined.
     * The return value indicates that previous calls to draw() have completed
     * sufficiently that modifications to the session image will not be
     * visible until after the next call to draw().  If the argument is TRUE,
     * the function should block until it can return TRUE,
     * otherwise it should not block.
     */

    BOOLEAN (*draw_complete)(BOOLEAN wait);


    /* This function is only significant when the depth returned by
     * get_screen_size() is 8, otherwise it may be NULL.
     * So it is not used by current builds.
     * When present, it is then assumed that a hardware palette
     * (or colormap in X11 terminology) with 256 entries is supported.
     */

     void (*set_palette)(struct ctx_colour *);


    /* Show a dialogue box and return the user's choice. 
     * A new dialogue replaces the previous one.
     * Buttons (up to 3) are ordered right-to-left by the
     * zero-terminated array, buttons.
     * 
     * Additional UI elements beyond text and basic buttons are specified
     * by the "extra" argument.  At version 1.0 of this interface,
     * any such dialogues may be ignored, by immediately returning
     * the default response, and configurable defaults will be used
     * in place of user input.
     *
     * **** Subject to change. ****
     */

    enum _button_type (*show_dialogue)(enum _EIDialogType type,
                                       char              *title,    /* UTF-8 */
                                       char              *body,
                                       const enum _button_type
                                                          *buttons, /* Array */
                                       enum _button_type  default_response,
                                       char              *icon,     /* File */
                                       struct dialogExtra
                                                         *extra);   /* Array */

/* Cursor functions - all optional. */

    /* Create a cursor image to be used as a mouse pointer.  The plugin_cursor
     * structure is deliberately undefined, so that the plugin implementation
     * can define it.  The cursor is drawn in what is believed to be the
     * (undocumented) MS Windows style: dest = (dest & mask) ^ image.
     * Some graphics systems may be unable to do that, and must use an
     * approximation.  The X11 implemention draws non-black parts of the image
     * inverted by a stipple pattern in areas where the mask is zero.
     *
     * Both image and mask are presented at the same depth as session drawing,
     * but the mask contains only black (0) and white (0xFFFFFF) pixels.
     * If is_mono is TRUE, the image contains only those pixels as well.
     *
     * The image data pointed to directly and indirectly by the arguments
     * need not be copied.  It will remain valid until after the cursor has
     * been destroyed.
     *
     * The position members of the image argument (x, y) specify the location
     * of the pixel that aligns with the reported cursor position (hotspot).
     * The actual image and mask begin at the origin (0, 0).
     *
     * The function Enable_software_cursor() (below) is one way to avoid
     * specifying these entry points.
     */

    struct plugin_cursor *(*create_cursor)(struct Draw_rect *image,
                                           struct Draw_rect *mask,
                                           BOOLEAN is_mono);


    /* Destroy a cursor. */

    void (*destroy_cursor)(struct plugin_cursor *cursor);


    /* Use a cursor. A NULL argument means that no cursor should be visible. */

    void (*use_cursor)(struct plugin_cursor *cursor);


    /* If the cursor must be explicitly redrawn each time it is moved,
     * this entry point should be defined; otherwise leave NULL.
     */

    void (*draw_cursor)(int x, int y);


    /* Set the pointer position. */

    void (*warp_cursor)(int x, int y);

/* Accelerated helper routines. */
    
    /* Optional function to do an optimised row copy with stride support. 
     * dst_stride, src_stride and row_width are all byte counts;
     * dst_stride and src_stride may be negative.
     */

    void (*copy_rows)(void *dst, int dst_stride, void *src, int src_stride,
                      unsigned int row_width, unsigned int num_rows);

/* Other. */

    /* Query the keyboard state.  This is optional.
     * The flag definitions are compatible with
     * Linux's header, /usr/include/linux/kd.h.     */

    unsigned int (*query_keyboard)(void);

#ifndef LED_SCR
#define LED_SCR 0x01    /* Scroll lock is on. */
#define LED_NUM	0x02	/* Num lock. */
#define LED_CAP	0x04	/* Caps lock. */
#endif /* LED_SCR */

    /* Overlay the session image with image data from another buffer.
     * The image_overlay structure is defined in image_overlay.h.
     * This is to support virtual channels that create their own graphics
     * output and blend it into the session image.  See subwindow.h and
     * the Virtual Channel SDK.
     *
     * This function will be called only when the buffer has been allocated
     * by the plugin, unless the PLUGIN_ANY_OVERLAY flag has been set
     * to indicate that any buffer is acceptable.
     */

    void (*draw_overlay)(struct image_overlay *over);

    /* Provide a logging facility with interface similar to vsyslog
     * (/usr/include/syslog.h).  Optional, but recommended when
     * fprintf(stderr, ...) does nothing useful.
     * If syslog itself is the preferred method, this function should
     * not be defined, as logging to syslog is a standard feature.
     */

    void (*logger)(int level, const char *format, va_list params);
};

/* Client entry points called from the plugin.  See simplified.h for the
 * entry points to be called with keyboard and mouse inputs.
 */

/* Read a client configuration parameter, stored as a key/value pair.
 * Plugins should use a distinctive prefix in key names.
 */

#define MAX_CONF_STR_LEN 256    /* Size for "value" argument below. */
void GetStrConfParam(const char *key, char *value);
int  GetIntConfParam(const char *key);

/* Client function to be called on cross-thread wake. */

extern void On_wake(void);

/* Client function to perform work not triggered by a timer or I/O ready.
 * It should be called whenever the event loop wakes from a blocking call.
 * The event loop MUST not block, unless the most recent call to this function
 * returned FALSE.
 *
 * One method is for the event loop to call Do_work() immediately
 * before sleeping.  When it returns TRUE, the next pass of the event loop
 * should check for I/O readiness with zero timeout.
 *
 * An alternative is to inject a dummy event or queue an idle callback
 * whenever this function returns TRUE.
 */

extern BOOLEAN Do_work(void);

/* Client function to deal with signals.  If possible, the client should run
 * with handled signals blocked so they should only occur inside the event loop,
 * waking it when blocked.  This function should be called
 * promptly after being woken by a signal.
 */

extern void Take_signal(void);

/* Provide updated state of keyboard locking shifts in same format as
 * query_keyboard() above.  This will typically be called when focus is
 * acquired, either initially or when it is recovered, but not when the user
 * presses a locking shift key.
 */

extern void Update_keyboard(unsigned int);

/* Request that the session be resized, by getting the server to do it.
 * If "now" is TRUE, the request is sent as soon as possible.
 * If the user is manually resizing a window,
 * the "now" argument should be FALSE, as that enables a timer 
 * to limit the rate of sending requests to the server.
 * Resizing is an expensive operation, and may silently fail.
 */

extern void Send_session_resize(int width, int height, BOOLEAN now);

/* This function may be used to set the show_dialogue member during the
 * early stages of development.  It uses fprintf() to show the dialogue
 * details on stdout, and assumes the user selected the default action.
 */

extern enum _button_type stub_dialogue_handler(enum _EIDialogType,
                                               char *, char *,
                                               const enum _button_type *,
                                               enum _button_type,
                                               char *,
                                               struct dialogExtra *);

/* Use the client's own code to draw a cursor image directly in the
 * session image buffer.  This provides all the cursor-related entry
 * points, except for warp-cursor.
 */

extern void Enable_software_cursor(void);

/* Fetch a count of milliseconds since the program started.
 * This may be useful when implementing start_timer().
 */

#ifndef _CTXOS_H_       /* FIX ME */
extern UINT32 Os_GetTickCountInMs(void);
#endif

/* Entry point to initialise plugin.
 *
 * The plugin gets an early chance to examine the environment and
 * the configuration parameters.
 * It should be possible for it to read command line parameters, but the
 * current code for processing those prevents that.
 *
 * If initialisation fails, the function should display a diagnostic message
 * and exit.
 *
 * For X11 builds, only the special_allocate(), special_free(), draw(),
 * draw_complete() and copy_rows() plugin entry points will be used,
 * others need not be set.  Plugins for X11 should not call any
 * of the host program entry points declared above, other than the
 * GetXXXConfParam() functions.
 */

extern void Plugin_init(struct mainloop_plugin *plug);

#if defined(NCS) || defined(PROXYLIB) || defined(WEB_R_TEST)
/* Private to Citrix ICA engine. Plugins do not use! */
extern struct mainloop_plugin Plug;
#endif /* NCS || LIBPROXY */

/* Optional entry points for plugin implementation-support code. */

void Syscall_loop_init(struct mainloop_plugin *plug); /* Use epoll(2) loop. */
#endif /* _MAINLOOP_H_ */
