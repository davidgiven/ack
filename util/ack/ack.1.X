.\" $Header$
.de SB
.\" SuBheader
.sp 1
.PP
.nr Sf \\n(.f
.ft B
\\$1 \\$2 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9
.ft \\n(Sf
.sp 1
..
.TH ACK I
.ad
.SH NAME
ack \- Amsterdam Compiler Kit
.SH SYNOPSIS
\fBack\fP arguments
.br
\fBacc\fP arguments
.br
\fBapc\fP arguments
.br
\fBabc\fP arguments
.br
\fBint\fP arguments
.br
\fImachine\fP arguments
.SH DESCRIPTION
This program transforms sources in several
languages to load files for a variety of machines,
internally using several phases.
The transformation can be stopped at any phase.
Combining sources from several languages is allowed.
The run-time system of the first language mentioned,
either in the program call name or in the arguments,
is automatically included.
The libraries of all other languages mentioned,
containing most of the run-time systems,
are also automatically included.
Two types of load files can be distinguished,
\fIa.out\fP files containing machine code and \fIe.out\fP
files containing virtual EM machine code.
The last type is designed for interpretation; it is obtained by calling
the compiler as \fIint\fP.
Compilation time for interpretation is fast and gives many
runtime checks,
but execution is about seven times slower.
Which combinations of languages and machines are allowed varies
in time and depends on the installation.
.PP
The actions of \fIack\fP are to repeatedly transform files with a
particular suffix into files with another suffix,
finally combining the results into a single file.
.PP
Different machines can use different suffices,
but the following are recognized by most machines:
.IP .p
Pascal program.
.IP .c
C module.
.IP .b
Basic program.
.IP .e
EM assembly module in human readable form.
.IP .k
Compact EM assembly code.
.IP .m
Optimized compact EM assembly code.
.IP .s
Machine assembly language code.
.IP .o
Object file.
.PP
\fIAck\fP accepts the following flags:
.SB machine used
.IP \-m\fImachine\fP
This flag tells \fIack\fP to generate a load file for \fImachine\fP.
\fIMachine\fP can also be used as the program call
name, instead of \fIack\fP.
e.g. \fIack \-mi86 file.p\fP is equivalent to \fIi86
file.p\fP.
.SB output files
.IP \-o
Use the next argument as the name of the resulting file.
\fIAck\fP produces \fIa.out\fP or \fIe.out\fP by default.
This flag can always be used when \fIack\fP produces a single output file,
as in
.br
.ti +5
\fBack -c.s main.c -o new.s\fP.
.br
The output is produced on \fInew.s\fP instead of \fImain.s\fP.
.IP \-c\fI.suffix\fP
.IP \-c
\fIAck\fP tries to transform each source into a file with the \fIsuffix\fP.
When no \fIsuffix\fP is specified \fIack\fP stops just
before the phase where it combines all arguments into a load file,
thereby transforming the sources into \fI.k\fP, \fI.s\fP,
\&\fI.o\fP or \fI.m\fP files.
One extra \fIsuffix\fP is recognized here, \fI.i\fP,
this tells \fIack\fP to only preprocess all human readable sources,
producing files with \fIsuffix\fP \fI.i\fP.
.br
Note: \fIack\fP refuses to overwrite argument \fI.e\fP files.
.IP \-t
Preserve all intermediate files.
If two \fB\-t\fP are used,
\fIack\fP also preserves core dumps and output of failed transformations.
.SB messages
.IP \-w
Suppress all warning messages.
.IP \-E
Produce a complete listing of each Pascal source program.
Normally for each error, one message,
including the source line number, is given.
.IP \-e
List only the erroneous lines of each Pascal source program.
.IP \-v
Verbose.
Print information while juggling with files.
.SB preprocessing
.IP \-I\fIdir\fP
\&\`#include\' files whose names do not begin with \`/\' are
always sought first in the directory of the \fIfile\fP argument,
then in the directories named in \fB\-I\fP options,
then in directories on a standard list.
.IP \-D\fIname=def\fP
.IP \-D\fIname\fP
Define the \fIname\fP to the preprocessor,
as if by \`#define\'.
If no definition is given the \fIname\fP is defined as 1.
.IP \-U\fIname\fP
Remove any initial definition of \fIname\fP, before
preprocessing.
.SB debugging
.IP \-p
This flag tells both the Pascal and C front ends to include
code enabling the user to do some monitoring/debugging.
Each time a routine is entered the routine \fBprocentry\fP
is called and just before each return \fBprocexit\fP is called.
These routines are supplied with one parameter, a pointer
to a string containing the name of the routine.
.SB optimizing
.IP \-O
.IP \-Oopt1,opt2,...
Try to use the named optimizers, or, if no optimizers are given,
try using the peephole optimizer and a target optimizer.
.IP \-L
Disable the generation of code by the front ends to
record line number and source file name at run-time.
.SB libraries
.IP \-l\fIname\fP
Tells \fIack\fP to insert a library module at this point.
For example: the library \fImon\fP contains the
routines for systems calls needed by both C and Pascal.
.IP \-.\fIsuffix\fP
When linking multiple \fI.o\fP or \fI.m\fP files created by
separate calls of \fIack\fP together, \fIack\fP cannot deduce
the run-time system needed,
unless called as \fIapc\fP or \fIacc\fP.
This flag serves to tell \fIack\fP which runtime system is
needed in such a case.
For example: "ack \-c x.c ; ack \-.c x.o".
.IP \-r.\fIsuffix\fP
Most frontends and backends use one or
more run-time libraries.
These flags tell \fIack\fP to include the libraries needed when
a file with \fIsuffix\fP would be included in the arguments.
.IP \-LIB
This flag tells the peephole optimizer
.RF em_opt VI
to add information about the visibility of the names used
to each output module.
This is needed by
assembler/linkers when these modules are to be inserted
in libraries.
.SB interpreter
.IP \-{xxx}
The string starting after \`{\' and terminated by a \`}\' is passed
as an option string to the Pascal compiler and supersedes corresponding
options given in the source file.
See the ACK reference manual [4] for a list of options.
.IP "\-+xxx, \-\-xxx"
When you want to interpret your program, you may select some
options during interpretation, like test, profile, flow, extra and count.
A short description of these flags follows:
.RS
.IP "  t(est)" 12
test for undefined, overflow, array bound etc.
.IP "  f(low)"
keep track of executed source lines.
.IP "  c(ount)"
count the number of times a source line is executed.
.IP "  p(rofile)"
count the memory cycles executed per source line.
.RE
.IP "" 5
Test is on by default, the others are off. Normally, you give these
flag options each time you run the interpreter.
The EM assembler/linker gives you the opportunity to change
the defaults per program.
The changed options are recorded in the "e.out" header.
These flags \-\- and \-+ are passed to the assembler for this purpose.
So, \-\-t and \-+pfce invert the defaults.
.SB general
.IP \-R\fIprogram=xxx\fP
Replace the \fIprogram\fP by the pathname \fIxxx\fP.
The program names referred to later in this manual are allowed here.
.IP \-R\fIprogram\-xxx\fP
The flag argument \fI\-xxx\fP is given to \fIprogram\fP.
.IP \-R\fIprogram:n\fP
Set the priority of the indicated transformation to \fIn\fP.
The default priority is 0, setting it to -1 makes it highly
inlikely the the phase will be used, setting it to 1 makes
it very likely that the phase will be used.
.IP \-k
Do not stop when an error occurs, but try to transform all
other arguments as far as possible.
.IP \-g
Try to run the resulting load file.
No arguments can be passed this way,
so it is only useful in simple cases.
.PP
All arguments without a suffix or with an unrecognized suffix
are passed to the loaders, as for flags.
.SH PREPROCESSOR
All C source programs are run through the preprocessor
before they are fed to the compiler proper.
On larger machines, the compiler has a built in preprocessor.
Other human readable sources (Pascal or Basic programs and
machine assembly) are only preprocessed when they start with a \`#\'.
.PP
\fIAck\fP adds a few macro definitions when it calls the
preprocessor.
These macro\'s contain the word- and pointer-size and the sizes
of some basic types used by the Pascal, Basic and/or C compiler.
All sizes are in bytes.
.sp 1
.TS
tab(:);
l l l l.
EM_WSIZE:wordsize:EM_PSIZE:pointer size
EM_SSIZE:size of shorts (C):EM_LSIZE:size of longs (C+Pascal)
EM_FSIZE:size of floats (C):EM_DSIZE:size of doubles (C+Pascal)
.TE
.sp 1
The name of the \fImachine\fP or something like it when
the machine name is numeric is also defined (as 1).
.PP
The default directories searched for include files differ for each machine.
.SH PROGRAMS
\fIAck\fP uses one or more programs in each phase of the
transformation.
The table below gives the names \fIack\fP uses for these
programs.
Internally \fIack\fP maintains a mapping of these names to pathnames
for load files.
The table specifies which type of files are accepted by each
program as input and the file type produced as output.
.sp 1
.in +2
.TS
tab(:);
l l l l.
input:name:output:description
\&.c:cem:.k:C front end [4,5,6]
\&.p:pc:.k:Pascal front end [2,3,6]
\&.b:abc:.k:Basic front end [6,8]
\&.e:encode:.k:Compactify EM assembly language [1]
\&.k:opt:.m:EM peephole optimizer
\&.k .m:decode:.e:Produce human readable EM assembly
\&.k .m:emass:e.out:Linker producing EM machine code [1]
\&.m:be:.s:backend
\&.s:asld:.out:Assembler/linker producing machine code in Ack a.out format
\&.s:as:.o:Assembler
\&.o:ld:a.out:Linker producing machine code
\&.o:led:.out:Linker producing machine code in Ack a.out format
.TE
.in -2
.SH "SEE ALSO"
.PD 0
em_opt(6), em_ass(6), em_cg(6), ack.out(5)
.IP [1]
A.S. Tanenbaum, Hans van Staveren, Ed Keizer and Johan
Stevenson \fIDescription of a machine architecture for use with
block structured languages\fP Informatica report IR-81.
.IP [2]
K. Jensen and N. Wirth
\fIPASCAL, User manual and report\fP Springer Verlag.
.IP [3]
The ISO Pascal standard proposal ISO/TC97/SC5-N462.
.IP [4]
B.W. Kernighan and D.M. Ritchie, \fIThe C Programming
language\fP, Prentice-Hall, 1978
.IP [5]
D.M. Ritchie, \fI C Reference Manual\fP
.IP [6]
Amsterdam Compiler Kit, reference manuals and UNIX manual pages.
.IP [7]
E.G. Keizer, Ack description file reference manual.
.IP [8]
M.L. Kersten, \fIThe ABC compiler\fP.
.PD
.SH DIAGNOSTICS
.PD
The diagnostics are intended to be self\-explanatory.
.SH BUGS
The -g flag is inoperative.
.br
Not all warning messages are superseded by \fB\-w\fP.
.br
Argument assembly files are not preprocessed when fed into the
universal assembler.
.SH AUTHOR
Ed Keizer, Vrije Universiteit, Amsterdam
