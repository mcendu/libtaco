// SPDX-License-Identifier: BSD-2-Clause
#include "tja/metadata.h"

#include "alloc.h"
#include "taco.h"
#include <string.h>

#define COUNTED_BY(count) TAIKO_ATTRIBUTE_GNU_(counted_by(count))

struct tja_balloon_ {
  taiko_allocator *alloc;
  size_t size;
  size_t capacity;
  int data[];
};

#define INITIAL_CAPACITY 8

tja_balloon *tja_balloon_create2_(taiko_allocator *a) {
  tja_balloon *b =
      taiko_malloc_(a, sizeof(tja_balloon) + INITIAL_CAPACITY * sizeof(int));
  b->alloc = a;
  b->size = 0;
  b->capacity = INITIAL_CAPACITY;
  memset(b->data, 0, INITIAL_CAPACITY * sizeof(int));
  return b;
}

tja_balloon *tja_balloon_append_(tja_balloon *b, int hits) {
  if (b->size == b->capacity) {
    size_t capacity = b->capacity * 2;
    tja_balloon *new = taiko_realloc_(
        b->alloc, b, sizeof(tja_balloon) + capacity * sizeof(int));
    if (!new)
      return NULL;

    b = new;
    b->capacity = capacity;
  }

  b->data[b->size] = hits;
  b->size += 1;
  return b;
}

const int *tja_balloon_data_(tja_balloon *balloon) { return balloon->data; }

size_t tja_balloon_size_(tja_balloon *balloon) { return balloon->size; }

void tja_balloon_free_(tja_balloon *balloon) {
  taiko_free_(balloon->alloc, balloon);
}
