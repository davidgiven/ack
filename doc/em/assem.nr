.bp
.P1 "EM ASSEMBLY LANGUAGE"
.PP
We use two representations for assembly language programs,
one is in ASCII and the other is the compact assembly language.
The latter needs less space than the first for the same program
and therefore allows faster processing.
Our only program accepting ASCII assembly
language converts it to the compact form.
All other programs expect compact assembly input.
The first part of the chapter describes the ASCII assembly
language and its semantics.
The second part describes the syntax of the compact assembly
language.
The last part lists the EM instructions with the type of
arguments allowed and an indication of the function.
Appendix A gives a detailed description of the effect of all
instructions in the form of a Pascal program.
.P2 "ASCII assembly language"
.PP
An assembly language program consists of a series of lines, each
line may be blank, contain one (pseudo)instruction or contain one
label.
Input to the assembler is in lower case.
Upper case is used in this
document merely to distinguish keywords from the surrounding prose.
Comment is allowed at the end of each line and starts with a semicolon ";".
This kind of comment does not exist in the compact form.
.QQ
Labels must be placed all by themselves on a line and start in
column 1.
There are two kinds of labels, instruction and data labels.
Instruction labels are unsigned positive integers.
The scope of an instruction label is its procedure.
.QQ
The pseudoinstructions CON, ROM and BSS may be preceded by a
line containing a
1\-8 character data label, the first character of which is a
letter, period or underscore.
The period may only be followed by
digits, the others may be followed by letters, digits and underscores.
The use of the character "." followed by a constant,
which must be in the range 1 to 32767 (e.g. ".40") is recommended
for compiler
generated programs.
These labels are considered as a special case and handled
more efficiently in compact assembly language (see below).
Note that a data label on its own or two consecutive labels are not
allowed.
.PP
Each statement may contain an instruction mnemonic or pseudoinstruction.
These must begin in column 2 or later (not column 1) and must be followed
by a space, tab, semicolon or LF.
Everything on the line following a semicolon is
taken as a comment.
.PP
Each input file contains one module.
A module may contain many procedures,
which may be nested.
A procedure consists of
a PRO statement, a (possibly empty)
collection of instructions and pseudoinstructions and finally an END
statement.
Pseudoinstructions are also allowed between procedures.
They do not belong to a specific procedure.
.PP
All constants in EM are interpreted in the decimal base.
The ASCII assembly language accepts constant expressions
wherever constants are allowed.
The operators recognized are: +, \-, *, % and / with the usual
precedence order.
Use of the parentheses ( and ) to alter the precedence order is allowed.
.P3 "Instruction arguments"
.PP
Unlike many other assembly languages, the EM assembly
language requires all arguments of normal and pseudoinstructions
to be either a constant or an identifier, but not a combination
of these two.
There is one exception to this rule: when a data label is used
for initialization or as an instruction argument,
expressions of the form 'label+constant' and 'label-constant'
are allowed.
This makes it possible to address, for example, the
third word of a ten word BSS block
directly.
Thus LOE LABEL+4 is permitted and so is CON LABEL+3.
The resulting address is must be in the same fragment as the label.
It is not allowed to add or subtract from instruction labels or procedure
identifiers,
which certainly is not a severe restriction and greatly aids
optimization.
.PP
Instruction arguments can be constants,
data labels, data labels offsetted by a constant, instruction
labels and procedure identifiers.
The range of integers allowed depends on the instruction.
Most instructions allow only integers
(signed or unsigned)
that fit in a word.
Arguments used as offsets to pointers should fit in a
pointer-sized integer.
Finally, arguments to LDC should fit in a double-word integer.
.PP
Several instructions have two possible forms:
with an explicit argument and with an implicit argument on top of the stack.
The size of the implicit argument is the wordsize.
The implicit argument is always popped before all other operands.
For example: 'CMI 4' specifies that two four-byte signed
integers on top of the stack are to be compared.
\&'CMI' without an argument expects a wordsized integer
on top of the stack that specifies the size of the integers to
be compared.
Thus the following two sequences are equivalent:
.KS
.TS
center, tab(:) ;
l r 30 l r.
LDL:\-10:LDL:\-10
LDL:\-14:LDL:\-14
::LOC:4
CMI:4:CMI:
ZEQ:*1:ZEQ:*1
.TE
.KE
Section 11.1.6 shows the arguments allowed for each instruction.
.P3 "Pseudoinstruction arguments"
.PP
Pseudoinstruction arguments can be divided in two classes:
Initializers and others.
The following initializers are allowed: signed integer constants,
unsigned integer constants, floating-point constants, strings,
data labels, data labels offsetted by a constant, instruction
labels and procedure identifiers.
.PP
Constant initializers in BSS, HOL, CON and ROM pseudoinstructions
can be followed by a letter I, U or F.
This indicator
specifies the type of the initializer: Integer, Unsigned or Float.
If no indicator is present I is assumed.
The size of the initializer is the wordsize unless
the indicator is followed by an integer specifying the
initializer's size.
This integer is governed by the same restrictions as for
transfer of objects to/from memory.
As in instruction arguments, initializers include expressions of the form:
\&"LABEL+offset" and "LABEL\-offset".
The offset must be an unsigned decimal constant.
The 'IUF' indicators cannot be used in the offsets.
.PP
Data labels are referred to by their name.
.PP
Strings are surrounded by double quotes (").
Semicolon's in string do not indicate the start of comment.
In the ASCII representation the escape character \e (backslash)
alters the meaning of subsequent character(s).
This feature allows inclusion of zeroes, graphic characters and
the double quote in the string.
The following escape sequences exist:
.TS
center, tab(:);
l l l.
newline:NL\|(LF):\en
horizontal tab:HT:\et
backspace:BS:\eb
carriage return:CR:\er
form feed:FF:\ef
backslash:\e:\e\e
double quote:":\e"
bit pattern:\fBddd\fP:\e\fBddd\fP
.TE
The escape \fB\eddd\fP consists of the backslash followed by 1,
2, or 3 octal digits specifying the value of
the desired character.
If the character following a backslash is not one of those
specified,
the backslash is ignored.
Example: CON "hello\e012\e0".
Each string element initializes a single byte.
The ASCII character set is used to map characters onto values.
.PP
Instruction labels are referred to as *1, *2, etc.  in both branch
instructions and as initializers.
.PP
The notation $procname means the identifier for the procedure
with the specified name.
This identifier has the size of a pointer.
.P3 Notation
.PP
First, the notation used for the arguments, classes of
instructions and pseudoinstructions.
.DS
.TS
tab(:);
l l l.
<cst>:\&=:integer constant (current range \-2**31..2**31\-1)
<dlb>:\&=:data label
<arg>:\&=:<cst> or <dlb> or <dlb>+<cst> or <dlb>\-<cst>
<con>:\&=:integer constant, unsigned constant, floating-point constant
<str>:\&=:string constant (surrounded by double quotes),
<ilb>:\&=:instruction label
::'*' followed by an integer in the range 0..32767.
<pro>:\&=:procedure number ('$' followed by a procedure name)
<val>:\&=:<arg>, <con>, <pro> or <ilb>.
<par>:\&=:<val> or <str>
<...>*:\&=:zero or more of <...>
<...>+:\&=:one or more of <...>
[...]:\&=:optional ...
.TE
.DE
.P3 "Pseudoinstructions"
.P4 "Storage declaration"
.PP
Initialized global data is allocated by the pseudoinstruction CON,
which needs at least one argument.
Each argument is used to allocate and initialize a number of
consecutive bytes in data memory.
The number of bytes to be allocated and the alignment depend on the type
of the argument.
For each argument, an integral number of words,
determined by the argument type, is allocated and initialized.
.PP
The pseudoinstruction ROM is the same as CON,
except that it guarantees that the initialized words
will not change during the execution of the program.
This information allows optimizers to do
certain calculations such as array indexing and
subrange checking at compile time instead
of at run time.
.PP
The pseudoinstruction BSS allocates
uninitialized global data or large blocks of data initialized
by the same value.
The first argument to this pseudo is the number
of bytes required, which must be a multiple of the wordsize.
The other arguments specify the value used for initialization and
whether the initialization is only for convenience or a strict necessity.
The pseudoinstruction HOL is similar to BSS in that it requests an
(un)initialized global data block.
Addressing of a HOL block, however, is quasi absolute.
The first byte is addressed by 0,
the second byte by 1 etc. in assembly language.
The assembler/loader adds the base address of
the HOL block to these numbers to obtain the
absolute address in the machine language.
.PP
The scope of a HOL block starts at the HOL pseudo and
ends at the next HOL pseudo or at the end of a module
whatever comes first.
Each instruction falls in the scope of at most one
HOL block, the current HOL block.
It is not allowed to have more than one HOL block per procedure.
.PP
The alignment restrictions are enforced by the
pseudoinstructions.
All initializers are aligned on a multiple of their size or the wordsize
whichever is smaller.
Strings form an exception, they are to be seen as a sequence of initializers
each for one byte, i.e. strings are not padded with zero bytes.
Switching to another type of fragment or placing a label forces
word-alignment.
There are three types of fragments in global data space: CON, ROM and
BSS/HOL.
.IP "BSS <cst1>,<val>,<cst2>"
.br
Reserve <cst1> bytes.
<val> is the value used to initialize the area.
<cst1> must be a multiple of the size of <val>.
<cst2> is 0 if the initialization is not strictly necessary,
1 if it is.
.IP "HOL <cst1>,<val>,<cst2>"
.br
Idem, but all following absolute global data references will
refer to this block.
Only one HOL is allowed per procedure,
it has to be placed before the first instruction.
.IP "CON <val>+"
.br
Assemble global data words initialized with the <val> constants.
.IP "ROM <val>+"
.br
Idem, but the initialized data will never be changed by the program.
.P4 "Partitioning"
.PP
Two pseudoinstructions partition the input into procedures:
.IP "PRO <pro>[,<cst>]"
.br
Start of procedure.
<pro> is the procedure name.
<cst> is the number of bytes for locals.
The number of bytes for locals must be specified in the PRO or
END pseudoinstruction.
When specified in both, they must be identical.
.IP "END  [<cst>]"
.br
End of Procedure.
<cst> is the number of bytes for locals.
The number of bytes for locals must be specified in either the PRO or
END pseudoinstruction or both.
.P4 "Visibility"
.PP
Names of data and procedures in an EM module can either be
internal or external.
External names are known outside the module and are used to link
several pieces of a program.
Internal names are not known outside the modules they are used in.
Other modules will not 'see' an internal name.
.QQ
To reduce the number of passes needed,
it must be known at the first occurrence whether
a name is internal or external.
If the first occurrence of a name is in a definition,
the name is considered to be internal.
If the first occurrence of a name is a reference,
the name is considered to be external.
If the first occurrence is in one of the following pseudoinstructions,
the effect of the pseudo has precedence.
.IP "EXA <dlb>"
.br
External name.
<dlb> is known, possibly defined, outside this module.
Note that <dlb> may be defined in the same module.
.IP "EXP <pro>"
.br
External procedure identifier.
Note that <pro> may be defined in the same module.
.IP "INA <dlb>"
.br
Internal name.
<dlb> is internal to this module and must be defined in this module.
.IP "INP <pro>"
.br
Internal procedure.
<pro> is internal to this module and must be defined in this module.
.P4 "Miscellaneous"
.PP
Two other pseudoinstructions provide miscellaneous features:
.IP "EXC <cst1>,<cst2>"
.br
Two blocks of instructions preceding this one are
interchanged before being processed.
<cst1> gives the number of lines of the first block.
<cst2> gives the number of lines of the second one.
Blank and pure comment lines do not count.
This instruction is obsolete. Its use is strongly discouraged.
.IP "MES <cst>[,<par>]*"
.br
A special type of comment.
Used by compilers to communicate with the
optimizer, assembler, etc. as follows:
.RS
.IP "MES 0"
.br
An error has occurred, stop further processing.
.IP "MES 1"
.br
Suppress optimization.
.IP "MES 2,<cst1>,<cst2>"
.br
Use wordsize <cst1> and pointer size <cst2>.
.IP "MES 3,<cst1>,<cst2>,<cst3>,<cst4>"
.br
Indicates that a local variable is never referenced indirectly.
Used to indicate that a register may be used for a specific
variable.
<cst1> is offset in bytes from AB if positive
and offset from LB if negative.
<cst2> gives the size of the variable.
<cst3> indicates the class of the variable.
The following values are currently recognized:
.br
0\0\0\0The variable can be used for anything.
.br
1\0\0\0The variable is used as a loopindex.
.br
2\0\0\0The variable is used as a pointer.
.br
3\0\0\0The variable is used as a floating point number.
.br
<cst4> gives the priority of the variable,
higher numbers indicate better candidates.
.IP "MES 4,<cst>,<str>"
.br
Number of source lines in file <str> (for profiler).
.IP "MES 5"
.br
Floating point used.
.IP "MES 6,<val>*"
.br
Comment.  Used to provide comments in compact assembly language.
.IP "MES 7,....."
.br
Reserved.
.IP "MES 8,<pro>[,<dlb>]..."
.br
Library module. Indicates that the module may only be loaded
if it is useful, that is, if it can satisfy any unresolved
references during the loading process.
May not be preceded by any other pseudo, except MES's.
.IP "MES 9,<cst>"
.br
Guarantees that no more than <cst> bytes of parameters are
accessed, either directly or indirectly.
.IP "MES 10,<cst>[,<par>]*
.br
This message number is reserved for the global optimizer.
It inserts these messages in its output as hints to backends.
<cst> indicates the type of hint.
.IP "MES 11"
.br
Procedures containing this message are possible destinations of
non-local goto's with the GTO instruction.
Some backends keep locals in registers,
the locals in this procedure should not be kept in registers and
all registers containing locals of other procedures should be
saved upon entry to this procedure.
.RE
.IP ""
Each backend is free to skip irrelevant MES pseudos.
.P2 "The Compact Assembly Language"
.PP
The assembler accepts input in a highly encoded form.
This
form is intended to reduce the amount of file transport between the
front ends, optimizers
and back ends, and also reduces the amount of storage required for storing
libraries.
Libraries are stored as archived compact assembly language, not machine
language.
.PP
When beginning to read the input, the assembler is in neutral state, and
expects either a label or an instruction (including the pseudoinstructions).
The meaning of the next byte(s) when in neutral state is as follows, where
b1, b2
etc. represent the succeeding bytes.
.TS
tab(:);
rw17 4 l.
0:Reserved for future use
1\-129:Machine instructions, see Appendix A, alphabetical list
130\-149:Reserved for future use
150\-161:BSS,CON,END,EXA,EXC,EXP,HOL,INA,INP,MES,PRO,ROM
162\-179:Reserved for future pseudoinstructions
180\-239:Instruction labels 0 \- 59  (180 is local label 0 etc.)
240\-244:See the Common Table below
245\-255:Not used
.TE
After a label, the assembler is back in neutral state; it can immediately
accept another label or an instruction in the next byte.
No linefeeds are used to separate lines.
.PP
If an opcode expects no arguments,
the assembler is back in neutral state after
reading the one byte containing the instruction number.
If it has one or
more arguments (only pseudos have more than 1), the arguments follow directly,
encoded as follows:
.TS
tab(:);
r l.
0\-239:Offsets from \-120 to 119
240\-255:See the Common Table below
.TE
Absence of an optional argument is indicated by a special
byte.
.TS
tab(:);
c s s s
c c s c
l4 l l4 l.
Common Table for Neutral State and Arguments
class:bytes:description

<ilb>:240:b1:Instruction label b1  (Not used for branches)
<ilb>:241:b1 b2:16 bit instruction label  (256*b2 + b1)
<dlb>:242:b1:Global label .0\-.255, with b1 being the label
<dlb>:243:b1 b2:Global label .0\-.32767
:::with 256*b2+b1 being the label
<dlb>:244:<string>:Global symbol not of the form .nnn
<cst>:245:b1 b2:16 bit constant
<cst>:246:b1 b2 b3 b4:32 bit constant
<cst>:247:b1 .. b8:64 bit constant
<arg>:248:<dlb><cst>:Global label + (possibly negative) constant
<pro>:249:<string>:Procedure name  (not including $)
<str>:250:<string>:String used in CON or ROM (no quotes-no escapes)
<con>:251:<cst><string>:Integer constant, size <cst> bytes
<con>:252:<cst><string>:Unsigned constant, size <cst> bytes
<con>:253:<cst><string>:Floating constant, size <cst> bytes
:254::unused
<end>:255::Delimiter for argument lists or
:::indicates absence of optional argument
.TE 1
.PP
The bytes specifying the value of a 16, 32 or 64 bit constant
are presented in two's complement notation, with the least
significant byte first. For example: the value of a 32 bit
constant is ((s4*256+b3)*256+b2)*256+b1, where s4 is b4\-256 if
b4 is greater than 128 else s4 takes the value of b4.
A <string> consists of a <cst> immediately followed by
a sequence of bytes with length <cst>.
.PP
.ne 8
The pseudoinstructions fall into several categories, depending on their
arguments:
.DS
Group 1 \- EXC, BSS, HOL have a known number of arguments
Group 2 \- EXA, EXP, INA, INP have a string as argument
Group 3 \- CON, MES, ROM have a variable number of various things
Group 4 \- END, PRO have a trailing optional argument.
.DE
Groups 1 and 2
use the encoding described above.
Group 3 also uses the encoding listed above, with an <end> byte after the
last argument to indicate the end of the list.
Group 4 uses
an <end> byte if the trailing argument is not present.
.TS
tab(|);
l s l
l s s
l 2 lw(30) l.
Example  ASCII|Example compact
(LOC = 69, BRA = 18 here):

2||182
1||181
\0LOC|10|69 130
\0LOC|\-10|69 110
\0LOC|300|69 245 44 1
\0BRA|*19|18 139
300||241 44 1
.3||242 3
\0CON|4,9,*2,$foo|151 124 129 240 2 249 123 102 111 111 255
\0CON|.35|151 242 35 255
.TE
.P2 "Assembly language instruction list"
.PP
For each instruction in the list the range of argument values
in the assembly language is given.
The column headed \fIassem\fP contains the mnemonics defined
in 11.1.3.
The following column specifies restrictions of the argument
value.
Addresses have to obey the restrictions mentioned in chapter 2.
The classes of arguments
are indicated by letters:
.ds b \fBb\fP
.ds c \fBc\fP
.ds d \fBd\fP
.ds g \fBg\fP
.ds f \fBf\fP
.ds l \fBl\fP
.ds n \fBn\fP
.ds w \fBw\fP
.ds p \fBp\fP
.ds r \fBr\fP
.ds s \fBs\fP
.ds z \fBz\fP
.ds o \fBo\fP
.ds - \fB\-\fP
.sp
.TS
tab(:);
c s l l
l l 15 l l.
\fIassem\fP:constraints:rationale

\&\*c:cst:fits word:constant
\&\*d:cst:fits double word:constant
\&\*l:cst::local offset
\&\*g:arg:>= 0:global offset
\&\*f:cst::fragment offset
\&\*n:cst:>= 0:counter
\&\*s:cst:>0 , word multiple:object size
\&\*z:cst:>= 0 , zero or word multiple:object size
\&\*o:cst:> 0 , word multiple or fraction:object size
\&\*w:cst:> 0 , word multiple:object size *
\&\*p:pro::pro identifier
\&\*b:ilb:>= 0:label number
\&\*r:cst:0,1,2:register number
\&\*-:::no argument
.TE
.PP
The * at the rationale for \*w indicates that the argument
can either be given as argument or on top of the stack.
If the argument is omitted, the argument is fetched from the
stack;
it is assumed to be a wordsized unsigned integer.
Instructions that check for undefined integer or floating-point
values and underflow or overflow
are indicated below by (*).
.sp 1
.DS
.ta 12n
GROUP 1 \- LOAD

  LOC \*c :	Load constant (i.e. push one word onto the stack)
  LDC \*d :	Load double constant ( push two words )
  LOL \*l :	Load word at \*l-th local (\*l<0) or parameter (\*l>=0)
  LOE \*g :	Load external word \*g
  LIL \*l :	Load word pointed to by \*l-th local or parameter
  LOF \*f :	Load offsetted (top of stack + \*f yield address)
  LAL \*l :	Load address of local or parameter
  LAE \*g :	Load address of external
  LXL \*n :	Load lexical (address of LB \*n static levels back)
  LXA \*n :	Load lexical (address of AB \*n static levels back)
  LOI \*o :	Load indirect \*o bytes (address is popped from the stack)
  LOS \*w :	Load indirect, \*w-byte integer on top of stack gives object size
  LDL \*l :	Load double local or parameter (two consecutive words are stacked)
  LDE \*g :	Load double external (two consecutive externals are stacked)
  LDF \*f :	Load double offsetted (top of stack + \*f yield address)
  LPI \*p :	Load procedure identifier
.DE

.DS
GROUP 2 \- STORE

  STL \*l :	Store local or parameter
  STE \*g :	Store external
  SIL \*l :	Store into word pointed to by \*l-th local or parameter
  STF \*f :	Store offsetted
  STI \*o :	Store indirect \*o bytes (pop address, then data)
  STS \*w :	Store indirect, \*w-byte integer on top of stack gives object size
  SDL \*l :	Store double local or parameter
  SDE \*g :	Store double external
  SDF \*f :	Store double offsetted
.DE

.DS
GROUP 3 \- INTEGER ARITHMETIC

  ADI \*w :	Addition (*)
  SBI \*w :	Subtraction (*)
  MLI \*w :	Multiplication (*)
  DVI \*w :	Division (*)
  RMI \*w :	Remainder (*)
  NGI \*w :	Negate (two's complement) (*)
  SLI \*w :	Shift left (*)
  SRI \*w :	Shift right (*)
.DE

.DS
GROUP 4 \- UNSIGNED ARITHMETIC

  ADU \*w :	Addition
  SBU \*w :	Subtraction
  MLU \*w :	Multiplication
  DVU \*w :	Division
  RMU \*w :	Remainder
  SLU \*w :	Shift left
  SRU \*w :	Shift right
.DE

.DS
GROUP 5 \- FLOATING POINT ARITHMETIC

  ADF \*w :	Floating add (*)
  SBF \*w :	Floating subtract (*)
  MLF \*w :	Floating multiply (*)
  DVF \*w :	Floating divide (*)
  NGF \*w :	Floating negate (*)
  FIF \*w :	Floating multiply and split integer and fraction part (*)
  FEF \*w :	Split floating number in exponent and fraction part (*)
.DE

.DS
GROUP 6 \- POINTER ARITHMETIC

  ADP \*f :	Add \*f to pointer on top of stack
  ADS \*w :	Add \*w-byte value and pointer
  SBS \*w :	Subtract pointers in same fragment and push diff as size \*w integer
.DE

.DS
GROUP 7 \- INCREMENT/DECREMENT/ZERO

  INC \*- :	Increment word on top of stack by 1 (*)
  INL \*l :	Increment local or parameter (*)
  INE \*g :	Increment external (*)
  DEC \*- :	Decrement word on top of stack by 1 (*)
  DEL \*l :	Decrement local or parameter (*)
  DEE \*g :	Decrement external (*)
  ZRL \*l :	Zero local or parameter
  ZRE \*g :	Zero external
  ZRF \*w :	Load a floating zero of size \*w
  ZER \*w :	Load \*w zero bytes
.DE

.DS
GROUP 8 \- CONVERT    (stack:	source, source size, dest. size (top))

  CII \*- :	Convert integer to integer (*)
  CUI \*- :	Convert unsigned to integer (*)
  CFI \*- :	Convert floating to integer (*)
  CIF \*- :	Convert integer to floating (*)
  CUF \*- :	Convert unsigned to floating (*)
  CFF \*- :	Convert floating to floating (*)
  CIU \*- :	Convert integer to unsigned
  CUU \*- :	Convert unsigned to unsigned
  CFU \*- :	Convert floating to unsigned
.DE

.DS
GROUP 9 \- LOGICAL

  AND \*w :	Boolean and on two groups of \*w bytes
  IOR \*w :	Boolean inclusive or on two groups of \*w bytes
  XOR \*w :	Boolean exclusive or on two groups of \*w bytes
  COM \*w :	Complement (one's complement of top \*w bytes)
  ROL \*w :	Rotate left a group of \*w bytes
  ROR \*w :	Rotate right a group of \*w bytes
.DE

.DS
GROUP 10 \- SETS

  INN \*w :	Bit test on \*w byte set (bit number on top of stack)
  SET \*w :	Create singleton \*w byte set with bit n on (n is top of stack)
.DE

.DS
GROUP 11 \- ARRAY

  LAR \*w :	Load array element, descriptor contains integers of size \*w
  SAR \*w :	Store array element
  AAR \*w :	Load address of array element
.DE

.DS
GROUP 12 \- COMPARE

  CMI \*w :	Compare \*w byte integers, Push negative, zero, positive for <, = or >
  CMF \*w :	Compare \*w byte reals
  CMU \*w :	Compare \*w byte unsigneds
  CMS \*w :	Compare \*w byte values, can only be used for bit for bit equality test
  CMP \*- :	Compare pointers

  TLT \*- :	True if less, i.e. iff top of stack < 0
  TLE \*- :	True if less or equal, i.e. iff top of stack <= 0
  TEQ \*- :	True if equal, i.e. iff top of stack = 0
  TNE \*- :	True if not equal, i.e. iff top of stack non zero
  TGE \*- :	True if greater or equal, i.e. iff top of stack >= 0
  TGT \*- :	True if greater, i.e. iff top of stack > 0
.DE

.DS
GROUP 13 \- BRANCH

  BRA \*b :	Branch unconditionally to label \*b

  BLT \*b :	Branch less (pop 2 words, branch if top > second)
  BLE \*b :	Branch less or equal
  BEQ \*b :	Branch equal
  BNE \*b :	Branch not equal
  BGE \*b :	Branch greater or equal
  BGT \*b :	Branch greater

  ZLT \*b :	Branch less than zero (pop 1 word, branch negative)
  ZLE \*b :	Branch less or equal to zero
  ZEQ \*b :	Branch equal zero
  ZNE \*b :	Branch not zero
  ZGE \*b :	Branch greater or equal zero
  ZGT \*b :	Branch greater than zero
.DE

.DS
GROUP 14 \- PROCEDURE CALL

  CAI \*- :	Call procedure (procedure identifier on stack)
  CAL \*p :	Call procedure (with identifier \*p)
  LFR \*s :	Load function result
  RET \*z :	Return (function result consists of top \*z bytes)
.DE

.DS
GROUP 15 \- MISCELLANEOUS

  ASP \*f :	Adjust the stack pointer by \*f
  ASS \*w :	Adjust the stack pointer by \*w-byte integer
  BLM \*z :	Block move \*z bytes; first pop destination addr, then source addr
  BLS \*w :	Block move, size is in \*w-byte integer on top of stack
  CSA \*w :	Case jump; address of jump table at top of stack
  CSB \*w :	Table lookup jump; address of jump table at top of stack
  DCH \*- :	Follow dynamic chain, convert LB to LB of caller
  DUP \*s :	Duplicate top \*s bytes
  DUS \*w :	Duplicate top \*w bytes
  EXG \*w :	Exchange top \*w bytes
  FIL \*g :	File name (external 4 := \*g)
  GTO \*g :	Non-local goto, descriptor at \*g
  LIM \*- :	Load 16 bit ignore mask
  LIN \*n :	Line number (external 0 := \*n)
  LNI \*- :	Line number increment
  LOR \*r :	Load register (0=LB, 1=SP, 2=HP)
  LPB \*- :	Convert local base to argument base
  MON \*- :	Monitor call
  NOP \*- :	No operation
  RCK \*w :	Range check; trap on error
  RTT \*- :	Return from trap
  SIG \*- :	Trap errors to proc identifier on top of stack, \-2 resets default
  SIM \*- :	Store 16 bit ignore mask
  STR \*r :	Store register (0=LB, 1=SP, 2=HP)
  TRP \*- :	Cause trap to occur (Error number on stack)
.DE
