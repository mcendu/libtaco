// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include <assert.h>
#include <math.h>

#define TYPE_EMPTY 0         // empty group
#define TYPE_EVEN 1          // group is even number of dons
#define TYPE_ODD 2           // group is odd number of dons
#define TYPE_EVEN_THEN_KAT 3 // group is even number of dons + kat
#define TYPE_ODD_THEN_KAT 4  // group is odd number of dons + kat
#define TYPE_KAT_MIXED 5     // anything else

#define EPSILON (1.0 / 1024.0)       // strict epsilon
#define EPSILON_RELAXED (1.0 / 64.0) // relaxed epsilon for hs/bpm wiggle room

#define NONGROUPING_THRESHOLD (1.0 / 6.0 - EPSILON_RELAXED)

typedef struct group_ group;
typedef struct annotator_state_ annotator_state;

struct group_ {
  size_t start;   // start of current annotation group
  size_t prev;    // second-to-last note in annotation group
  size_t last;    // last note in annotation group
  int type;       // type of group (determines if do-ko-don is used)
                  // instead of do-do-don
  int time;       // tick timestamp of last note in group
  double spacing; // visual spacing (1.0 = 4/4 whole note at HS1)
};

struct annotator_state_ {
  group group;     // current group pending annotation
  double bpm;      // bpm at last note
  double hi_speed; // hi-speed
  double next_bpm; // latest bpm
};

static const int group_type_transitions[][3] = {
    [TYPE_EMPTY] = {TYPE_ODD, TYPE_KAT_MIXED, TYPE_EMPTY},
    [TYPE_EVEN] = {TYPE_ODD, TYPE_EVEN_THEN_KAT, TYPE_ODD},
    [TYPE_ODD] = {TYPE_EVEN, TYPE_ODD_THEN_KAT, TYPE_EVEN},
    [TYPE_EVEN_THEN_KAT] = {TYPE_KAT_MIXED, TYPE_KAT_MIXED, TYPE_EVEN},
    [TYPE_ODD_THEN_KAT] = {TYPE_KAT_MIXED, TYPE_KAT_MIXED, TYPE_ODD},
    [TYPE_KAT_MIXED] = {TYPE_KAT_MIXED, TYPE_KAT_MIXED, TYPE_KAT_MIXED},
};

#define TRANSITION_BACKSTEP 2

static const uint8_t annotate_from_flags[] = {
    TACO_TEXT_DO, TACO_TEXT_KO, TACO_TEXT_DON, TACO_TEXT_DON,
    TACO_TEXT_KA, TACO_TEXT_KA, TACO_TEXT_KAT, TACO_TEXT_KAT,
};

static void start_group_(group *g) { g->type = TYPE_EMPTY; }

static void add_note_(group *g, const taco_section *branch, size_t i,
                      double spacing) {
  const taco_event *e = taco_section_locate(branch, i);
  assert((e->type == TACO_EVENT_DON || e->type == TACO_EVENT_KAT));

  if (g->type == TYPE_EMPTY) {
    g->start = i;
    g->prev = i;
    g->last = i;
  } else {
    g->prev = g->last;
    g->last = i;
  }

  // update group type using a state machine table
  g->type = group_type_transitions[g->type][e->type == TACO_EVENT_KAT];

  g->time = e->time;
  g->spacing = spacing;
}

static void annotate_group_(taco_section *branch, const group *g,
                            double spacing_after) {
  taco_event *i = taco_section_locate_mut_(branch, g->start);
  int parity = 0;

  if (g->type == TYPE_EMPTY)
    return;

  while (1) {
    int type = taco_event_type(i);
    // is this the last note in group?
    bool final = i >= taco_section_locate(branch, g->last);

    if (type == TACO_EVENT_DON || type == TACO_EVENT_KAT) {
      parity += 1;
      // should this note be a "ko" instead of a "do"?
      bool even = g->type == TYPE_ODD && parity % 2 == 0;
      // is there enough space and logical sense for a long text?
      bool long_final = final &&
                        // we need enough space before, and...
                        g->spacing >= (1.0 / 32.0) &&
                        // enough space after
                        spacing_after > (1.0 / 8.0) - EPSILON;
      // is this a don or kat?
      bool kat = type == TACO_EVENT_KAT;

      int flags = (even) | (long_final << 1) | (kat << 2);
      i->detail_int.value = annotate_from_flags[flags];
    }

    if (final)
      break;
    i = taco_event_next_mut_(i);
  }
}

static void process_event_(taco_section *branch, size_t i,
                           annotator_state *state) {
  group *g = &state->group;
  taco_event *e = taco_section_locate_mut_(branch, i);

  // calculate spacing
  double time_delta =
      (e->time - g->time) / (double)taco_section_tickrate(branch);
  double effective_hs = state->hi_speed * (state->next_bpm / state->bpm);
  double spacing = time_delta * effective_hs;

  if (e->type < 0) {
    // non-interactive event; check for hi-speed and bpm updates
    if (e->type == TACO_EVENT_BPM)
      state->next_bpm = e->detail_float.value;
    else if (e->type == TACO_EVENT_SCROLL)
      state->hi_speed = e->detail_float.value;
    return;
  } else if (e->type != TACO_EVENT_DON && e->type != TACO_EVENT_KAT) {
    // non-annotatable notes.
    // the current group is annotated; new group is empty.
    annotate_group_(branch, g, spacing);
    start_group_(g);
    g->time = e->time;
    g->spacing = spacing;

    state->bpm = state->next_bpm;
    return;
  }

  if (g->type == TYPE_EMPTY) {
    // group is empty, add the note
    add_note_(g, branch, i, spacing);
  } else {
    if (spacing > NONGROUPING_THRESHOLD) {
      // the new spacing is very large, don't use the short form
      annotate_group_(branch, g, spacing);
      start_group_(g);
      add_note_(g, branch, i, spacing);
    } else if (g->start == g->last) {
      // group has only one note, adding
      add_note_(g, branch, i, spacing);
    } else {
      double spacing_delta = spacing - g->spacing;

      if (spacing_delta < -EPSILON_RELAXED) {
        // spacing becomes closer. the last note of the group is removed
        // from the group (which is then annotated), and that note forms
        // a new group along with the new note.
        size_t prev = g->last;
        g->last = g->prev;
        g->type = group_type_transitions[g->type][TRANSITION_BACKSTEP];

        annotate_group_(branch, g, g->spacing);
        start_group_(g);
        add_note_(g, branch, prev, NAN);
        add_note_(g, branch, i, spacing);
      } else if (spacing_delta > EPSILON_RELAXED) {
        // spacing becomes farther. the current group is annotated; the
        // new note forms a new group.
        annotate_group_(branch, g, spacing);
        start_group_(g);
        add_note_(g, branch, i, spacing);
      } else {
        // spacing is similar. new note is added to current group.
        add_note_(g, branch, i, spacing);
      }
    }

    state->bpm = state->next_bpm;
  }
}

static void init_state_(annotator_state *state, const taco_section *branch) {
  // sane initial bpm values
  state->bpm = 120;
  state->next_bpm = 120;
  state->hi_speed = 1.0;
  state->group.spacing = NAN;
  state->group.type = TYPE_EMPTY;
  state->group.time = -0x20000000;
}

int tja_pass_annotate_(tja_parser *parser, taco_section *branch) {
  annotator_state state;
  init_state_(&state, branch);

  size_t count = taco_section_size(branch);

  for (int i = 0; i < count; ++i) {
    process_event_(branch, i, &state);
  }

  annotate_group_(branch, &state.group, INFINITY);
  return 0;
}
