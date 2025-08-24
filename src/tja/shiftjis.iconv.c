// SPDX-License-Identifier: BSD-2-Clause
#include "tja/shiftjis.h"

#include "alloc.h"
#include "errno.h"
#include "iconv.h"
#include "taco.h"
#include <assert.h>
#include <string.h>

#define BUFFER_SIZE 16384 // page size of Apple silicon

bool tja_is_file_utf8_(taco_file *file, taco_allocator *alloc) {
  iconv_t checker = iconv_open("UTF-8", "UTF-8");
  if (checker == (iconv_t)-1) {
    return false;
  }

  char *buffer = taco_malloc_(alloc, 2 * BUFFER_SIZE);
  char *in = buffer;
  char *out = &buffer[BUFFER_SIZE];
  char *read_head = in;
  bool result;

  // the file is validated from start
  taco_file_seek_(file, 0, SEEK_SET);

  while (1) {
    size_t count =
        taco_file_read_(file, read_head, BUFFER_SIZE - (read_head - in));
    if (count == 0) {
      // ensure there is no truncated characters at the end
      result = read_head == in;
      break;
    }

    // add leftovers from last block to byte count
    count += read_head - in;

    char *in_head = in;
    char *out_head = out;
    size_t out_remaining = BUFFER_SIZE;

    size_t error = iconv(checker, &in_head, &count, &out_head, &out_remaining);
    assert((error != (size_t)-1 || errno != E2BIG));

    if (error != (size_t)-1 || /* result == (size_t)-1 && */ errno == EINVAL) {
      // iconv updates count to be the number of remaining bytes
      memmove(in, in_head, count);
      read_head = in + count;
    } else {
      result = false;
      break;
    }
  }

  taco_file_seek_(file, 0, SEEK_SET);
  taco_free_(alloc, in);
  iconv_close(checker);
  return result;
}
