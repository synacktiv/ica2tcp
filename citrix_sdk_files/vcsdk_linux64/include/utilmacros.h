/***************************************************************************
*
*   utilmacros.h
*
*   This file contains various useful macro definitions.
*
*   These macros are common to all platforms and so MUST be completely
*   portable.
*
*   $Id$
*
*   Copyright 2005-2012 Citrix Systems, Inc.  All Rights Reserved.
*
****************************************************************************/

#ifndef _UTILMACROS_H_
#define _UTILMACROS_H_


#define CLEAR_STRING(string)                (string)[0] = '\0'
#define IS_EMPTY_STRING(string)             ((string)[0] == '\0')

// Macros for safe operations on a buffer. These rely on sizeof being able to
// work on the destination.
#define STRNCPY_TO_BUFFER(d, s) (d[sizeof(d) - 1] = '\0'), strncpy(d, s, sizeof(d) -1)
#define CLEAR_BUFFER(d) memset(d, '\0', sizeof(d))

// Macro for safe copy with the size of the destination as a separate parameter.
#define STRNCPY_TO_PTR(d, l, s) (d[l-1] = '\0'), strncpy(d, s, l-1)

// Buffer And Size Parameters macro to conveniently add the sizeof a buffer
// to a parameter list
#define BASP(buf)               buf, sizeof(buf)

#define ELEMENTS_IN_ARRAY(array)            (sizeof(array) / sizeof((array)[0]))

/* Calculate the offset in bytes of a structure member. */

#define MEMBER_OFFSET(member, struct_type) \
    ((int)(&(((struct_type *)0)->member)))

/* N.B. the argument to this macro MUST be a string literal, so that the
 *      sizeof operator gives the correct result (if the argument were a
 *      string pointer, sizeof would give the size of the pointer!) - so
 *      concatenate the argument with an empty string, which is syntactically
 *      legal only if that argument is indeed a literal!
 */
#define LITERAL_LENGTH(literal)             (sizeof("" literal) - 1)

/* ASSERT a value while dodging an "set but not used" warning. */

#ifdef DEBUG
#define ASSERT_EQUAL(expr, value, type) \
    {type okapi; ASSERT((okapi = (expr)) == value, okapi); }
#else
#define ASSERT_EQUAL(expr, value, type) expr
#endif

/* ICA uses the Microsoft separator int file paths. */

#define ICA_PATH_SEPARATOR '\\'

#endif /* _UTILMACROS_H_ */
