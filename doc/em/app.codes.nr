.bp
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
.QQ
The first field on each line gives the EM instruction mnemonic,
the second field gives some flags.
If the opcodes are minis or shorties the third field specifies
how many minis/shorties are used.
The last field gives the number of the (first) interpreter
opcode.
.LP
Flags :
.IP ""
Opcode type, only one of the following may be specified.
.RS
.IP \-
opcode without argument
.IP m
mini
.IP s
shortie
.IP 2
opcode with 2-byte signed argument
.IP 4
opcode with 4-byte signed argument
.IP 8
opcode with 8-byte signed argument
.IP u
opcode with 2-byte unsigned argument
.RE
.IP ""
Secondary (escaped) opcodes.
.RS
.IP e
The opcode thus marked is in the secondary opcode group instead
of the primary
.RE
.IP ""
restrictions on arguments
.RS
.IP N
Negative arguments only
.IP P
Positive and zero arguments only
.RE
.IP ""
mapping of arguments
.RS
.IP w
argument must be divisible by the wordsize and is divided by the
wordsize before use as opcode argument.
.IP o
argument ( possibly after division ) must be >= 1 and is
decremented before use as opcode argument
.RE
.LP
If the opcode type is 2,4 or 8 the resulting argument is used as
opcode argument (least significant byte first).
If the opcode type is mini, the argument is added
to the first opcode \- if in range \- .
If the argument is negative, the absolute value minus one is
used in the algorithm above.
.br
For shorties with positive arguments the first opcode is used
for arguments in the range 0..255, the second for the range
256..511, etc..
For shorties with negative arguments the first opcode is used
for arguments in the range \-1..\-256, the second for the range
\-257..\-512, etc..
The byte following the opcode contains the least significant
byte of the argument.
First some examples of these specifications.
.IP "aar mwPo 1 34"
.br
Indicates that opcode 34 is used as a mini for Positive
instruction arguments only.
The w and o indicate division and decrementing of the
instruction argument.
Because the resulting argument must be zero ( only opcode 34 may be used),
this mini can only be used for instruction argument 2.
Conclusion: opcode 34 is for "AAR 2".
.IP "adp sP 1 41"
.br
Opcode 41 is used as shortie for ADP with arguments in the range
0..255.
.IP "bra sN 2 60"
.br
Opcode 60 is used as shortie for BRA with arguments \-1..\-256,
61 is used for arguments \-257..\-512.
.IP "zer e\- 145"
.br
Escaped opcode 145 is used for ZER.
.LP
The interpreter opcode table:
.DS
.so itables
.DE
.PP
The table above results in the following dispatch tables.
Dispatch tables are used by interpreters to jump to the
routines implementing the EM instructions, indexed by the next opcode.
Each line of the dispatch tables gives the routine names
of eight consecutive opcodes, preceded by the first opcode number
on that line.
Routine names consist of an EM mnemonic followed by a suffix.
The suffices show the encoding used for each opcode.
.LP
The following suffices exist:
.TS
tab(:);
l l.
\ .z:no arguments
\ .l:16-bit argument
\ .L:32-bit argument
\ .u:16-bit unsigned argument
\ .lw:16-bit argument divided by the wordsize
\ .Lw:32-bit argument divided by the wordsize
\ .p:positive 16-bit argument
\ .P:positive 32-bit argument
\ .pw:positive 16-bit argument divided by the wordsize
\ .Pw:positive 32-bit argument divided by the wordsize
\ .n:negative 16-bit argument
\ .N:negative 32-bit argument
\ .nw:negative 16-bit argument divided by the wordsize
\ .Nw:negative 32-bit argument divided by the wordsize
\ .s<num>:shortie with <num> as high order argument byte
\ .w<num>:shortie with argument divided by the wordsize
\ .<num>:mini with <num> as argument
\ .<num>W:mini with <num>*wordsize as argument
.TE
.LP
<num> is a possibly negative integer.
.LP
The dispatch table for the 256 primary opcodes:
.sp 1
.so dispat1
.sp 2
The list of secondary opcodes (escape1):
.sp 1
.so dispat2
.sp 2
Finally, the list of opcodes with four byte arguments (escape2).
.sp 1
.so dispat3
