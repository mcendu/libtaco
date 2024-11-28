/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_ALLOC_H_
#define TAIKO_ALLOC_H_

#include "taco.h"

#include <stddef.h>

extern taiko_allocator taiko_default_allocator_;

extern void *taiko_default_malloc_(size_t size, void *null);
extern void taiko_default_free_(void *ptr, void *null);
extern void *taiko_default_realloc_(void *ptr, size_t size, void *null);

#define taiko_malloc_(a, size) ((a)->malloc((size), (a)->heap))
#define taiko_free_(a, ptr) ((a)->free((ptr), (a)->heap))
#define taiko_realloc_(a, ptr, size) ((a)->realloc((ptr), (size), (a)->heap))

extern char *taiko_strdup_(taiko_allocator *a, const char *str);
extern char *taiko_strndup_(taiko_allocator *a, const char *str, size_t maxlen);

#endif /* !TAIKO_ALLOC_H_ */
