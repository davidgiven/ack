.bp
.P1 "TYPE REPRESENTATIONS"
.PP
The representations used for typed objects are not precisely
specified by EM.
Sometimes we only specify that a typed object occupies a
certain amount of space and state no further restrictions.
If one wants to have a different representation of the value of
an object on the stack one has to use a convert instruction
in most cases.
We do specify some relations between the representations of
types.
This allows some intermixed use of operators for different types
on the same object(s).
For example, the instruction ZER pushes signed and
unsigned integers with the value zero and empty sets.
ZER has as only argument the size of the object.
.QQ
The representation of floating point numbers is a good example,
it allows widely varying implementations.
The only ways to create floating point numbers are via
initialization and via conversions from integer numbers.
Only by using conversions to integers and comparing
two floating point numbers with each other, can these numbers
be converted to human readable output.
Implementations may use base 10, base 2 or any other
base for exponents, and have freedom in choosing the range of
exponent and mantissa.
.QQ
Other types are more precisely described.
In the following paragraphs a description will be given of the
restrictions imposed on the representation of the types used.
A number \fBn\fP used in these paragraphs indicates the size of
the object in \fIbits\fP.
.P2 "Unsigned integers"
.PP
The range of unsigned integers is 0..
.Ex 2 "\fBn\fP" -1.
A binary representation is assumed.
The order of the bits within an object is knowingly left
unspecified.
Discussing bit order within each 8-bit byte is academic,
so the only real freedom of this specification lies in the byte
order.
We really do not care whether an implementation of a 4-byte
integer has its bytes in a particular order of significance.
This of course means that some sequences of instructions have
unpredictable effects.
For example:
.DS
LOC 258 ; STL 0 ; LAL 0 ; LOI 1      ( wordsize >=2 )
.DE
The value on the stack after executing this sequence
can be anything,
but will most likely be 1 or 2.
.QQ
Conversion between unsigned integers of different sizes have to
be done with explicit convert instructions.
One cannot simply pad an unsigned integer with zero's at either end
and expect a correct result.
.QQ
We assume existence of at least single word unsigned arithmetic
in any implementation.
.P2 "Signed Integers"
.PP
The range of signed integers is
.Ex \-2 "\fBn\fP\-1" ~..
.Ex 2 "\fBn\fP\-1" \-1,
in other words the range of signed integers of \fBn\fP bits
using two's complement arithmetic.
The representation is the same as for unsigned integers except the range
.Ex 2 "\fBn\fP\-1" ~..
.Ex 2 "\fBn\fP" \-1
is mapped on the
range 
.Ex \-2 "\fBn\fP\-1" ~..~\-1.
In other words, the most significant bit is used as sign bit.
The convert instructions between signed and unsigned integers
of the same size can be used to catch errors.
.QQ
The value 
.Ex \-2 "\fBn\fP\-1" 
is used for undefined
signed integers.
EM implementations should trap when this value is used in an
operation on signed integers.
The instruction mask, accessed with SIM and LIM \-~see chapter 9~\-,
can be used to disable such traps.
.QQ
We assume existence of at least single word signed arithmetic
in any implementation.
.P2 "Floating point values"
.PP
Floating point values must have a signed mantissa and a signed
exponent.
Although no base is specified, base 2 is the normal choice,
because the FEF instruction pushes the exponent in base 2.
.QQ
The implementation of floating point arithmetic is optional.
The compilers currently in use have runtime parameters for the
size of the floating point values they should use.
Common choices are 4 and/or 8 bytes.
.P2 Pointers
.PP
EM has two kinds of pointers: for instruction and for data
space.
Each kind can only be used for its own space, conversion between
these two subtypes is impossible.
We assume that pointers have a range from 0 upwards.
Any implementation may have holes in the pointer range between
fragments.
One can of course not expect to be able to address two megabyte
of memory using a 2-byte pointer.
Normally, a 2-byte pointer allows up to 65536 bytes of
addressable memory.
.QQ
Pointer representation has one restriction.
The pointer with the same representation as the integer zero of
the same size should be invalid.
Some languages and/or runtime systems represent the nil
pointer as zero.
.P2 "Bit sets"
.PP
All bit sets of size \fBn\fP are subsets of the set
{~i~|~i>=0,~i<\fBn\fP~}.
A bit set contains a bit for each element showing its
presence or absence.
Bit sets are subdivided into words.
The word with the lowest EM address governs the subset
{~i~|~i>=0,~i<\fBm\fP~}, where \fBm\fP is the number of bits in
a word.
The next higher words each govern the next higher \fBm\fP set elements.
The relation between a set with size of
a word and an unsigned integer word is that
the value of the unsigned integer is the summation of the
2\v'-0.5m'i\v'0.5m' where i is in the set.
.QQ
Example: a 2-word bit set (wordsize 2) containing the
elements 1, 6, 8, 15, 18, 21, 27 and 28 is composed of two
integers, e.g. at addresses 40 and 42.
The word at 40 contains the value 33090 (or~\-32446),
the word at 42 contains the value 6180.
