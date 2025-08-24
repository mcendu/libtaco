// SPDX-License-Identifier: BSD-2-Clause
#include "tja/shiftjis.h"

#include "alloc.h"
#include "io.h"
#include "taco.h"
#include <errno.h>
#include <iconv.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 16384 // page size of Apple silicon

/* Presents a non UTF-8 encoded file as UTF-8. */
typedef struct tja_iconv_ tja_iconv;

static int tja_iconv_destroy_(tja_iconv *self);
static size_t tja_iconv_read_(char *restrict buf, size_t reserved, size_t count,
                              tja_iconv *self);

struct tja_iconv_ {
  taco_allocator *alloc;
  taco_file *file;
  iconv_t iconv;
  char *buffer;
  char *tail;
  char *head;
  bool eof;
};

static const taco_io tja_iconv_funcs_ = {
    .version = sizeof(taco_io),
    .close = (taco_close_fn *)tja_iconv_destroy_,
    .read = (taco_read_fn *)tja_iconv_read_,
};

static tja_iconv *tja_iconv_create_(taco_allocator *alloc, taco_file *file,
                                    const char *src_charset) {
  if (!src_charset) {
    src_charset = "Shift_JIS";
  }

  // initialize filter
  tja_iconv *obj = taco_malloc_(alloc, sizeof(tja_iconv));
  iconv_t iconv = iconv_open("UTF-8", src_charset);
  char *buffer = taco_malloc_(alloc, BUFFER_SIZE);

  if (!obj || !buffer || iconv == (iconv_t)-1) {
    taco_free_(alloc, buffer);
    taco_free_(alloc, obj);
    // iconv_close does not promise sanity on null pointers
    if (iconv != (iconv_t)-1)
      iconv_close(iconv);
    return NULL;
  }

  obj->alloc = alloc;
  obj->file = file;
  obj->iconv = iconv;
  obj->buffer = buffer;
  obj->head = buffer;
  obj->tail = buffer;
  obj->eof = false;

  return obj;
}

static int tja_iconv_destroy_(tja_iconv *self) {
  iconv_close(self->iconv);
  taco_free_(self->alloc, self->buffer);
  taco_free_(self->alloc, self);

  // make the occasional user of fclose return value happy here
  return 0;
}

taco_file *tja_iconv_open_(taco_allocator *alloc, taco_file *file,
                           const char *src_charset) {
  tja_iconv *obj = tja_iconv_create_(alloc, file, src_charset);
  if (!obj) {
    return NULL;
  }

  taco_file *wrapper =
      taco_file_open_(alloc, obj, taco_file_name_(file), &tja_iconv_funcs_);
  if (!wrapper) {
    tja_iconv_destroy_(obj);
    return NULL;
  }

  return wrapper;
}

static void buffer_input(tja_iconv *self) {
  // shortcut if we are at end of file
  if (self->eof)
    return;

  // salvage leftovers (incomplete multibyte sequences)
  size_t leftover_size = self->tail - self->head;
  memmove(self->buffer, self->head, leftover_size);
  char *read_start = self->buffer + leftover_size;

  // buffer data
  self->head = self->buffer;
  self->tail = self->buffer + taco_file_read_(self->file, read_start,
                                              BUFFER_SIZE - leftover_size);
  self->eof = self->tail == self->buffer;
}

// Consume the first byte of an invalid multibyte sequence and put a U+FFFD
// REPLACEMENT CHARACTER in the output stream.
static int place_fffd(char **restrict inbuf, size_t *restrict inbytesleft,
                      char **restrict outbuf, size_t *restrict outbytesleft) {
  // UTF-8 encodes U+FFFD as the byte sequence 0xEF 0xBF 0xBD
  if (*outbytesleft < 3) {
    return -1;
  }

  memcpy(*outbuf, (char[]){0xef, 0xbf, 0xbd}, 3);

  *outbuf += 3;
  *outbytesleft -= 3;
  *inbuf += 1;
  *inbytesleft -= 1;
  return 0;
}

static size_t tja_iconv_read_(char *restrict buf, size_t reserved, size_t count,
                              tja_iconv *self) {
  // refill buffer if we ran out
  if (self->head >= self->tail) {
    buffer_input(self);
    if (self->eof)
      return 0;
  }

  size_t inbytesleft = self->tail - self->head;
  char *head = buf;
  size_t outbytesleft = count;

  // convert data until the backing file EOFs or the output buffer is filled
  while (1) {
    size_t result =
        iconv(self->iconv, &self->head, &inbytesleft, &head, &outbytesleft);
    if (result != (size_t)-1 || /* result == (size_t)-1 && */ errno == EINVAL) {
      // input buffer ran out of data, refilling
      buffer_input(self);
      if (self->eof)
        // backing file also ran out of data, bail out
        return head - (char *)buf;

      // inbytesleft needs to be updated to reflect the newly buffered data
      inbytesleft = self->tail - self->head;
    } else if (errno == EILSEQ) {
      // invalid byte sequence, output U+FFFD then continue
      if (place_fffd(&self->head, &inbytesleft, &head, &outbytesleft) != 0) {
        // end of output buffer = return here
        return head - (char *)buf;
      }
    } else if (errno == E2BIG) {
      // end of output buffer
      return head - (char *)buf;
    }
  }
}
