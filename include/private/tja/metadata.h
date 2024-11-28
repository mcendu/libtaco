/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_METADATA_H_
#define TJA_METADATA_H_

#include "taco.h"

enum tja_metadata_enum_ {
  TJA_METADATA_UNRECOGNIZED,
  TJA_METADATA_TITLE,
  TJA_METADATA_SUBTITLE,
  TJA_METADATA_GENRE,
  TJA_METADATA_MAKER,
  TJA_METADATA_AUDIO,
  TJA_METADATA_BPM,
  TJA_METADATA_OFFSET,
  TJA_METADATA_DEMOSTART,
  TJA_METADATA_SIDE,
  TJA_METADATA_COURSE,
  TJA_METADATA_LEVEL,
  TJA_METADATA_STYLE,
  TJA_METADATA_BALLOON,
  TJA_METADATA_BALLOONEXP,
  TJA_METADATA_BALLOONMAS,
  TJA_METADATA_PAPAMAMA,
  TJA_METADATA_MAX,
};

typedef struct tja_metadata_ tja_metadata;
typedef struct tja_metadata_field_ tja_metadata_field;
typedef struct tja_balloon_ tja_balloon;

extern tja_metadata *tja_metadata_create2_(taiko_allocator *alloc);
extern void tja_metadata_free_(tja_metadata *meta);
extern int tja_metadata_add_field_(tja_metadata *meta,
                                   tja_metadata_field *field);
extern int tja_metadata_update_(tja_metadata *old, tja_metadata *updates);
extern int tja_courseset_apply_metadata_(taiko_courseset *set,
                                         tja_metadata *meta);
extern int tja_course_apply_metadata_(taiko_course *course, tja_metadata *meta);

extern int tja_interpret_side_(const char *str);
extern int tja_interpret_course_(const char *str);
extern int tja_interpret_style_(const char *str);

extern tja_balloon *tja_balloon_create2_(taiko_allocator *alloc);
extern tja_balloon *tja_balloon_append_(tja_balloon *balloon, int hits);
extern const int *tja_balloon_data_(tja_balloon *balloon);
extern size_t tja_balloon_size_(tja_balloon *balloon);
extern void tja_balloon_free_(tja_balloon *balloon);

struct tja_metadata_field_ {
  int key;
  int reserved;
  union {
    int integer;
    double real;
    char *text;
    tja_balloon *balloon;
  };
};

#endif /* !TJA_METADATA_H_ */
