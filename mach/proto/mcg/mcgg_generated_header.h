#include "mcg.h"
#include "mcgg.h"

#define PANIC printf

#define burm_assert(b, s) assert(b)

extern void burm_panic_cannot_match(NODEPTR_TYPE node);

extern bool burm_predicate_signed_constant(struct burm_node* node, arith size);
extern bool burm_predicate_unsigned_constant(struct burm_node* node, arith size);
extern bool burm_predicate_specific_constant(struct burm_node* node, arith value);

/* vim: set sw=4 ts=4 expandtab : */

