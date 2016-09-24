#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#define TRACE

#define STATE_TYPE void*
typedef struct tree {
	int op;
	struct tree *kids[2];
	STATE_TYPE state_label;
} *NODEPTR_TYPE;
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->state_label)
#define PANIC printf

static void burm_trace(NODEPTR_TYPE p, int eruleno, int cost, int bestcost) {
#ifdef TRACE
	extern const char *burm_string[];

	fprintf(stderr, "0x%p matched %s with cost %d vs. %d\n", p,
		burm_string[eruleno], cost, bestcost);
#endif
}

#define burm_assert(b, s) assert(b && s)

