// SPDX-License-Identifier: BSD-2-Clause
#include "tja/shiftjis.h"

extern "C" {
#include "alloc.h"
#include "io.h"
#include "taco.h"
}

#include <cstdio>
#include <cstring>
#include <simdutf.h>

using simdutf::trim_partial_utf8;
using simdutf::validate_utf8;

extern "C" bool tja_is_file_utf8_(taco_file *file, taco_allocator *alloc) {
  const size_t BUFFER_SIZE = 16384; // page size of Apple silicon
  char *buffer = (char *)taco_malloc_(alloc, BUFFER_SIZE);
  char *read_head = buffer;
  bool result;

  // the file is validated from start
  taco_file_seek_(file, 0, SEEK_SET);

  while (1) {
    size_t count =
        taco_file_read_(file, read_head, BUFFER_SIZE - (read_head - buffer));
    if (count == 0) {
      // ensure there is no truncated characters at the end
      result = read_head == buffer;
      break;
    }

    // add leftovers from last block to byte count
    count += read_head - buffer;
    // the end might contain truncated characters
    size_t trimmed = trim_partial_utf8(buffer, count);

    if (!validate_utf8(buffer, trimmed)) {
      result = false;
      break;
    }

    // move partial multibyte sequence to start
    memmove(buffer, buffer + trimmed, count - trimmed);
    read_head = buffer + (count - trimmed);
  }

  taco_file_seek_(file, 0, SEEK_SET);
  taco_free_(alloc, (void *)buffer);
  return result;
}
