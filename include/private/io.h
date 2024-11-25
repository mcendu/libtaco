/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_IO_H_
#define TAIKO_IO_H_

#include "taiko.h"

#include <stdarg.h>
#include <stdio.h>

typedef struct taiko_file_ taiko_file;

extern taiko_file *taiko_file_open_(taiko_allocator *alloc, void *stream,
                                    taiko_read_fn *read, taiko_write_fn *write,
                                    taiko_close_fn *close,
                                    taiko_printf_fn *printf);
extern taiko_file *taiko_file_open_path_(const char *path, const char *mode);
extern taiko_file *taiko_file_open_stdio_(FILE *file);
extern taiko_file *taiko_file_open_null_(taiko_allocator *alloc);
extern void taiko_file_close_(taiko_file *file);

extern size_t taiko_file_read_(taiko_file *file, void *dst, size_t size);
extern size_t taiko_file_write_(taiko_file *file, const void *src, size_t size);
extern int taiko_file_printf_(taiko_file *file, const char *format, ...)
    TAIKO_PRINTF(2, 3);
extern int taiko_file_vprintf_(taiko_file *file, const char *format,
                               va_list arg) TAIKO_PRINTF(2, 0);

extern taiko_file *taiko_get_stderr_();

#endif /* !TAIKO_IO_H_ */
