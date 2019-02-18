1i\
/* this part is generated from em_table */
# Remove pseudo instructions
/^\(...\)	\([0-9]\).*/d
# Remove opcode constant categories/count
/^\(....\)	\([0-9]\).*/d
/^\(.....\)	\([0-9]\).*/d
# Below are the actual replacement of opcodes.
s/^\(...\)	\(d\).*/#define C_\1(c) CC_opcst(op_\1, c)/
s/^\(...\)	\([cslnfzor]\).*/#define C_\1(c) CC_opcst(op_\1, c)/
s/^\(...\)	\(w\).*/#define C_\1(w) CC_opcst(op_\1, w)\
#define C_\1_narg() CC_opnarg(op_\1)/
s/^\(...\)	\(g\).*/#define C_\1(g) CC_opcst(op_\1, g)\
#define C_\1_dnam(g, o) CC_opdnam(op_\1, g, o)\
#define C_\1_dlb(g, o) CC_opdlb(op_\1, g, o)/
s/^\(...\)	\(p\).*/#define C_\1(p) CC_oppnam(op_\1, p)/
s/^\(...\)	\(b\).*/#define C_\1(b) CC_opilb(op_\1, b)/
s/^\(...\)	\(-\).*/#define C_\1() CC_op(op_\1)/
