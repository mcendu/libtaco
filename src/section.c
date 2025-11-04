// SPDX-License-Identifier: BSD-2-Clause
#include "section.h"

#include "alloc.h"
#include "note.h" /* IWYU pragma: keep; struct size taken */
#include "taco.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define INITIAL_CAPACITY (4096 / sizeof(taco_event))

typedef struct bpm_entry_ bpm_entry;

struct bpm_entry_ {
  int ticks;
  float bpm;
  double time;
};

struct taco_section_ {
  taco_allocator *alloc;
  size_t size;
  size_t capacity;
  int tickrate;
  taco_event *events;

  bpm_entry *bpm_times;
  size_t time_events;
};

static inline void invalidate_bpm_times_(taco_section *restrict s);

taco_section *taco_section_create_(void) {
  return taco_section_create2_(&taco_default_allocator_);
}

taco_section *taco_section_create2_(taco_allocator *a) {
  taco_section *section = taco_malloc_(a, sizeof(taco_section));
  taco_event *events = taco_malloc_(a, INITIAL_CAPACITY * sizeof(taco_event));

  if (!section || !events) {
    taco_free_(a, section);
    taco_free_(a, events);
    return NULL;
  }

  memset(section, 0, sizeof(taco_section));
  section->alloc = a;
  section->events = events;
  section->size = 0;
  section->capacity = INITIAL_CAPACITY;
  section->tickrate = 96;
  section->bpm_times = NULL;
  section->time_events = 0;
  return section;
}

taco_section *taco_section_clone_(const taco_section *restrict other) {
  taco_allocator *a = other->alloc;
  taco_section *section = taco_malloc_(a, sizeof(taco_section));
  taco_event *events = taco_malloc_(a, other->capacity * sizeof(taco_event));

  if (!section || !events) {
    taco_free_(a, section);
    taco_free_(a, events);
    return NULL;
  }

  memcpy(events, other->events, other->size * sizeof(taco_event));

  memset(section, 0, sizeof(taco_section));
  section->alloc = a;
  section->events = events;
  section->size = other->size;
  section->capacity = other->capacity;
  section->tickrate = other->tickrate;
  section->bpm_times = NULL;
  section->time_events = 0;
  return section;
}

void taco_section_free_(taco_section *section) {
  if (section) {
    taco_free_(section->alloc, section->events);
    taco_free_(section->alloc, section->bpm_times);
    taco_free_(section->alloc, section);
  }
}

size_t taco_section_size(const taco_section *restrict s) { return s->size; }

int taco_section_tickrate(const taco_section *restrict s) {
  return s->tickrate;
}

void taco_section_set_tickrate_(taco_section *restrict s, int tickrate) {
  s->tickrate = tickrate;
}

const taco_event *taco_section_begin(const taco_section *restrict s) {
  return s->events;
}

const taco_event *taco_section_end(const taco_section *restrict s) {
  return s->events + s->size;
}

const taco_event *taco_section_locate(const taco_section *restrict s,
                                      size_t i) {
  if (i >= s->size)
    return NULL;
  return s->events + i;
}

taco_event *taco_section_begin_mut_(taco_section *restrict s) {
  invalidate_bpm_times_(s);
  return s->events;
}

taco_event *taco_section_end_mut_(taco_section *restrict s) {
  invalidate_bpm_times_(s);
  return s->events + s->size;
}

taco_event *taco_section_locate_mut_(taco_section *restrict s, size_t i) {
  invalidate_bpm_times_(s);
  return s->events + i;
}

static taco_event *reserve(taco_section *restrict s, size_t count) {
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
  taco_event *events =
      taco_realloc_(s->alloc, s->events, newcap * sizeof(taco_event));
  if (!events)
    return NULL;

  s->events = events;
  s->size = newsize;
  s->capacity = newcap;
  return s->events + oldsize;
}

int taco_section_trim_(taco_section *restrict s) {
  size_t size = s->size < INITIAL_CAPACITY ? INITIAL_CAPACITY : s->size;
  taco_event *events =
      taco_realloc_(s->alloc, s->events, size * sizeof(taco_event));
  if (!events)
    return -1;

  s->events = events;
  s->capacity = size;
  return 0;
}

int taco_section_push_(taco_section *restrict s,
                       const taco_event *restrict event) {
  invalidate_bpm_times_(s);
  taco_event *start = reserve(s, 1);
  if (!start)
    return -1;

  memcpy(start, event, sizeof(*event));
  return 0;
}

extern int taco_section_push_many_(taco_section *restrict s,
                                   const taco_event *restrict events,
                                   size_t count) {
  invalidate_bpm_times_(s);
  taco_event *start = reserve(s, count);
  if (!start)
    return -1;

  memcpy(start, events, count * sizeof(taco_event));
  return 0;
}

int taco_section_concat_(taco_section *restrict s,
                         const taco_section *restrict other) {
  invalidate_bpm_times_(s);
  return taco_section_push_many_(s, other->events, other->size);
}

int taco_section_pop_(taco_section *s, size_t count) {
  invalidate_bpm_times_(s);
  if (count > s->size)
    count = s->size;
  s->size -= count;
  return 0;
}

void taco_section_clear_(taco_section *restrict s) {
  invalidate_bpm_times_(s);
  s->size = 0;
}

int taco_section_set_balloons_(taco_section *restrict section,
                               const int *restrict balloons, size_t count) {
  const int *j = balloons;
  const int *end = balloons + count;

  taco_section_foreach_mut_(i, section) {
    if (i->type != TACO_EVENT_BALLOON && i->type != TACO_EVENT_KUSUDAMA)
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

#define TIME(bpm, ticks, tickrate)                                             \
  ((60.0 / (double)(bpm) * 4.0) * ((double)(ticks) / (double)(tickrate)))

int taco_section_cache_seconds_(taco_section *restrict s) {
  bpm_entry *old = s->bpm_times;

  // scan
  int time_events = 0;
  taco_section_foreach(i, s) {
    if (taco_event_type(i) == TACO_EVENT_BPM)
      time_events += 1;
    if (taco_event_type(i) == TACO_EVENT_DELAY)
      time_events += 1;
  }

  if (time_events == 0) {
    s->bpm_times = NULL;
    return 0;
  }

  bpm_entry *bpm_times =
      taco_malloc_(s->alloc, time_events * sizeof(bpm_entry));
  if (!bpm_times) {
    s->bpm_times = NULL;
    return -1;
  }

  int ticks = 0;
  double time = 0;
  bpm_entry *entry = bpm_times;
  double bpm = 120; // sane default for initial bpm

  // calculate time for events affecting timing
  taco_section_foreach(i, s) {
    if (i->type == TACO_EVENT_BPM || i->type == TACO_EVENT_DELAY) {
      int delta = i->time - ticks;
      time += TIME(bpm, delta, s->tickrate);
      ticks = i->time;

      if (i->type == TACO_EVENT_BPM)
        bpm = i->detail_float.value;
      if (i->type == TACO_EVENT_DELAY)
        time += i->detail_float.value;

      if (entry > bpm_times && delta == 0) {
        entry -= 1;
        time_events -= 1;
      }

      entry->ticks = ticks;
      entry->time = time;
      entry->bpm = bpm;
      entry += 1;
    }
  }

  s->time_events = time_events;
  taco_free_(s->alloc, old);
  s->bpm_times = bpm_times;

  return 0;
}

static inline void invalidate_bpm_times_(taco_section *restrict s) {
  taco_free_(s->alloc, s->bpm_times);
  s->bpm_times = NULL;
}

static const bpm_entry *find_bpm_section_start_(int ticks,
                                                const bpm_entry *start,
                                                const bpm_entry *end) {
  if (end - start <= 1)
    return start;

  const bpm_entry *mid = &start[(end - start) / 2];
  if (ticks < mid->ticks)
    return find_bpm_section_start_(ticks, start, mid);
  else
    return find_bpm_section_start_(ticks, mid, end);
}

TACO_PUBLIC double taco_event_seconds(const taco_event *restrict e,
                                      const taco_section *restrict s) {
  if (e < s->events || e >= s->events + s->size)
    return NAN;

  if (!s->bpm_times) {
    return NAN;
  }

  const bpm_entry *entry = find_bpm_section_start_(
      taco_event_time(e), s->bpm_times, s->bpm_times + s->time_events);
  double start = entry->time;
  int delta = taco_event_time(e) - entry->ticks;
  return start + TIME(entry->bpm, delta, s->tickrate);
}
