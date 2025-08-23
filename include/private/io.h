/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_IO_H_
#define TACO_IO_H_

#include "taco.h"

#include <stdarg.h>
#include <stdio.h>

typedef struct taco_file_ taco_file;

extern taco_file *taco_file_open_(taco_allocator *alloc, void *stream,
                                  const char *filename,
                                  const taco_io *callbacks);
extern taco_file *taco_file_open_path_(const char *path, const char *mode);
extern taco_file *taco_file_open_stdio_(FILE *file);
extern taco_file *taco_file_open_null_(taco_allocator *alloc);
extern void taco_file_close_(taco_file *file);

extern const char *taco_file_name_(const taco_file *file);
extern void taco_file_set_name_(taco_file *file, const char *filename);

extern size_t taco_file_read_(taco_file *file, void *dst, size_t size);
extern size_t taco_file_write_(taco_file *file, const void *src, size_t size);
extern int taco_file_printf_(taco_file *file, const char *format, ...)
    TACO_PRINTF(2, 3);
extern int taco_file_vprintf_(taco_file *file, const char *format, va_list arg)
    TACO_PRINTF(2, 0);
extern int taco_file_seek_(taco_file *file, uint64_t offset, int whence);

extern taco_file *taco_get_stderr_();

#endif /* !TACO_IO_H_ */
