.SN 5
.BP
.S1 "MAPPING OF EM DATA MEMORY ONTO TARGET MACHINE MEMORY"
The EM architecture is designed to be implemented
on many existing and future machines.
EM memory is highly fragmented to make
adaptation to various memory architectures possible.
Format and encoding of pointers is explicitly undefined.
.P
This chapter gives solutions to some of the
anticipated problems.
First, we describe a possible memory layout for machines
with 64K bytes of address space.
Here we use a member of the EM family with 2-byte word and pointer
size.
The most straightforward layout is shown in figure 2.
.Dr 40
       65534 \-> |-------------------------------|
                |///////////////////////////////|
                |//// unimplemented memory /////|
                |///////////////////////////////|
          ML \-> |-------------------------------|
                |                               |
                |                               | <\- LB
                |     stack and local area      |
                |                               |
                |-------------------------------| <\- SP
                |///////////////////////////////|
                |//////// inaccessible /////////|
                |///////////////////////////////|
                |-------------------------------| <\- HP
                |                               |
                |           heap area           |
                |                               |
                |                               |
          HB \-> |-------------------------------|
                |                               |
                |       global data area        |
                |                               |
          EB \-> |-------------------------------|
                |                               |
                |         program text          | <\- PC
                |                               |
                |        ( and tables )         |
                |                               |
                |                               |
          PB \-> |-------------------------------|
                |///////////////////////////////|
                |////////// undefined //////////|
                |///////////////////////////////|
           0 \-> |-------------------------------|
.Df
Figure 2.  Memory layout showing typical register
positions during execution of an EM program.
.De
.N 1
The base registers for the various memory pieces can be stored
in target machine registers or memory.
.IS
.N 1
.TS
tab(;);
l 1 l l l.
PB;:;program base;points to the base of the instruction address space.
EB;:;external base;points to the base of the data address space.
HB;:;heap base;points to the base of the heap area.
ML;:;memory limit;marks the high end of the addressable data space.
.TE 1
.IE
The stack grows from high
EM addresses to low EM addresses, and the heap the
other way.
The memory between SP and HP is not accessible,
but may be allocated later to the stack or the heap if needed.
The local data area is allocated starting at the high end of
memory.
.P
Because EM address 0 is not mapped onto target
address 0, a problem arises when pointers are used.
If a program pushed a constant, say 6, onto the stack,
and then tried to indirect through it,
the wrong word would be fetched,
because EM address 6 is mapped onto target address EB+6
and not target address 6 itself.
This particular problem is solved by explicitly declaring
the format of a pointer to be undefined,
so that using a constant as a pointer is completely illegal.
However, the general problem of mapping pointers still exists.
.P
There are two possible solutions.
In the first solution, EM pointers are represented
in the target machine as true EM addresses,
for example, a pointer to EM address 6 really is
stored as a 6 in the target machine.
This solution implies that every time a pointer is fetched
EB must be added before referencing
the target machine's memory.
If the target machine has powerful indexing
facilities, EB can be kept in a target machine register,
and the relocation can indeed be done on
every reference to the data address space
at a modest cost in speed.
.P
The other solution consists of having EM pointers
refer to the true target machine address.
Thus the instruction LAE 6 (Load Address of External 6)
would push the value of EB+6 onto the stack.
When this approach is chosen, back ends must know
how to offset from EB, to translate all
instructions that manipulate EM addresses.
However, the problem is not completely solved,
because a front end may have to initialize a pointer
in CON or ROM data to point to a global address.
This pointer must also be relocated by the back end or the interpreter.
.P
Although the EM stack grows from high to low EM addresses,
some machines have hardware PUSH and POP
instructions that require the stack to grow upwards.
If reasons of efficiency demand the use of these
instructions, then EM
can be implemented with the memory layout
upside down, as shown in figure 3.
This is possible because the pointer format is explicitly undefined.
The first element of a word array will have a
lower physical address than the second element.
.Dr 18
          |                 |                    |                 |
          |      EB=60      |                    |        ^        |
          |                 |                    |        |        |
          |-----------------|                    |-----------------|
      105 |   45   |   44   | 104            214 |   41   |   40   | 215
          |-----------------|                    |-----------------|
      103 |   43   |   42   | 102            212 |   43   |   42   | 213
          |-----------------|                    |-----------------|
      101 |   41   |   40   | 100            210 |   45   |   44   | 211
          |-----------------|                    |-----------------|
          |        |        |                    |                 |
          |        v        |                    |      EB=255     |
          |                 |                    |                 |

                Type A                                 Type B
.Df
Figure 3. Two possible memory implementations.
Numbers within the boxes are EM addresses.
The other numbers are physical addresses.
.De
.A 1 0
So, we have two different EM memory implementations:
.IS
.PS - 4
.PT A~\-
stack downwards
.PT B~\-
stack upwards
.PE
.IE
.P
For each of these two possibilities we give the translation of
the EM instructions to push the third byte of a global data
block starting at EM address 40 onto the stack and to load the
word at address 40.
All translations assume a word and pointer size of two bytes.
The target machine used is a PDP-11 augmented with push and pop instructions.
Registers 'r0' and 'r1' are used and suffer from sign extension for byte
transfers.
Push $40 means push the constant 40, not word 40.
.P
The translation of the EM instructions depends on the pointer representation
used.
For each of the two solutions explained above the translation is given.
.P
First, the translation for the two implementations using EM addresses as
pointer representation:
.DS
.TS
tab(:), center;
l s l s l s
_ s _ s _ s
l 2 l 6 l 2 l 6 l 2 l.
EM:type A:type B


LAE:40:push:$40:push:$40

ADP:3:pop:r0:pop:r0
::add:$3,r0:add:$3,r0
::push:r0:push:r0

LOI:1:pop:r0:pop:r0
::\-::neg:r0
::clr:r1:clr:r1
::bisb:eb(r0),r1:bisb:eb(r0),r1
::push:r1:push:r1

LOE:40:push:eb+40:push:eb-41
.TE
.DE
.P
The translation for the two implementations, if the target machine address is
used as pointer representation, is:
.N 1
.DS
.TS
tab(:), center;
l s l s l s
_ s _ s _ s
l 2 l 6 l 2 l 6 l 2 l.
EM:type A:type B


LAE:40:push:$eb+40:push:$eb-40

ADP:3:pop:r0:pop:r0
::add:$3,r0:sub:$3,r0
::push:r0:push:r0

LOI:1:pop:r0:pop:r0
::clr:r1:clr:r1
::bisb:(r0),r1:bisb:(r0),r1
::push:r1:push:r1

LOE:40:push:eb+40:push:eb-41
.TE
.DE
.P
The translation presented above is not intended to be optimal.
Most machines can handle these simple cases in one or two instructions.
It demonstrates, however, the flexibility of the EM design.
.P
There are several possibilities to implement EM on machines with
address spaces larger than 64k bytes.
For EM with two byte pointers one could allocate instruction and
data space each in a separate 64k piece of memory.
EM pointers still have to fit in two bytes,
but the base registers PB and EB may be loaded in hardware registers
wider than 16 bits, if available.
EM implementations can also make efficient use of a machine
with separate instruction and data space.
.P
EM with 32 bit pointers allows one to make use of machines
with large address spaces.
In a virtual, segmented memory system one could use a separate
segment for each fragment.
