/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_ALLOC_H_
#define TACO_ALLOC_H_

#include "taco.h"

#include <stddef.h>

extern taco_allocator taco_default_allocator_;

extern void *taco_default_malloc_(size_t size, void *null);
extern void taco_default_free_(void *ptr, void *null);
extern void *taco_default_realloc_(void *ptr, size_t size, void *null);

#define taco_malloc_(a, size) ((a)->malloc((size), (a)->heap))
#define taco_free_(a, ptr) ((a)->free((ptr), (a)->heap))
#define taco_realloc_(a, ptr, size) ((a)->realloc((ptr), (size), (a)->heap))

extern char *taco_strdup_(taco_allocator *a, const char *str);
extern char *taco_strndup_(taco_allocator *a, const char *str, size_t maxlen);

#endif /* !TACO_ALLOC_H_ */
