// SPDX-License-Identifier: BSD-2-Clause
#include "tja/shiftjis.h"

extern "C" {
#include "io.h"
}

#include <array>
#include <cstdio>
#include <cstring>
#include <memory>
#include <simdutf.h>

using simdutf::trim_partial_utf8;
using simdutf::validate_utf8;
using std::array;
using std::make_unique;

extern "C" bool tja_is_file_utf8_(taco_file *file) {
  const size_t BUFFER_SIZE = 16384; // page size of Apple silicon
  auto buffer = make_unique<array<char, BUFFER_SIZE>>();
  char *read_head = buffer->data();
  bool result;

  // the file is validated from start
  taco_file_seek_(file, 0, SEEK_SET);

  while (1) {
    size_t count = taco_file_read_(file, read_head,
                                   BUFFER_SIZE - (read_head - buffer->data()));
    if (count == 0) {
      // ensure there is no truncated characters at the end
      result = read_head == buffer->data();
      break;
    }

    // add leftovers from last block to byte count
    count += read_head - buffer->data();
    // the end might contain truncated characters
    size_t trimmed = trim_partial_utf8(buffer->data(), count);

    if (!validate_utf8(buffer->data(), trimmed)) {
      result = false;
      break;
    }

    // move partial multibyte sequence to start
    memmove(buffer->data(), buffer->data() + trimmed, count - trimmed);
    read_head = buffer->data() + (count - trimmed);
  }

  taco_file_seek_(file, 0, SEEK_SET);
  return result;
}
