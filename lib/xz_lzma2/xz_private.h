/*
 * Private includes and definitions
 *
 * Author: Lasse Collin <lasse.collin@tukaani.org>
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */

#ifndef XZ_PRIVATE_H
#define XZ_PRIVATE_H

#include "xz_config.h"


/* If no specific decoding mode is requested, enable support for all modes. */
#if !defined(XZ_DEC_SINGLE) && !defined(XZ_DEC_PREALLOC) \
		&& !defined(XZ_DEC_DYNALLOC)
#	define XZ_DEC_SINGLE
#	define XZ_DEC_PREALLOC
#	define XZ_DEC_DYNALLOC
#endif

/*
 * The DEC_IS_foo(mode) macros are used in "if" statements. If only some
 * of the supported modes are enabled, these macros will evaluate to true or
 * false at compile time and thus allow the compiler to omit unneeded code.
 */
#ifdef XZ_DEC_SINGLE
#	define DEC_IS_SINGLE(mode) ((mode) == XZ_SINGLE)
#else
#	define DEC_IS_SINGLE(mode) (false)
#endif

#ifdef XZ_DEC_PREALLOC
#	define DEC_IS_PREALLOC(mode) ((mode) == XZ_PREALLOC)
#else
#	define DEC_IS_PREALLOC(mode) (false)
#endif

#ifdef XZ_DEC_DYNALLOC
#	define DEC_IS_DYNALLOC(mode) ((mode) == XZ_DYNALLOC)
#else
#	define DEC_IS_DYNALLOC(mode) (false)
#endif

#if !defined(XZ_DEC_SINGLE)
#	define DEC_IS_MULTI(mode) (true)
#elif defined(XZ_DEC_PREALLOC) || defined(XZ_DEC_DYNALLOC)
#	define DEC_IS_MULTI(mode) ((mode) != XZ_SINGLE)
#else
#	define DEC_IS_MULTI(mode) (false)
#endif

/*
 * Allocate memory for LZMA2 decoder. xz_dec_lzma2_reset() must be used
 * before calling xz_dec_lzma2_run().
 */
struct xz_dec_lzma2 *xz_dec_lzma2_create(enum xz_mode mode,
						   uint32_t dict_max);

/*
 * Decode the LZMA2 properties (one byte) and reset the decoder. Return
 * XZ_OK on success, XZ_MEMLIMIT_ERROR if the preallocated dictionary is not
 * big enough, and XZ_OPTIONS_ERROR if props indicates something that this
 * decoder doesn't support.
 */
enum xz_ret xz_dec_lzma2_reset(struct xz_dec_lzma2 *s,
					 uint8_t props);

/* Decode raw LZMA2 stream from b->in to b->out. */
enum xz_ret xz_dec_lzma2_run(struct xz_dec_lzma2 *s,
				       struct xz_buf *b);

/* Free the memory allocated for the LZMA2 decoder. */
void xz_dec_lzma2_end(struct xz_dec_lzma2 *s);


#endif
