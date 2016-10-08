#include "mcg.h"
#include "mcgg.h"


#define PANIC printf

#define burm_assert(b, s) assert(b)

extern void burm_panic_cannot_match(NODEPTR_TYPE node);

extern bool burm_predicate_constant_signed_16_bit(struct burm_node* node);
extern bool burm_predicate_constant_is_zero(struct burm_node* node);

/* vim: set sw=4 ts=4 expandtab : */

