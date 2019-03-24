/* $Header; mach1.c, v1.1 06-Mar-89 AJM */
extern word_t opcode;
extern int success;	/* LDR/STR address failure flag */


/* ARM32 Specific routines */
void branch(word_t brtyp, word_t link, valu_t val);
void data(long opc, long ins, valu_t val, short typ);
int calcimm(word_t *opc, valu_t *val,short typ);
word_t calcoffset(valu_t val);
void strldr(long opc, long ins, valu_t val);
void calcadr(word_t ins, word_t reg, valu_t val, short typ);
word_t calcshft(valu_t val, short typ, word_t styp);
void rotateleft2(long *x);
void putaddr(long opc, long ins, long val, int count);
int oursmall(int fitsmall, int gain);
