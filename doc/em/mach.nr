.BP
.SN 10
.S1 "EM MACHINE LANGUAGE"
The EM machine language is designed to make program text compact
and to make decoding easy.
Compact program text has many advantages: programs execute faster,
programs occupy less primary and secondary storage and loading
programs into satellite processors is faster.
The decoding of EM machine language is so simple,
that it is feasible to use interpreters as long as EM hardware
machines are not available.
This chapter is irrelevant when back ends are used to
produce executable target machine code.
.S2 "Instruction encoding"
A design goal of EM is to make the
program text as compact as possible.
Decoding must be easy, however.
The encoding is fully byte oriented, without any small bit fields.
There are 256 primary opcodes, two of which are an escape to
two groups of 256 secondary opcodes each.
.A
EM instructions without arguments have a single opcode assigned,
possibly escaped:
.DS

         |--------------|
         |    opcode    |
         |--------------|

                or

         |--------------|--------------|
         |    escape    |     opcode   |
         |--------------|--------------|

.DE
The encoding for instructions with an argument is more complex.
Several instructions have an address from the global data area
as argument.
Other instructions have different opcodes for positive
and negative arguments.
.N 1
There is always an opcode that takes the next two bytes as argument,
high byte first:
.DS

         |--------------|--------------|--------------|
         |    opcode    |    hibyte    |    lobyte    |
         |--------------|--------------|--------------|

                or

         |--------------|--------------|--------------|--------------|
         |    escape    |    opcode    |    hibyte    |    lobyte    |
         |--------------|--------------|--------------|--------------|

.DE
.DS
An extra escape is provided for instructions with four or eight byte arguments.

  |--------------|--------------|--------------|   |--------------|
  |    ESCAPE    |    opcode    |    hibyte    |...|    lobyte    |
  |--------------|--------------|--------------|   |--------------|

.DE
For most instructions some argument values predominate.
The most frequent combinations of instruction and argument
will be encoded in a single byte, called a mini:
.DS

         |---------------|
         |opcode+argument|  (mini)
         |---------------|

.DE
The number of minis is restricted, because only
254 primary opcodes are available.
Many instructions have the bulk of their arguments
fall in the range 0 to 255.
Instructions that address global data have their arguments
distributed over a wider range,
but small values of the high byte are common.
For all these cases there is another encoding
that combines the instruction and the high byte of the argument
into a single opcode.
These opcodes are called shorties.
Shorties may be escaped.
.DS

         |--------------|--------------|
         | opcode+high  |    lobyte    |  (shortie)
         |--------------|--------------|

                or

         |--------------|--------------|--------------|
         |    escape    | opcode+high  |    lobyte    |
         |--------------|--------------|--------------|

.DE
Escaped shorties are useless if the normal encoding has a primary opcode.
Note that for some instruction-argument combinations
several different encodings are available.
It is the task of the assembler to select the shortest of these.
The savings by these mini and shortie
opcodes are considerable, about 55%.
.P
Further improvements are possible:
the arguments of
many instructions are a multiple of the wordsize.
Some do also not allow zero as an argument.
If these arguments are divided by the wordsize and,
when zero is not allowed, then decremented by 1, more of them can
be encoded as shortie or mini.
The arguments of some other instructions
rarely or never assume the value 0, but start at 1.
The value 1 is then encoded as 0,
2 as 1 and so on.
.P
Assigning opcodes to instructions by the assembler is completely
table driven.
For details see appendix B.
.S2 "Procedure descriptors"
The procedure identifiers used in the interpreter are indices
into a table of procedure descriptors.
Each descriptor contains:
.IS 6
.PS - 4
.PT 1.
the number of bytes to be reserved for locals at each
invocation.
.N
This is a pointer-szied integer.
.PT 2.
the start address of the procedure
.PE
.IE
.S2 "Load format"
The EM machine language load format defines the interface between
the EM assembler/loader and the EM machine itself.
A load file consists of a header, the program text to be executed,
a description of the global data area and the procedure descriptor table,
in this order.
All integers in the load file are presented with the
least significant byte first.
.P
The header has two parts: the first half (eight 16-bit integers)
aids in selecting
the correct EM machine or interpreter.
Some EM machines, for instance, may have hardware floating point
instructions.
.N
The header entries are as follows (bit 0 is rightmost):
.IS 2
.VS 1 0
.PS 1 4 "" :
.PT
magic number (07255)
.PT
flag bits with the following meaning:
.PS - 7 "" :
.PT bit 0
TEST; test for integer overflow etc.
.PT bit 1
PROFILE; for each source line: count the number of memory
cycles executed.
.PT bit 2
FLOW; for each source line: set a bit in a bit map table if
instructions on that line are executed.
.PT bit 3
COUNT; for each source line: increment a counter if that line
is entered.
.PT bit 4
REALS; set if a program uses floating point instructions.
.PT bit 5
EXTRA; more tests during compiler debugging.
.PE
.PT
number of unresolved references.
.PT
version number; used to detect obsolete EM load files.
.PT
wordsize ; the number of bytes in each machine word.
.PT
pointer size ; the number of bytes available for addressing.
.PT
unused
.PT
unused
.PE
.IE
The second part of the header (eight entries, of pointer size bytes each)
describes the load file itself:
.IS 2
.PS 1 4 "" :
.PT
NTEXT; the program text size in bytes.
.PT
NDATA; the number of load-file descriptors (see below).
.PT
NPROC; the number of entries in the procedure descriptor table.
.PT
ENTRY; procedure number of the procedure to start with.
.PT
NLINE; the maximum source line number.
.PT
SZDATA; the address of the lowest uninitialized data byte.
.PT
unused
.PT
unused
.PE
.IE
.P
The program text consists of NTEXT bytes.
NTEXT is always a multiple of the wordsize.
The first byte of the program text is the
first byte of the instruction address
space, i.e. it has address 0.
Pointers into the program text are found in the procedure descriptor
table where relocation is simple and in the global data area.
The initialization of the global data area allows easy
relocation of pointers into both address spaces.
.P
The global data area is described by the NDATA descriptors.
Each descriptor describes a number of consecutive words (of~wordsize)
and consists of a sequence of bytes.
While reading the descriptors from the load file, one can
initialize the global data area from low to high addresses.
The size of the initialized data area is given by SZDATA,
this number can be used to check the initialization.
.N
The header of each descriptor consists of a byte, describing the type,
and a count.
The number of bytes used for this (unsigned) count depends on the
type of the descriptor and
is either a pointer-sized integer
or one byte.
The meaning of the count depends on the descriptor type.
At load time an interpreter can
perform any conversion deemed necessary, such as
reordering bytes in integers
and pointers and adding base addresses to pointers.
.BP
.A
In the following pictures we show a graphical notation of the
initializers.
The leftmost rectangle represents the leading byte.
.N 1
.DS
.PS - 4 " "
Fields marked with
.N 1
.PT n
contain a pointer-sized integer used as a count
.PT m
contain a one-byte integer used as a count
.PT b
contain a one-byte integer
.PT w
contain a wordsized integer
.PT p
contain a data or instruction pointer
.PT s
contain a null terminated ASCII string
.PE 1
.DE 0
.VS 1 1
.DS

    -------------------
    | 0 |      n      |           repeat last initialization n times
    -------------------
.DE
.DS
    ---------
    | 1 | m |                     m uninitialized words
    ---------
.DE
.DS
               ____________
              /    bytes   \e
    -----------------   -----
    | 2 | m | b | b |...| b |     m initialized bytes
    -----------------   -----
.DE
.DS
               _________
              /  word   \e
    -----------------------
    | 3 | m |      w      |...    m initialized wordsized integers
    -----------------------
.DE
.DS
               _________
              / pointer \e
    -----------------------
    | 4 | m |      p      |...    m initialized data pointers
    -----------------------
.DE
.DS
               _________
              / pointer \e
    -----------------------
    | 5 | m |      p      |...    m initialized instruction pointers
    -----------------------
.DE
.DS
               ____________
              /    bytes   \e
    -------------------------
    | 6 | m | b | b |...| b |     initialized integer of size m
    -------------------------
.DE
.DS
               ____________
              /    bytes   \e
    -------------------------
    | 7 | m | b | b |...| b |     initialized unsigned of size m
    -------------------------
.DE
.DS
               ____________
              /   string   \e
    -------------------------
    | 8 | m |        s      |     initialized float of size m
    -------------------------
.DE 3
.PS - 8
.PT type~0:
If the last initialization initialized k bytes starting
at address \fIa\fP, do the same initialization again n times,
starting at \fIa\fP+k, \fIa\fP+2*k, .... \fIa\fP+n*k.
This is the only descriptor whose starting byte
is followed by an integer with the
size of a
pointer,
in all other descriptors the first byte is followed by a one-byte count.
This descriptor must be preceded by a descriptor of
another type.
.PT type~1:
Reserve m words, not explicitly initialized (BSS and HOL).
.PT type~2:
The m bytes following the descriptor header are
initializers for the next m bytes of the
global data area.
m is divisible by the wordsize.
.PT type~3:
The m words following the header are initializers for the next m words of the
global data area.
.PT type~4:
The m data address space pointers following the header are
initializers for the next
m data pointers in the global data area.
Interpreters that represent EM pointers by
target machine addresses must relocate all data pointers.
.PT type~5:
The m instruction address space pointers following the header are
initializers for the next
m instruction pointers in the global data area.
Interpreters that represent EM instruction pointers by
target machine addresses must relocate these pointers.
.PT type~6:
The m bytes following the header form
a signed integer number with a size of m bytes,
which is an initializer for the next m bytes
of the global data area.
m is governed by the same restrictions as for
transfer of objects to/from memory.
.PT type~7:
The m bytes following the header form
an unsigned integer number with a size of m bytes,
which is an initializer for the next m bytes
of the global data area.
m is governed by the same restrictions as for
transfer of objects to/from memory.
.PT type~8:
The header is followed by an ASCII string, null terminated, to
initialize, in global data,
a floating point number with a size of m bytes.
m is governed by the same restrictions as for
transfer of objects to/from memory.
The ASCII string contains the notation of a real as used in the
Pascal language.
.PE
.P
The NPROC procedure descriptors on the load file consist of
an instruction space address (of~pointer~size) and
an integer (of~pointer~size) specifying the number of bytes for
locals.
