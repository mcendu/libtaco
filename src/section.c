// SPDX-License-Identifier: BSD-2-Clause
#include "section.h"

#include "alloc.h"
#include "note.h" /* IWYU pragma: keep; struct size taken */
#include "taiko.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>

struct taiko_section_ {
  taiko_allocator *alloc;
  size_t size;
  size_t capacity;
  int tickrate;
  taiko_event *events;
};

#define INITIAL_CAPACITY (4096 / sizeof(taiko_event))

taiko_section *taiko_section_create_(void) {
  return taiko_section_create2_(&taiko_default_allocator_);
}

taiko_section *taiko_section_create2_(taiko_allocator *a) {
  taiko_section *section = taiko_malloc_(a, sizeof(taiko_section));
  taiko_event *events =
      taiko_malloc_(a, INITIAL_CAPACITY * sizeof(taiko_event));

  if (!section || !events) {
    taiko_free_(a, section);
    taiko_free_(a, events);
    return NULL;
  }

  section->alloc = a;
  section->events = events;
  section->size = 0;
  section->capacity = INITIAL_CAPACITY;
  section->tickrate = 480;
  return section;
}

taiko_section *taiko_section_clone_(const taiko_section *restrict other) {
  taiko_allocator *a = other->alloc;
  taiko_section *section = taiko_malloc_(a, sizeof(taiko_section));
  taiko_event *events = taiko_malloc_(a, other->capacity * sizeof(taiko_event));

  if (!section || !events) {
    taiko_free_(a, section);
    taiko_free_(a, events);
    return NULL;
  }

  memcpy(events, other->events, other->size * sizeof(taiko_event));

  section->alloc = a;
  section->events = events;
  section->size = other->size;
  section->capacity = other->capacity;
  section->tickrate = other->tickrate;
  return section;
}

void taiko_section_free_(taiko_section *section) {
  if (section) {
    taiko_free_(section->alloc, section->events);
    taiko_free_(section->alloc, section);
  }
}

size_t taiko_section_size(const taiko_section *restrict s) { return s->size; }

int taiko_section_tickrate(const taiko_section *restrict s) {
  return s->tickrate;
}

void taiko_section_set_tickrate_(taiko_section *restrict s, int tickrate) {
  s->tickrate = tickrate;
}

const taiko_event *taiko_section_begin(const taiko_section *restrict s) {
  return s->events;
}

const taiko_event *taiko_section_end(const taiko_section *restrict s) {
  return s->events + s->size;
}

const taiko_event *taiko_section_locate(const taiko_section *restrict s,
                                        size_t i) {
  if (i >= s->size)
    return NULL;
  return s->events + i;
}

taiko_event *taiko_section_begin_mut_(taiko_section *restrict s) {
  return s->events;
}

taiko_event *taiko_section_end_mut_(taiko_section *restrict s) {
  return s->events + s->size;
}

taiko_event *taiko_section_locate_mut_(taiko_section *restrict s, size_t i) {
  return s->events + i;
}

static taiko_event *reserve(taiko_section *restrict s, size_t count) {
  size_t oldsize = s->size;
  size_t newsize = s->size + count;

  /* check if capacity is enough */
  if (newsize <= s->capacity) {
    s->size = newsize;
    return s->events + oldsize;
  }

  /* calculate new capacity */
  size_t newcap = s->capacity;
  while (newcap < newsize) {
    if (s->capacity > SIZE_MAX / 2)
      newcap = SIZE_MAX;
    else
      newcap *= 2;
  }

  /* realloc */
  taiko_event *events =
      taiko_realloc_(s->alloc, s->events, newcap * sizeof(taiko_event));
  if (!events)
    return NULL;

  s->events = events;
  s->size = newsize;
  s->capacity = newcap;
  return s->events + oldsize;
}

int taiko_section_trim_(taiko_section *restrict s) {
  size_t size = s->size < INITIAL_CAPACITY ? INITIAL_CAPACITY : s->size;
  taiko_event *events =
      taiko_realloc_(s->alloc, s->events, size * sizeof(taiko_event));
  if (!events)
    return -1;

  s->events = events;
  s->capacity = size;
  return 0;
}

int taiko_section_push_(taiko_section *restrict s,
                        const taiko_event *restrict event) {
  taiko_event *start = reserve(s, 1);
  if (!start)
    return -1;

  memcpy(start, event, sizeof(*event));
  return 0;
}

extern int taiko_section_push_many_(taiko_section *restrict s,
                                    const taiko_event *restrict events,
                                    size_t count) {
  taiko_event *start = reserve(s, count);
  if (!start)
    return -1;

  memcpy(start, events, count * sizeof(taiko_event));
  return 0;
}

int taiko_section_concat_(taiko_section *restrict s,
                          const taiko_section *restrict other) {
  return taiko_section_push_many_(s, other->events, other->size);
}

int taiko_section_delete_(taiko_section *s, taiko_event *i) {
  if (i < taiko_section_begin(s) || i >= taiko_section_end(s))
    return -1;

  size_t len = taiko_section_end(s) - (i + 1);
  memmove(i, i + 1, len * sizeof(taiko_event));
  s->size -= 1;
  return 0;
}

void taiko_section_clear_(taiko_section *restrict s) { s->size = 0; }

int taiko_section_set_balloons_(taiko_section *restrict section,
                                const int balloons[restrict], size_t count) {
  const int *j = balloons;
  const int *end = balloons + count;

  taiko_section_foreach_mut_ (i, section) {
    if (i->type != TAIKO_EVENT_BALLOON && i->type != TAIKO_EVENT_KUSUDAMA)
      continue;

    if (j != end) {
      i->detail_int.value = *j;
      j += 1;
    } else {
      i->detail_int.value = 1;
    }
  }

  return j - balloons;
}
