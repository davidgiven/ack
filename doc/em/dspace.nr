.bp
.P1 "DATA ADDRESS SPACE"
.PP
The data address space is divided into three parts, called 'areas',
each with its own addressing method:
global data area,
local data area (including the stack),
and heap data area.
These data areas must be part of the same
address space because all data is accessed by
the same type of pointers.
.PP
Space for global data is reserved using several pseudoinstructions in the
assembly language, as described in
the next paragraph and chapter 11.
The size of the global data area is fixed per program.
.QQ
Global data is addressed absolutely in the machine language.
Many instructions are available to address global data.
They all have an absolute address as argument.
Examples are LOE, LAE and STE.
.PP
Part of the global data area is initialized by the
compiler, the
rest is not initialized at all or is initialized
with a value, typically \-32768 or 0.
Part of the initialized global data may be made read-only
if the implementation supports protection.
.PP
The local data area is used as a stack,
which grows from high to low addresses
and contains some data for each active procedure
invocation, called a 'frame'.
The size of the local data area varies dynamically during
execution.
Below the current procedure frame resides the operand stack.
The stack pointer SP always points to the bottom of
the local data area.
Local data is addressed by offsetting from the local base pointer LB.
LB always points to the frame of the current procedure.
Only the words of the current frame and the parameters
can be addressed directly.
Variables in other active procedures are addressed by following
the chain of statically enclosing procedures using the LXL or LXA instruction.
The variables in dynamically enclosing procedures can be
addressed with the use of the DCH instruction.
.QQ
Many instructions have offsets to LB as argument,
for instance LOL, LAL and STL.
The arguments of these instructions range from \-1 to some
(negative) minimum
for the access of local storage and from 0 to some (positive)
maximum for parameter access.
.PP
The procedure call instructions CAL and CAI each create a new frame
on the stack.
Each procedure has an assembly-time parameter specifying
the number of bytes needed for local storage.
This storage is allocated each time the procedure is called and
must be a multiple of the wordsize.
Each procedure, therefore, starts with a stack with the local variables
already allocated.
The return instructions RET and RTT remove a frame.
The actual parameters must be removed by the calling procedure.
.PP
RET may copy some words from the stack of
the returning procedure to an unnamed 'function return area'.
This area is available for 'READ-ONCE' access using the LFR instruction.
The result of a LFR is only defined if the size used to fetch
is identical to the size used in the last return.
The instruction ASP, used to remove the parameters from the
stack, the branch instruction BRA and the non-local goto
instruction GTO are the only ones that leave the contents of
the 'function return area' intact.
All other instructions are allowed to destroy the function
return area.
Thus parameters can be popped before fetching the function result.
The maximum size of all function return areas is
implementation dependent,
but should allow procedure instance identifiers and all
implemented objects of type integer, unsigned, float
and pointer to be returned.
In most implementations
the maximum size of the function return
area is twice the pointer size,
because we want to be able to handle 'procedure instance
identifiers' which consist of a procedure identifier and the LB
of a frame belonging to that procedure.
.PP
The heap data area grows upwards, to higher numbered
addresses.
It is initially empty.
The initial value of the heap pointer HP
marks the low end.
The heap pointer may be manipulated
by the LOR and STR instructions.
The heap can only be addressed indirectly,
by pointers derived from previous values of HP.
.P2 "Global data area"
.PP
The initial size of the global data area is determined at assembly time.
Global data is allocated by several
pseudoinstructions in the EM assembly
language.
Each pseudoinstruction allocates one or more bytes.
The bytes allocated for a single pseudo form
a 'block'.
A block differs from a fragment, because,
under certain conditions, several blocks are allocated
in a single fragment.
This guarantees that the bytes of these blocks
are consecutive.
.PP
Global data is addressed absolutely in binary
machine language.
Most compilers, however,
cannot assign absolute addresses to their global variables,
especially not if the language
allows programs to be composed of several separately compiled modules.
The assembly language therefore allows the compiler to name
the first address of a global data block with an alphanumeric label.
Moreover, the only way to address such a named global data block
in the assembly language is by using its name.
It is the task of the assembler/loader to
translate these labels into absolute addresses.
These labels may also be used
in CON and ROM pseudoinstructions to initialize pointers.
.PP
The pseudoinstruction CON allocates initialized data.
ROM acts like CON but indicates that the initialized data will
not change during execution of the program.
The pseudoinstruction BSS allocates a block of uninitialized
or identically initialized
data.
The pseudoinstruction HOL is similar to BSS,
but it alters the meaning of subsequent absolute addressing in
the assembly language.
.PP
Another type of global data is a small block,
called the ABS block, with an implementation defined size.
Storage in this type of block can only be addressed
absolutely in assembly language.
The first word has address 0 and is used to maintain the
source line number.
Special instructions LIN and LNI are provided to
update this counter.
A pointer at location 4 points to a string containing the
current source file name.
The instruction FIL can be used to update the pointer.
.PP
All numeric arguments of the instructions that address
the global data area refer to locations in the
ABS block unless
they are preceded by at least one HOL pseudo in the same
module,
in which case they refer to the storage area allocated by the
last HOL pseudoinstruction.
Thus LOE 0 loads the zeroth word of the most recent HOL, unless no HOL has
appeared in the current file so
far, in which case it loads the zeroth word of the
ABS fragment.
.PP
The global data area is highly fragmented.
The ABS block and each HOL and BSS block are separate fragments.
The way fragments are formed from CON and ROM blocks is more complex.
The assemblers group several blocks into a single fragment.
A fragment only contains blocks of the same type: CON or ROM.
It is guaranteed that the bytes allocated for two consecutive CON pseudos are
allocated consecutively in a single fragment, unless
these CON pseudos are separated in the assembly language program
by a data label definition or one or more of the following pseudos:
.DS
ROM, BSS, HOL and END
.DE
An analogous rule holds for ROM pseudos.
.P2 "Local data area"
.PP
The local data area consists of a sequence of frames, one for
each active procedure.
Below the frame of the current procedure resides the
expression stack.
Frames are generated by procedure calls and are
removed by procedure returns.
A procedure frame consists of six 'zones':
.DS
1.  The return status block
2.  The local variables and compiler temporaries
3.  The register save block
4.  The dynamic local generators
5.  The operand stack.
6.  The parameters of a procedure one level deeper
.DE
A sample frame is shown in Figure 1.
.PP
Before a procedure call is performed the actual
parameters are pushed onto the stack of the calling procedure.
The exact details are compiler dependent.
EM allows procedures to be called with a variable number of
parameters.
The implementation of the C-language almost forces its runtime
system to push the parameters in reverse order, that is,
the first positional parameter last.
Most compilers use the C calling convention to be compatible.
The parameters of a procedure belong to the frame of the
calling procedure.
Note that the evaluation of the actual parameters may imply
the calling of procedures.
The parameters can be accessed with certain instructions using
offsets of 0 and greater.
The first byte of the last parameter pushed has offset 0.
Note that the parameter at offset 0 has a special use in the
instructions following the static chain (LXL and LXA).
These instructions assume that this parameter contains the LB of
the statically enclosing procedure.
Procedures that do not have a dynamically enclosing procedure
do not need a static link at offset 0.
.PP
Two instructions are available to perform procedure calls, CAL
and CAI.
Several tasks are performed by these call instructions.
.QQ
First, a part of the status of the calling procedure is
saved on the stack in the return status block.
This block should contain the return address of the calling
procedure, its LB and other implementation dependent data.
The size of this block is fixed for any given implementation
because the lexical instructions LPB, LXL and LXA must be able to
obtain the base addresses of the procedure parameters \fBand\fP local
variables.
An alternative solution can be used on machines with a highly
segmented address space.
The stack frames need not be contiguous then and the first
status save area can contain the parameter base AB,
which has the value of SP just after the last parameter has
been pushed.
.QQ
Second, the LB is changed to point to the
first word above the local variables.
The new LB is a copy of the SP after the return status
block has been pushed.
.QQ
Third, the amount of local storage needed by the procedure is
reserved.
The parameters and local storage are accessed by the same instructions.
Negative offsets are used for access to local variables.
The highest byte, that is the byte nearest
to LB, has to be accessed with offset \-1.
The pseudoinstruction specifying the entry point of a
procedure, has an argument that specifies the amount of local
storage needed.
The local variables allocated by the CAI or CAL instructions
are the only ones that can be accessed with a fixed negative offset.
The initial value of the allocated words is
not defined, but implementations that check for undefined
values will probably initialize them with a
special 'undefined' pattern, typically \-32768.
.QQ
Fourth, any EM implementation is allowed to reserve a variable size
block beneath the local variables.
This block could, for example, be used to save a variable number
of registers.
.QQ
Finally, the address of the entry point of the called procedure
is loaded into the Program Counter.
.PP
The ASP instruction can be used to allocate further (dynamic)
local storage.
The base address of such storage must be obtained with a LOR~SP
instruction.
This same instruction ASP may also be used
to remove some words from the stack.
.PP
There is a version of ASP, called ASS, which fetches the number
of bytes to allocate from the stack.
It can be used to allocate space for local
objects whose size is unknown at compile time,
so called 'dynamic local generators'.
.PP
Control is returned to the calling procedure with a RET instruction.
Any return value is then copied to the 'function return area'.
The frame created by the call is deallocated and the status of
the calling procedure is restored.
The value of SP just after the return value has been popped must
be the same as the
value of SP just before executing the first instruction of this
invocation.
This means that when a RET is executed the operand stack can
only contain the return value and all dynamically generated locals must be
deallocated.
Violating this restriction might result in hard to detect
errors.
The calling procedure has to remove the parameters from the stack.
This can be done with the aforementioned ASP instruction.
.PP
Each procedure frame is a separate fragment.
Because any fragment may be placed anywhere in memory,
procedure frames need not be contiguous.
.Dr 47
                |===============================|
                |     actual parameter  n-1     |
                |-------------------------------|
                |              .                |
                |              .                |
                |              .                |
                |-------------------------------|
                |     actual parameter  0       | ( <\- AB )
                |===============================|


                |===============================|
                |///////////////////////////////|
                |///// return status block /////|
                |///////////////////////////////|   <\- LB
                |===============================|
                |                               |
                |       local variables         |
                |                               |
                |-------------------------------|
                |                               |
                |      compiler temporaries     |
                |                               |
                |===============================|
                |///////////////////////////////|
                |///// register save block /////|
                |///////////////////////////////|
                |===============================|
                |                               |
                |   dynamic local generators    |
                |                               |
                |===============================|
                |           operand             |
                |-------------------------------|
                |           operand             |
                |===============================|
                |         parameter  m-1        |
                |-------------------------------|
                |              .                |
                |              .                |
                |              .                |
                |-------------------------------|
                |         parameter  0          | <\- SP
                |===============================|
.Df
Figure 1. A sample procedure frame and parameters.
.De
.P2 "Heap data area"
.PP
The heap area starts empty, with HP
pointing to the low end of it.
HP always contains a word address.
A copy of HP can always be obtained with the LOR instruction.
A new value may be stored in the heap pointer using the STR instruction.
If the new value is greater than the old one,
then the heap grows.
If it is smaller, then the heap shrinks.
HP may never point below its original value.
All words between the current HP and the original HP
are allocated to the heap.
The heap may not grow into a part of memory that is already allocated.
When this is attempted, the STR instruction will cause a trap to occur.
In this case, HP retains its old value.
.PP
The only way to address the heap is indirectly.
Whenever an object is allocated by increasing HP,
then the old HP value must be saved and can be used later to address
the allocated object.
If, in the meantime, HP is decreased so that the object
is no longer part of the heap, then an attempt to access
the object is not allowed.
Furthermore, if the heap pointer is increased again to above
the object address, then access to the old object gives undefined results.
.PP
The heap is a single fragment.
All bytes have consecutive addresses.
No limits are imposed on the size of the heap as long as it fits
in the available data address space.
