// SPDX-License-Identifier: BSD-2-Clause
#include "alloc.h"

#include "taco.h"
#include <stdlib.h>
#include <string.h>

taiko_allocator taiko_default_allocator_ = {
    .malloc = taiko_default_malloc_,
    .free = taiko_default_free_,
    .realloc = taiko_default_realloc_,
    .heap = NULL,
};

void *taiko_default_malloc_(size_t size, void *heap) { return malloc(size); }

void taiko_default_free_(void *ptr, void *heap) { return free(ptr); }

void *taiko_default_realloc_(void *ptr, size_t size, void *heap) {
  return realloc(ptr, size);
}

char *taiko_strdup_(taiko_allocator *a, const char *src) {
  size_t len = strlen(src) + 1;
  char *dst = taiko_malloc_(a, len);
  if (!dst)
    return NULL;
  memcpy(dst, src, len);
  return dst;
}

char *taiko_strndup_(taiko_allocator *a, const char *src, size_t maxlen) {
  char *nul = memchr(src, 0, maxlen);
  size_t len = nul ? nul - src : maxlen;

  char *dst = taiko_malloc_(a, len + 1);
  if (!dst)
    return NULL;
  memcpy(dst, src, len);
  dst[len] = '\0';
  return dst;
}
