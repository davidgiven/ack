.SN 3
.BP
.S1 "INSTRUCTION ADDRESS SPACE"
The instruction space of the EM machine contains
the code for procedures.
Tables necessary for the execution of this code, for example, procedure
descriptor tables, may also be present.
The instruction space does not change during
the execution of a program, so that it may be
protected.
No further restrictions to the instruction address space are
necessary for the abstract and assembly language level.
.P
Each procedure has a single entry point: the first instruction.
A special type of pointer identifies a procedure.
Pointers into the instruction
address space have the same size as pointers into data space and
can, for example, contain the address of the first instruction
or an index in a procedure descriptor table.
.A
There is a single EM program counter, PC, pointing
to the next instruction to be executed.
The procedure pointed to by PC is
called the 'current' procedure.
A procedure may call another procedure using the CAL or CAI
instruction.
The calling procedure remains 'active' and is resumed whenever the called
procedure returns.
Note that a procedure has several 'active' invocations when
called recursively.
.P
Each procedure must return properly.
It is not allowed to fall through to the
code of the next procedure.
There are several ways to exit from a procedure:
.IS 3
.PS
.PT
the RET instruction, which returns to the
calling procedure.
.PT
the RTT instruction, which exits a trap handling routine and resumes
the trapping instruction (see next chapter).
.PT
the GTO instruction, which is used for non-local goto's.
It can remove several frames from the stack and transfer
control to an active procedure.
(see also MES~11 in paragraph 11.1.4.4)
.PE
.IE
.P
All branch instructions can transfer control
to any label within the same procedure.
Branch instructions can never jump out of a procedure.
.P
Several language implementations use a so called procedure
instance identifier, a combination of a procedure identifier and
the LB of a stack frame, also called static link.
.P
The program text for each procedure, as well as any tables,
are fragments and can be allocated anywhere
in the instruction address space.
