.bp
.P1 "INTRODUCTION"
.PP
EM is a family of intermediate languages designed for producing
portable compilers.
The general strategy is for a program called \fBfront end\fP
to translate the source program to EM.
Another program, \fBback end\fP,
translates EM to target assembly language.
Alternatively, the EM code can be assembled to a binary form
and interpreted.
These considerations led to the following goals:
.IP 1
The design should allow translation to,
or interpretation on, a wide range of existing machines.
Design decisions should be delayed as far as possible
and the implications of these decisions should
be localized as much as possible.
.br
The current microcomputer technology offers 8, 16 and 32 bit machines
with various sizes of address space.
EM should be flexible enough to be useful on most of these
machines.
The differences between the members of the EM family should only
concern the wordsize and address space size.
.IP 2
The architecture should ease the task of code generation for
high level languages such as Pascal, C, Ada, Algol 68, BCPL.
.IP 3
The instruction set used by the interpreter should be compact,
to reduce the amount of memory needed
for program storage, and to reduce the time needed to transmit
programs over communication lines.
.IP 3
It should be designed with microprogrammed implementations in
mind; in particular, the use of many short fields within
instruction opcodes should be avoided, because their extraction by the
microprogram or conversion to other instruction formats is inefficient.
.PP
The basic architecture is based on the concept of a stack. The stack
is used for procedure return addresses, actual parameters, local variables,
and arithmetic operations.
There are several built-in object types,
for example, signed and unsigned integers,
floating point numbers, pointers and sets of bits.
There are instructions to push and pop objects
to and from the stack.
The push and pop instructions are not typed.
They only care about the size of the objects.
For each built-in type there are
reverse Polish type instructions that pop one or more
objects from the top of
the stack, perform an operation, and push the result back onto the
stack.
For all types except pointers,
these instructions have the object size
as argument.
.PP
There are no visible general registers used for arithmetic operands
etc. This is in contrast to most third generation computers, which usually
have 8 or 16 general registers. The decision not to have a group of
general registers was fully intentional, and follows W.L. Van der
Poel's dictum that a machine should have 0, 1, or an infinite
number of any feature. General registers have two primary uses: to hold
intermediate results of complicated expressions, e.g.
.DS
((a*b + c*d)/e + f*g/h) * i
.DE
and to hold local variables.
.PP
Various studies
have shown that the average expression has fewer than two operands,
making the former use of registers of doubtful value. The present trend
toward structured programs consisting of many small
procedures greatly reduces the value of registers to hold local variables
because the large number of procedure calls implies a large overhead in
saving and restoring the registers at every call.
.PP
Although there are no general purpose registers, there are a
few internal registers with specific functions as follows:
.TS
tab(:);
l 1 l l l.
PC:\-:Program Counter:Pointer to next instruction
LB:\-:Local Base:Points to base of the local variables
:::in the current procedure.
SP:\-:Stack Pointer:Points to the highest occupied word on the stack.
HP:\-:Heap Pointer:Points to the top of the heap area.
.TE
.PP
Furthermore, reverse Polish code is much easier to generate than
multi-register machine code, especially if highly efficient code is
desired.
When translating to assembly language the back end can make
good use of the target machine's registers.
An EM machine can
achieve high performance by keeping part of the stack
in high speed storage (a cache or microprogram scratchpad memory) rather
than in primary memory.
.PP
Again according to van der Poel's dictum,
all EM instructions have zero or one argument.
We believe that instructions needing two arguments
can be split into two simpler ones.
The simpler ones can probably be used in other
circumstances as well.
Moreover, these two instructions together often
have a shorter encoding than the single
instruction before.
.PP
This document describes EM at three different levels:
the abstract level, the assembly language level and
the machine language level.
.QQ
The most important level is that of the abstract EM architecture.
This level deals with the basic design issues.
Only the functional capabilities of instructions are relevant, not their
format or encoding.
Most chapters of this document refer to the abstract level
and it is explicitly stated whenever
another level is described.
.QQ
The assembly language is intended for the compiler writer.
It presents a more or less orthogonal instruction
set and provides symbolic names for data.
Moreover, it facilitates the linking of
separately compiled 'modules' into a single program
by providing several pseudoinstructions.
.QQ
The machine language is designed for interpretation with a compact
program text and easy decoding.
The binary representation of the machine language instruction set is
far from orthogonal.
Frequent instructions have a short opcode.
The encoding is fully byte oriented.
These bytes do not contain small bit fields, because
bit fields would slow down decoding considerably.
.PP
A common use for EM is for producing portable (cross) compilers.
When used this way, the compilers produce
EM assembly language as their output.
To run the compiled program on the target machine,
the back end, translates the EM assembly language to
the target machine's assembly language.
When this approach is used, the format of the EM
machine language instructions is irrelevant.
On the other hand, when writing an interpreter for EM machine language
programs, the interpreter must deal with the machine language
and not with the symbolic assembly language.
.PP
As mentioned above, the
current microcomputer technology offers 8, 16 and 32 bit
machines with address spaces ranging from
.Ex 2 16
to
.Ex 2 32
bytes.
Having one size of pointers and integers restricts
the usefulness of the language.
We decided to have a different language for each combination of
word and pointer size.
All languages offer the same instruction set and differ only in
memory alignment restrictions and the implicit size assumed in
several instructions.
The languages
differ slightly for the
different size combinations.
For example: the
size of any object on the stack and alignment restrictions.
The wordsize is restricted to powers of 2 and
the pointer size must be a multiple of the wordsize.
Almost all programs handling EM will be parametrized with word
and pointer size.
