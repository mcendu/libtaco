// SPDX-License-Identifier: BSD-2-Clause
#include "io.h"

#include "alloc.h"
#include "taco.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

struct taiko_file_ {
  taiko_allocator *alloc;
  void *stream;
  char *filename;
  taiko_read_fn *read;
  taiko_write_fn *write;
  taiko_close_fn *close;
  taiko_printf_fn *printf;
};

taiko_file *taiko_file_open_(taiko_allocator *alloc, void *stream,
                             const char *filename, taiko_read_fn *read,
                             taiko_write_fn *write, taiko_close_fn *close,
                             taiko_printf_fn *printf) {
  taiko_file *f = taiko_malloc_(alloc, sizeof(taiko_file));
  if (!f)
    return NULL;

  f->alloc = alloc;
  f->stream = stream;
  f->filename = taiko_strdup_(alloc, filename);
  f->read = read;
  f->write = write;
  f->close = close;
  f->printf = printf;
  return f;
}

taiko_file *taiko_file_open_path_(const char *path, const char *mode) {
  FILE *f = fopen(path, mode);
  if (!f)
    return NULL;

  taiko_file *result =
      taiko_file_open_(&taiko_default_allocator_, f, path,
                       (taiko_read_fn *)fread, (taiko_write_fn *)fwrite,
                       (taiko_close_fn *)fclose, (taiko_printf_fn *)vfprintf);
  if (!result) {
    fclose(f);
    return NULL;
  }

  return result;
}

taiko_file *taiko_file_open_stdio_(FILE *file) {
  return taiko_file_open_(&taiko_default_allocator_, file, "<stream>",
                          (taiko_read_fn *)fread, (taiko_write_fn *)fwrite,
                          NULL, (taiko_printf_fn *)vfprintf);
}

taiko_file *taiko_file_open_null_(taiko_allocator *alloc) {
  if (!alloc)
    alloc = &taiko_default_allocator_;

  return taiko_file_open_(alloc, NULL, "<null>", NULL, NULL, NULL, NULL);
}

void taiko_file_close_(taiko_file *file) {
  if (file->close)
    file->close(file->stream);

  taiko_free_(file->alloc, file->filename);
  taiko_free_(file->alloc, file);
}

const char *taiko_file_name_(const taiko_file *file) { return file->filename; }

void taiko_file_set_name_(taiko_file *file, const char *filename) {
  taiko_free_(file->alloc, file->filename);
  file->filename = taiko_strdup_(file->alloc, filename);
}

size_t taiko_file_read_(taiko_file *file, void *dst, size_t size) {
  if (file->read)
    return file->read(dst, 1, size, file->stream);
  return 0;
}

size_t taiko_file_write_(taiko_file *file, const void *src, size_t size) {
  if (file->write)
    return file->write(src, 1, size, file->stream);
  return 0;
}

int taiko_file_printf_(taiko_file *file, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = taiko_file_vprintf_(file, format, ap);
  va_end(ap);

  return result;
}

int taiko_file_vprintf_(taiko_file *file, const char *format, va_list arg) {
  if (file->printf)
    return file->printf(file->stream, format, arg);

  // without a dedicated printf, format in memory and dump to stream
  va_list ap_lencheck;
  va_copy(ap_lencheck, arg);
  int len = vsnprintf(NULL, 0, format, ap_lencheck);
  va_end(ap_lencheck);

  char *str = taiko_malloc_(file->alloc, len + 1);
  vsnprintf(str, len + 1, format, arg);
  int result = taiko_file_write_(file, str, len);
  taiko_free_(file->alloc, str);
  return result;
}
