.BP
.AP "EM CODE TABLES"
The following table is used by the assembler for EM machine
language.
It specifies the opcodes used for each instruction and
how arguments are mapped to machine language arguments.
The table is presented in three columns,
each line in each column contains three or four fields.
Each line describes a range of interpreter opcodes by
specifying for which instruction the range is used, the type of the
opcodes (mini, shortie, etc..) and range for the instruction
argument.
.A
The first field on each line gives the EM instruction mnemonic,
the second field gives some flags.
If the opcodes are minis or shorties the third field specifies
how many minis/shorties are used.
The last field gives the number of the (first) interpreter
opcode.
.N 1
Flags :
.IS 3
.N 1
Opcode type, only one of the following may be specified.
.PS - 5 "  "
.PT \-
opcode without argument
.PT m
mini
.PT s
shortie
.PT 2
opcode with 2-byte signed argument
.PT 4
opcode with 4-byte signed argument
.PT 8
opcode with 8-byte signed argument
.PT u
opcode with 2-byte unsigned argument
.PE
Secondary (escaped) opcodes.
.PS - 5 "  "
.PT e
The opcode thus marked is in the secondary opcode group instead
of the primary
.PE
restrictions on arguments
.PS - 5 "  "
.PT N
Negative arguments only
.PT P
Positive and zero arguments only
.PE
mapping of arguments
.PS - 5 "  "
.PT w
argument must be divisible by the wordsize and is divided by the
wordsize before use as opcode argument.
.PT o
argument ( possibly after division ) must be >= 1 and is
decremented before use as opcode argument
.PE
.IE
If the opcode type is 2,4 or 8 the resulting argument is used as
opcode argument (least significant byte first).
.N
If the opcode type is mini, the argument is added
to the first opcode \- if in range \- .
If the argument is negative, the absolute value minus one is
used in the algorithm above.
.N
For shorties with positive arguments the first opcode is used
for arguments in the range 0..255, the second for the range
256..511, etc..
For shorties with negative arguments the first opcode is used
for arguments in the range \-1..\-256, the second for the range
\-257..\-512, etc..
The byte following the opcode contains the least significant
byte of the argument.
First some examples of these specifications.
.PS - 5
.PT "aar mwPo 1 34"
Indicates that opcode 34 is used as a mini for Positive
instruction arguments only.
The w and o indicate division and decrementing of the
instruction argument.
Because the resulting argument must be zero ( only opcode 34 may be used
), this mini can only be used for instruction argument 2.
Conclusion: opcode 34 is for "AAR 2".
.PT "adp sP 1 41"
Opcode 41 is used as shortie for ADP with arguments in the range
0..255.
.PT "bra sN 2 60"
Opcode 60 is used as shortie for BRA with arguments \-1..\-256,
61 is used for arguments \-257..\-512.
.PT "zer e\- 145"
Escaped opcode 145 is used for ZER.
.PE
The interpreter opcode table:
.N 1
.IS 3
.so itables
.IE
.P
The table above results in the following dispatch tables.
Dispatch tables are used by interpreters to jump to the
routines implementing the EM instructions, indexed by the next opcode.
Each line of the dispatch tables gives the routine names
of eight consecutive opcodes, preceded by the first opcode number
on that line.
Routine names consist of an EM mnemonic followed by a suffix.
The suffices show the encoding used for each opcode.
.N
The following suffices exist:
.N 1
.VS 1 0
.IS 4
.PS - 11
.PT .z
no arguments
.PT .l
16-bit argument
.PT .L
32-bit argument
.PT .u
16-bit unsigned argument
.PT .lw
16-bit argument divided by the wordsize
.PT .Lw
32-bit argument divided by the wordsize
.PT .p
positive 16-bit argument
.PT .P
positive 32-bit argument
.PT .pw
positive 16-bit argument divided by the wordsize
.PT .Pw
positive 32-bit argument divided by the wordsize
.PT .n
negative 16-bit argument
.PT .N
negative 32-bit argument
.PT .nw
negative 16-bit argument divided by the wordsize
.PT .Nw
negative 32-bit argument divided by the wordsize
.PT .s<num>
shortie with <num> as high order argument byte
.PT .w<num>
shortie with argument divided by the wordsize
.PT .<num>
mini with <num> as argument
.PT .<num>W
mini with <num>*wordsize as argument
.PE 1
<num> is a possibly negative integer.
.VS 1 1
.IE
The dispatch table for the 256 primary opcodes:
.N 1
.so dispat1
.N 2
The list of secondary opcodes (escape1):
.N 1
.so dispat2
.N 2
Finally, the list of opcodes with four byte arguments (escape2).
.N 1
.so dispat3
