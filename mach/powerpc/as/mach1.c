/*
 * $Source$
 * $State$
 */

/*
 * Do not #include anything here.  Do it in mach/proto/as/comm0.h
 */

void no_hl(void);
word_t eval_hl(struct expr_t* expr, int token);
void emit_hl(word_t in);
void machfinish(int n);
