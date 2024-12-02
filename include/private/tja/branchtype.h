/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_BRANCHTYPE_H_
#define TJA_BRANCHTYPE_H_

#define BRANCHTYPE_CLASS_INTEGER 0
#define BRANCHTYPE_CLASS_PERCENTAGE 1

extern int tja_branch_type_(const char *code);
extern int tja_branchtype_convert_threshold_(int type, double value);

extern int tja_branchtype_class(int type);

#endif /* !TJA_BRANCHTYPE_H_ */
