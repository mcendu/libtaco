/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_SHIFTJIS_H_
#define TJA_SHIFTJIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "io.h"
#include "taco.h"
#include <stdbool.h>
#include <stddef.h>

/*
 * Creates a filter on a file that converts it to UTF-8.
 *
 * If the source character set is not specified (i.e. NULL), it is assumed
 * to be in Shift_JIS per TJA scene convention.
 */
extern taco_file *tja_iconv_open_(taco_allocator *alloc, taco_file *file,
                                  const char *src_charset);

/* Check if the stream is valid UTF-8. */
extern bool tja_is_file_utf8_(taco_file *file, taco_allocator *alloc);

#ifdef __cplusplus
}
#endif

#endif /* !TJA_SHIFTJIS_H_ */
