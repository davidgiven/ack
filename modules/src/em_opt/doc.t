.TL
A Tour of the Peephole Optimizer Library
.AU
B. J. McKenzie
.NH
Introduction
.LP
The peephole optimizer consists of three major parts:
.IP a)
the table describing the optimization to be performed
.IP b)
a program to parse these tables and build input and output routines to
interface to the library and a dfa based routine to recognize patterns and
make the requested replacements.
.IP c)
common routines for the library that are independent of the table of a)
.LP
The library conforms to the
.I EM_CODE(3)
module interface with entry points with names like
.I C_xxx.
The library module results in calls to a module with an identical interface
but with calls to routines with names of the form
.I O_xxx.

.LP
We shall now describe each of these in turn in some detail.

.NH
The optimization table
.LP
The file
.I patterns
contains the patterns of EM instructions  to be recognized by the optimizer
and the EM instructions to replace them. Each pattern may have an
optional restriction that must be satisfied before the replacement is made.
The syntax of the table will be described using extended BNF notation
used by
.I LLGen
where:
.DS
.I
	[...]	- are used to group items
	|	- is used to separate alternatives
	;	- terminates a rule
	?	- indicates item is optional
	*	- indicates item is repeated zero or more times
	+	- indicates item is repeated one or more times
.R
.DE
The format of each rule in the table is:
.DS
.I
	rule	: pattern global_restriction? ':' replacement
		;
.R
.DE
Each rule must be on a single line except that it may be broken after the
colon if the next line begins with a tab character.
The pattern has the syntax:
.DS
.I
	pattern	: [ EM_mnem [ local_restriction ]? ]+
		;
	EM-mnem : "An EM instruction mnemonic"
		| 'lab'
		;
.R
.DE
and consists of a sequence of one or more EM instructions or
.I lab
which stands for a defined instruction label. Each EM-mnem may optionally be
followed by a local restriction on the argument of the mnemonic and take
one of the following forms depending on the type of the EM instruction it
follows:
.DS
.I
	local_restriction	: normal_restriction
				| opt_arg_restriction
				| ext_arg_restriction
				;
.R
.DE
A normal restriction is used after all types of EM instruction except for
those that allow an optional argument, (such as
.I adi
) or those involving external names, (such as
.I lae
)
and takes the form:
.DS
.I
	normal_restriction	: [ rel_op ]? expression
				;
	rel_op	: '=='
		| '!='
		| '<='
		| '<'
		| '>='
		| '>'
		;
.R
.DE
If the rel_op is missing, the equality
.I ==
operator is assumed. The general form of expression is defined later but
basically it involves simple constants, references to EM_mnem arguments
that appear earlier in the pattern and expressions similar to those used
in C expressions.

The form of the restriction after those EM instructions like
.I adi
whose arguments are optional takes the form:
.DS
.I
	opt_arg_restriction	: normal_restriction
				| 'defined'
				| 'undefined'
				;
.R
.DE
The
.I defined
and
.I undefined
indicate that the argument is present
or absent respectively. The normal restriction form implies that the
argument is present and satisfies the restriction.

The form of the restriction after those EM instructions like
.I lae
whose arguments refer to external object take the form:
.DS
.I
	ext_arg_restriction	: patarg  offset_part?
				;
	offset_part		: [ '+' | '-' ] expression
				;
.R
.DE
Such an argument has one of three forms: a offset with no name, an
offset form a name or an offset from a label. With no offset part
the restriction requires the argument to be identical to a previous
external argument. With an offset part it requires an identical name
part, (either empty, same name or same label) and supplies a relationship
among the offset parts. It is possible to refer to test for the same
external argument, the same name or to obtain the offset part of an external
argument using the
.I sameext
,
.I samenam
and
.I offset
functions given below.
.LP
The general form of an expression is:
.DS
.I
	expression	: expression binop expression
			| unaryop expression
			| '(' expression ')'
			| bin_function '(' expression ',' expression ')'
			| ext_function '(' patarg ',' patarg ')'
			| 'offset' '(' patarg ')'
			| patarg
			| 'p'
			| 'w'
			| INTEGER
			;
.R
.DE
.DS
.I
	bin_function	: 'sfit'
			| 'ufit'
			| 'samesign'
			| 'rotate'
			;
.R
.DE
.DS
.I
	ext_function	: 'samenam'
			| 'sameext'
			;
	patarg		: '$' INTEGER
			;
	binop		: "As for C language"
	unaryop		: "As for C language"
.R
.DE
The INTEGER in the
.I patarg
refers to the first, second, etc. argument in the pattern and it is
required to refer to a pattern that appears earlier in the pattern
The
.I w
and
.I p
refer to the word size and pointer size (in bytes) respectively. The
various function test for:
.IP sfit 10
the first argument fits as a signed value of
the number of bit specified by the second argument.
.IP ufit 10
as for sfit but for unsigned values.
.IP samesign 10
the first argument has the same sign as the second.
.IP rotate 10
the value of the first argument rotated by the number of bit specified
by the second argument.
.IP samenam 10
both arguments refer to externals and have either no name, the same name
or same label.
.IP sameext 10
both arguments refer to the same external.
.IP offset 10
the argument is an external and this yields it offset part.

.LP
The global restriction takes the form:
.DS
.I
	global_restriction	: '?' expression
				;
.R
.DE
and is used to express restrictions that cannot be expressed as simple
restrictions on a single argument or are can be expressed in a more
readable fashion as a global restriction. An example of such a rule is:
.DS
.I
	dup w ldl stf  ? p==2*w : ldl $2 stf $3 ldl $2 lof $3
.R
.DE
which says that this rule only applies if the pointer size is twice the
word size.

.NH
Incompatibilities with Previous Optimizer
.LP
The current table format is not compatible with previous versions of the
peephole optimizer tables. In particular the previous table had no provision
for local restrictions and only the equivalent of the global restriction.
This meant that our
.I '?'
character that announces the presence of the optional global restriction was
not required. The previous optimizer performed a number of other tasks that
were unrelated to optimization that were possible because the old optimizer
read the EM code for a complete procedure at a time. This included task such
as register variable reference counting and moving the information regarding
the number of bytes of local storage required by a procedure from it
.I end
pseudo instruction to it's
.I pro
pseudo instruction. These tasks are no longer done. If there are required
then the must be performed by some other program in the pipeline.

.NH
The Parser
.LP
The program to parse the tables and build the pattern table dependent dfa
routines is built from the files:
.IP parser.h 15
header file
.IP parser.g 15
LLGen source file defining syntax of table
.IP syntax.l 15
Lex sources file defining form of tokens in table.
.IP initlex.c 15
Uses the data in the library
.I em_data.a
to initialize the lexical analyser to recognize EM instruction mnemonics.
.IP outputdfa.c 15
Routines to output dfa when it has been constructed.
.IP outcalls.c 15
Routines to output the file
.I incalls.c
defined in section 4.
.IP findworst.c 15
Routines to analyze patterns to find how to continue matching after a
successful replacement or failed match.

.LP
The parser checks that the tables conform to the syntax outlined in the
previous section and also mades a number of semantic checks on their
validity. Further versions could make further checks such as looking for
cycles in the rules or checking that each replacement leaves the same
number of bytes on the stack as the pattern it replaces. The parser
builds an internal dfa representation of the rules by combining rules with
common prefixes. All local and global restrictions are combined into a single
test to be performed are a complete pattern has been detected in the input.
The idea is to build a structure so that each of the patterns can be matched
and then the corresponding tests made and the first that succeeds is replaced.
If two rules have the same pattern and both their tests also succeed the one
that appears first in the tables file will be done. Somewhat less obvious
is that id one pattern is a proper prefix of a longer pattern and its test
succeeds then the second pattern will not be checked for.

A major task of the parser if to decide on the action to take when a rule has
been partially matched or when a pattern has been completely matched but its
test does not succeed. This requires a search of all patterns to see if any
part of the part matched could be part of some other pattern. for example
given the two patterns:
.DS
.I
	loc adi w loc adi w : loc $1+$3 adi w
	loc adi w loc sbi w : loc $1-$3 adi w
.R
.DE
If the first pattern fails after seeing the input:
.DS
.I
	loc adi loc
.R
.DE
the parser will still need to check whether the second pattern matches.
This requires a decision on how to fix up any internal data structures in
the dfa matcher, such as moving some instructions from the pattern to the
output queue and moving the pattern along and then deciding what state
it should continue from. Similar decisions  are requires after a pattern
has been replaced. For example if the replacement is empty it is necessary
to backup
.I n-1
instructions where
.I n
is the length of the longest pattern in the tables.

.NH
Structure of the Resulting Library

.LP
The major data structures maintained by the library consist of three queues;
an
.I output
queue of instructions awaiting output, a
.I pattern
queue containing instructions that match the current prefix, and a
.I backup
queue of instructions that have been backed up over and need to be reparsed
for further pattern matches.

.LP
If no errors are detected by the parser in the tables it output the following
files:
.IP dfa.c 10
this consists of a large switch statement that maintains the current state of
the dfa and makes a transition to the next state if the next input instruction
matches.
.IP incalls.r 10
this contains an entry for every EM instruction (plus
.I lab
) giving information on how to build a routine with the name
.I C_xxx
that conforms to the
.I EM_CODE(3)
modules interface. If the EM instruction does not appear in the tables
patterns at all then the dfa routine is called to flush any current queued
output and the the output
.I O_xxx
routine is called. If the EM instruction does appear in a pattern then the instruction is added onto the end of the pattern queue and the dfa routines called
to attempted to make a transition. This file is input to the
.I awk
program
.I makefuns.awk
to produce individual C files with names like
.I C_xxx.c
each containing a single function definition. This enables the loader to
only load those routines that are actually needed when the library is loaded.
.IP trans.c 10
this contains a routine that is called after each transition to a state that
contains restrictions and replacements. The restrictions a converted to
C expressions and the replacements coded as calls to output instructions
into the output queue.

.LP
The following files contain code that is independent of the pattern tables:
.IP nopt.c 10
general routines to initialize, and maintain the data structures.
.IP aux.c 10
routines to implement the functions used in the rules.
.IP mkcalls.c 10
code to convert the internal data structures to calls on the output
.I O_xxx
routines when the output queue is flushed.

.NH
Miscellaneous Issues
.LP
The size of the output and backup queues are fixed in size according to the
values of
.I MAXOUTPUT
and
.I MAXBACKUP
defined in the file
.I nopt.h.
The size of the pattern queue is set to the length of the maximum pattern
length by the tables output by the parser. The queues are implemented as
arrays of pointers to structures containing the instruction and its arguments.
The space for the structures are initially obtained by calls to
.I Malloc
(from the
.I alloc(3)
module),
and freed when the output queue or patterns queue is cleared. These freed
structures are collected on a free list and reused to avoid the overheads
of repeated calls to
.I malloc
and
.I free.

.LP
The fixed size of the output and pattern queues causes no difficulty in
practice and can only result in some potential optimizations being missed.
When the output queue fills it is simply prematurely flushed and backups
when the backup queue is fill are simply ignored. A possible improvement
would be to flush only part of the output queue when it fills. It should
be noted that it is not possible to statically determine the maximum possible
size for these queues as they need to be unbounded in the worst case. A
study of the rule
.DS
.I
	inc dec :
.R
.DE
with the input consisting of
.I N
.I inc
and then
.I N
.I dec
instructions requires an output queue length of
.I N-1
to find all possible replacements.
