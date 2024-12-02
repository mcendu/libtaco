// SPDX-License-Identifier: BSD-2-Clause
#include "io.h"

#include "alloc.h"
#include "taco.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

struct taco_file_ {
  taco_allocator *alloc;
  void *stream;
  char *filename;
  taco_read_fn *read;
  taco_write_fn *write;
  taco_close_fn *close;
  taco_printf_fn *printf;
};

taco_file *taco_file_open_(taco_allocator *alloc, void *stream,
                             const char *filename, taco_read_fn *read,
                             taco_write_fn *write, taco_close_fn *close,
                             taco_printf_fn *printf) {
  taco_file *f = taco_malloc_(alloc, sizeof(taco_file));
  if (!f)
    return NULL;

  f->alloc = alloc;
  f->stream = stream;
  f->filename = taco_strdup_(alloc, filename);
  f->read = read;
  f->write = write;
  f->close = close;
  f->printf = printf;
  return f;
}

taco_file *taco_file_open_path_(const char *path, const char *mode) {
  FILE *f = fopen(path, mode);
  if (!f)
    return NULL;

  taco_file *result =
      taco_file_open_(&taco_default_allocator_, f, path,
                       (taco_read_fn *)fread, (taco_write_fn *)fwrite,
                       (taco_close_fn *)fclose, (taco_printf_fn *)vfprintf);
  if (!result) {
    fclose(f);
    return NULL;
  }

  return result;
}

taco_file *taco_file_open_stdio_(FILE *file) {
  return taco_file_open_(&taco_default_allocator_, file, "<stream>",
                          (taco_read_fn *)fread, (taco_write_fn *)fwrite,
                          NULL, (taco_printf_fn *)vfprintf);
}

taco_file *taco_file_open_null_(taco_allocator *alloc) {
  if (!alloc)
    alloc = &taco_default_allocator_;

  return taco_file_open_(alloc, NULL, "<null>", NULL, NULL, NULL, NULL);
}

void taco_file_close_(taco_file *file) {
  if (file->close)
    file->close(file->stream);

  taco_free_(file->alloc, file->filename);
  taco_free_(file->alloc, file);
}

const char *taco_file_name_(const taco_file *file) { return file->filename; }

void taco_file_set_name_(taco_file *file, const char *filename) {
  taco_free_(file->alloc, file->filename);
  file->filename = taco_strdup_(file->alloc, filename);
}

size_t taco_file_read_(taco_file *file, void *dst, size_t size) {
  if (file->read)
    return file->read(dst, 1, size, file->stream);
  return 0;
}

size_t taco_file_write_(taco_file *file, const void *src, size_t size) {
  if (file->write)
    return file->write(src, 1, size, file->stream);
  return 0;
}

int taco_file_printf_(taco_file *file, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = taco_file_vprintf_(file, format, ap);
  va_end(ap);

  return result;
}

int taco_file_vprintf_(taco_file *file, const char *format, va_list arg) {
  if (file->printf)
    return file->printf(file->stream, format, arg);

  // without a dedicated printf, format in memory and dump to stream
  va_list ap_lencheck;
  va_copy(ap_lencheck, arg);
  int len = vsnprintf(NULL, 0, format, ap_lencheck);
  va_end(ap_lencheck);

  char *str = taco_malloc_(file->alloc, len + 1);
  vsnprintf(str, len + 1, format, arg);
  int result = taco_file_write_(file, str, len);
  taco_free_(file->alloc, str);
  return result;
}
