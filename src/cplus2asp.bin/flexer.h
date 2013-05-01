#ifndef flexHEADER_H
#define flexHEADER_H 1
#define flexIN_HEADER 1

#line 6 "flexer.h"
#line 25 "flexer.ll"
#include <string>
#include "parser.h"
#include "flexertypes.h"
#include "ltsglobals.h"

#ifdef flexIN_HEADER
/**
 * @file flexer.h
 * @brief Contains rules and definitions used by flex to create a lexical analyzer.
 */

extern YYSTYPE yyxlval;
extern YYLTYPE yyxlloc;

extern int flexerLineNum;
extern int flexerCharPos;
extern std::string flexerFileName;

extern flexerValueType flexeryylvalType;

/**
 * Outputs an error message to stderr.
 * Automatically adds file and line number information to the message.
 * @param msg - The specific message to add after the error header info.
 */
void flexerError(char const *msg);

/**
 * Resets the internal state of the flexer to initial values.
 */
void flexerReset();
#endif



#line 43 "flexer.h"

#define  YY_INT_ALIGNED short int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5
#define YY_FLEX_SUBMINOR_VERSION 35
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif

/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* end standard C headers. */

/* flex integer type definitions */

#ifndef FLEXINT_H
#define FLEXINT_H

/* C99 systems have <inttypes.h>. Non-C99 systems may or may not. */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

/* C99 says to define __STDC_LIMIT_MACROS before including stdint.h,
 * if you want the limit (max/min) macros for int types. 
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;

/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#endif /* ! C99 */

#endif /* ! FLEXINT_H */

#ifdef __cplusplus

/* The "const" storage-class-modifier is valid. */
#define YY_USE_CONST

#else	/* ! __cplusplus */

/* C99 requires __STDC__ to be defined as 1. */
#if defined (__STDC__)

#define YY_USE_CONST

#endif	/* defined (__STDC__) */
#endif	/* ! __cplusplus */

#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst
#endif

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

extern int flexleng;

extern FILE *flexin, *flexout;

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */
    
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

void flexrestart (FILE *input_file  );
void flex_switch_to_buffer (YY_BUFFER_STATE new_buffer  );
YY_BUFFER_STATE flex_create_buffer (FILE *file,int size  );
void flex_delete_buffer (YY_BUFFER_STATE b  );
void flex_flush_buffer (YY_BUFFER_STATE b  );
void flexpush_buffer_state (YY_BUFFER_STATE new_buffer  );
void flexpop_buffer_state (void );

YY_BUFFER_STATE flex_scan_buffer (char *base,yy_size_t size  );
YY_BUFFER_STATE flex_scan_string (yyconst char *yy_str  );
YY_BUFFER_STATE flex_scan_bytes (yyconst char *bytes,int len  );

void *flexalloc (yy_size_t  );
void *flexrealloc (void *,yy_size_t  );
void flexfree (void *  );

/* Begin user sect3 */

#define flexwrap(n) 1
#define YY_SKIP_YYWRAP

extern int flexlineno;

extern char *flextext;
#define yytext_ptr flextext

#ifdef YY_HEADER_EXPORT_START_CONDITIONS
#define INITIAL 0
#define BLK_SCOMMENT 1
#define LINE_COMMENT 2
#define SINGLE_QUOTE_STR 3
#define DOUBLE_QUOTE_STR 4
#define RAW_ASP_CP 5
#define RAW_ASP 6
#define RAW_LUA_CP 7
#define RAW_LUA 8
#define RAW_F2LP_CP 9
#define RAW_F2LP 10

#endif

#ifndef YY_NO_UNISTD_H
/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 */
#include <unistd.h>
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

/* Accessor methods to globals.
   These are made visible to non-reentrant scanners for convenience. */

int flexlex_destroy (void );

int flexget_debug (void );

void flexset_debug (int debug_flag  );

YY_EXTRA_TYPE flexget_extra (void );

void flexset_extra (YY_EXTRA_TYPE user_defined  );

FILE *flexget_in (void );

void flexset_in  (FILE * in_str  );

FILE *flexget_out (void );

void flexset_out  (FILE * out_str  );

int flexget_leng (void );

char *flexget_text (void );

int flexget_lineno (void );

void flexset_lineno (int line_number  );

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int flexwrap (void );
#else
extern int flexwrap (void );
#endif
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy (char *,yyconst char *,int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * );
#endif

#ifndef YY_NO_INPUT

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k */
#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif /* __ia64__ */
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1

extern int flexlex (void);

#define YY_DECL int flexlex (void)
#endif /* !YY_DECL */

/* yy_get_previous_state - get the state just before the EOB char was reached */

#undef YY_NEW_FILE
#undef YY_FLUSH_BUFFER
#undef yy_set_bol
#undef yy_new_buffer
#undef yy_set_interactive
#undef YY_DO_BEFORE_ACTION

#ifdef YY_DECL_IS_OURS
#undef YY_DECL_IS_OURS
#undef YY_DECL
#endif

#line 645 "flexer.ll"


#line 379 "flexer.h"
#undef flexIN_HEADER
#endif /* flexHEADER_H */
