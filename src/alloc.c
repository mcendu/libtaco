// SPDX-License-Identifier: BSD-2-Clause
#include "alloc.h"

#include "taco.h"
#include <stdlib.h>
#include <string.h>

taco_allocator taco_default_allocator_ = {
    .malloc = taco_default_malloc_,
    .free = taco_default_free_,
    .realloc = taco_default_realloc_,
    .heap = NULL,
};

void *taco_default_malloc_(size_t size, void *heap) { return malloc(size); }

void taco_default_free_(void *ptr, void *heap) { return free(ptr); }

void *taco_default_realloc_(void *ptr, size_t size, void *heap) {
  return realloc(ptr, size);
}

char *taco_strdup_(taco_allocator *a, const char *src) {
  size_t len = strlen(src) + 1;
  char *dst = taco_malloc_(a, len);
  if (!dst)
    return NULL;
  memcpy(dst, src, len);
  return dst;
}

char *taco_strndup_(taco_allocator *a, const char *src, size_t maxlen) {
  char *nul = memchr(src, 0, maxlen);
  size_t len = nul ? nul - src : maxlen;

  char *dst = taco_malloc_(a, len + 1);
  if (!dst)
    return NULL;
  memcpy(dst, src, len);
  dst[len] = '\0';
  return dst;
}
