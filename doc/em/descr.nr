.bp
.P1 "DESCRIPTORS"
.PP
Several instructions use descriptors, notably the range check instruction,
the array instructions, the goto instruction and the case jump instructions.
Descriptors reside in data space.
They may be constructed at run time, but
more often they are fixed and allocated in ROM data.
.PP
All instructions using descriptors, except GTO, have as argument
the size of the integers in the descriptor.
All implementations have to allow integers of the size of a
word in descriptors.
All integers popped from the stack and used for indexing or comparing
must have the same size as the integers in the descriptor.
.P2 "Range check descriptors"
.PP
Range check descriptors consist of two integers:
.IP 1.
lower bound	signed
.IP 2.
upper bound	signed
.LP
The range check instruction checks an integer on the stack against
these bounds and causes a trap if the value is outside the interval.
The value itself is neither changed nor removed from the stack.
.P2 "Array descriptors"
.PP
Each array descriptor describes a single dimension.
For multi-dimensional arrays, several array instructions are
needed to access a single element.
Array descriptors contain the following three integers:
.IP 1.
lower bound		signed
.IP 2.
upper bound \- lower bound	unsigned
.IP 3.
number of bytes per element	unsigned
.LP
The array instructions LAR, SAR and AAR have the pointer to the start
of the descriptor as operand on the stack.
.LP
The element A[I] is fetched as follows:
.IP 1.
Stack the address of A  (e.g., using LAE or LAL)
.IP 2.
Stack the value of I (n-byte integer)
.IP 3.
Stack the pointer to the descriptor (e.g., using LAE)
.IP 4.
LAR n (n is the size of the integers in the descriptor and I)
.LP
All array instructions first pop the address of the descriptor
and the index.
If the index is not within the bounds specified, a trap occurs.
If ok, (I~\-~lower bound) is multiplied
by the number of bytes per element (the third word).  The result is added
to the address of A and replaces A on the stack.
.QQ
At this point LAR, SAR and AAR diverge.
AAR is finished.  LAR pops the address and fetches the data
item,
the size being specified by the descriptor.
The usual restrictions for memory access must be obeyed.
SAR pops the address and stores the
data item now exposed.
.P2 "Non-local goto descriptors"
.PP
The GTO instruction provides a way of returning directly to any
active procedure invocation.
The argument of the instruction is the address of a descriptor
containing three pointers:
.IP 1.
value of PC after the jump
.IP 2.
value of SP after the jump
.IP 3.
value of LB after the jump
.LP
GTO replaces the loads PC, SP and LB from the descriptor,
thereby jumping to a procedure
and removing zero or more frames from the stack.
The LB, SP and PC in the descriptor must belong to a
dynamically enclosing procedure,
because some EM implementations will need to backtrack through
the dynamic chain and use the implementation dependent data
in frames to restore registers etc.
.P2 "Case descriptors"
.PP
The case jump instructions CSA and CSB both
provide multiway branches selected by a case index.
Both fetch two operands from the stack:
first a pointer to the low address of the case descriptor
and then the case index.
CSA uses the case index as index in the descriptor table, but CSB searches
the table for an occurrence of the case index.
Therefore, the descriptors for CSA and CSB,
as shown in figure 4, are different.
All pointers in the table must be addresses of instructions in the
procedure executing the case instruction.
.PP
CSA selects the new PC by indexing.
If the index, a signed integer, is greater than or equal to
the lower bound and less than or equal to the upper bound,
then fetch the new PC from the list of instruction pointers by indexing with
index-lower.
The table does not contain the value of the upper bound,
but the value of upper-lower as an unsigned integer.
The default instruction pointer is used when the index is out of bounds.
If the resulting PC is 0, then trap.
.PP
CSB selects the new PC by searching.
The table is searched for an entry with index value equal to the case index.
That entry or, if none is found, the default entry contains the
new PC.
When the resulting PC is 0, a trap is performed.
.PP
The choice of which case instruction to use for
each source language case statement
is up to the front end.
If the range of the index value is dense, i.e
.DS
(highest value \- lowest value) / number of cases
.DE
is less than some threshold, then CSA is the obvious choice.
If the range is sparse, CSB is better.
.Dr 30
   |--------------------|        |--------------------|  high address
   | pointer for upb    |        |    pointer n-1     |
   |--------------------|        |-  -  -  -  -  -  - |
   |         .          |        |     index  n-1     |
   |         .          |        |--------------------|
   |         .          |        |          .         |
   |         .          |        |          .         |
   |         .          |        |          .         |
   |         .          |        |--------------------|
   |         .          |        |    pointer  1      |
   |--------------------|        |-  -  -  -  -  -  - |
   | pointer for lwb+1  |        |     index   1      |
   |--------------------|        |--------------------|
   | pointer for lwb    |        |    pointer  0      |
   |--------------------|        |-  -  -  -  -  -  - |
   |   upper - lower    |        |     index   0      |
   |--------------------|        |--------------------|
   |    lower bound     |        | number of entries  |
   |--------------------|        |--------------------|
   |  default pointer   |        |  default pointer   |  low address
   |--------------------|        |--------------------|

       CSA descriptor                CSB descriptor
.Df
Figure 4. Descriptor layout for CSA and CSB
.De
