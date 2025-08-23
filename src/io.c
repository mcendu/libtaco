// SPDX-License-Identifier: BSD-2-Clause
#include "config.h"

#ifdef TACO_HAS_FSEEKO_
#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64

#define _POSIX_C_SOURCE 200112L
#endif

#include "io.h"

#include "alloc.h"
#include "taco.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#ifdef TACO_HAS_FSEEKO_
#define stdio_seek ((taco_seek_fn *)fseeko)
#else
static int stdio_seek(void *file, uint64_t offset, int whence) {
  return fseek(file, (long)offset, whence);
}
#endif

struct taco_file_ {
  taco_allocator *alloc;
  void *stream;
  char *filename;
  const taco_io *callbacks;
};

static const taco_io stdio_owned_callbacks_ = {
    .version = sizeof(taco_io),
    .read = (taco_read_fn *)fread,
    .write = (taco_write_fn *)fwrite,
    .seek = stdio_seek,
    .printf = (taco_printf_fn *)vfprintf,
    .close = (taco_close_fn *)fclose,
};

static const taco_io stdio_borrowed_callbacks_ = {
    .version = sizeof(taco_io),
    .read = (taco_read_fn *)fread,
    .write = (taco_write_fn *)fwrite,
    .seek = stdio_seek,
    .printf = (taco_printf_fn *)vfprintf,
};

static const taco_io null_callbacks_ = {
    .version = sizeof(taco_io),
};

taco_file *taco_file_open_(taco_allocator *alloc, void *stream,
                           const char *filename, const taco_io *callbacks) {
  taco_file *f = taco_malloc_(alloc, sizeof(taco_file));
  if (!f)
    return NULL;

  f->alloc = alloc;
  f->stream = stream;
  f->filename = taco_strdup_(alloc, filename);
  f->callbacks = callbacks;
  return f;
}

taco_file *taco_file_open_path_(const char *path, const char *mode) {
  FILE *f = fopen(path, mode);
  if (!f)
    return NULL;

  taco_file *result = taco_file_open_(&taco_default_allocator_, f, path,
                                      &stdio_owned_callbacks_);
  if (!result) {
    fclose(f);
    return NULL;
  }

  return result;
}

taco_file *taco_file_open_stdio_(FILE *file) {
  return taco_file_open_(&taco_default_allocator_, file, "<stream>",
                         &stdio_borrowed_callbacks_);
}

taco_file *taco_file_open_null_(taco_allocator *alloc) {
  if (!alloc)
    alloc = &taco_default_allocator_;

  return taco_file_open_(alloc, NULL, "<null>", &null_callbacks_);
}

void taco_file_close_(taco_file *file) {
  if (!file)
    return;

  if (file->callbacks->version > offsetof(taco_io, close) &&
      file->callbacks->close)
    file->callbacks->close(file->stream);

  taco_free_(file->alloc, file->filename);
  taco_free_(file->alloc, file);
}

const char *taco_file_name_(const taco_file *file) { return file->filename; }

void taco_file_set_name_(taco_file *file, const char *filename) {
  taco_free_(file->alloc, file->filename);
  file->filename = taco_strdup_(file->alloc, filename);
}

size_t taco_file_read_(taco_file *file, void *dst, size_t size) {
  if (file->callbacks->version > offsetof(taco_io, read) &&
      file->callbacks->read)
    return file->callbacks->read(dst, 1, size, file->stream);
  return 0;
}

size_t taco_file_write_(taco_file *file, const void *src, size_t size) {
  if (file->callbacks->version > offsetof(taco_io, write) &&
      file->callbacks->write)
    return file->callbacks->write(src, 1, size, file->stream);
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
  if (file->callbacks->version > offsetof(taco_io, printf) &&
      file->callbacks->printf)
    return file->callbacks->printf(file->stream, format, arg);

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

int taco_file_seek_(taco_file *file, uint64_t offset, int whence) {
  if (file->callbacks->version > offsetof(taco_io, seek) &&
      file->callbacks->seek)
    return file->callbacks->seek(file->stream, offset, whence);
  return -1;
}
