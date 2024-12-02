/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_POSTPROC_H_
#define TJA_POSTPROC_H_

#include "parser.h"
#include "taco.h"

extern void tja_pass_convert_time_(tja_parser *parser, taco_section *section);
extern void tja_pass_checkpoint_rolls_(tja_parser *parser,
                                       taco_section *section);
extern void tja_pass_compile_branches_(tja_parser *parser,
                                       taco_section *branch);
extern void tja_pass_cleanup_(tja_parser *parser, taco_section *section);

#endif /* !TJA_POSTPROC_H_ */
