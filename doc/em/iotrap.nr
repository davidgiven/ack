.SN 8
.VS 1 0
.BP
.S1 "ENVIRONMENT INTERACTIONS"
EM programs can interact with their environment in three ways.
Two, starting/stopping and monitor calls, are dealt with in this chapter.
The remaining way to interact, interrupts, will be treated
together with traps in chapter 9.
.S2 "Program starting and stopping"
EM user programs start with a call to a procedure called
m_a_i_n.
The assembler and backends look for the definition of a procedure
with this name in their input.
The call passes three parameters to the procedure.
The parameters are similar to the parameters supplied by the
UNIX
.FS
UNIX is a Trademark of Bell Laboratories.
.FE
operating system to C programs.
These parameters are often called
.BW argc ,
.B argv
and
.BW envp .
Argc is the parameter nearest to LB and is a wordsized integer.
The other two are pointers to the first element of an array of
string pointers.
.N
The
.B argv
array contains
.B argc
strings, the first of which contains the program call name.
The other strings in the
.B argv
array are the program parameters.
.P
The
.B envp
array contains strings in the form "name=string", where 'name'
is the name of an environment variable and string its value.
The
.B envp
is terminated by a zero pointer.
.P
An EM user program stops if the program returns from the first
invocation of m_a_i_n.
The contents of the function return area are used to procure a
wordsized program return code.
EM programs also stop when traps and interrupts occur that are
not caught and when the exit monitor call is executed.
.S2 "Input/Output and other monitor calls"
EM differs from most conventional machines in that it has high level i/o
instructions.
Typical instructions are OPEN FILE and READ FROM FILE instead
of low level instructions such as setting and clearing
bits in device registers.
By providing such high level i/o primitives, the task of implementing
EM on various non EM machines is made considerably easier.
.P
I/O is initiated by the MON instruction, which expects an iocode on top
of the stack.
Often there are also parameters which are pushed on the
stack in reverse order, that is: last
parameter first.
Some i/o functions also provide results, which are returned on the stack.
In the list of monitor calls we use several types of parameters and results,
these types consist of integers and unsigneds of varying sizes, but never
smaller than the wordsize, and the two pointer types.
.N 1
The names of the types used are:
.IS 4
.PS - 10
.PT int
an integer of wordsize
.PT int2
an integer whose size is the maximum of the wordsize and 2
bytes
.PT int4
an integer whose size is the maximum of the wordsize and 4
bytes
.PT intp
an integer with the size of a pointer
.PT uns2
an unsigned integer whose size is the maximum of the wordsize and 2
.PT unsp
an unsigned integer with the size of a pointer
.PT ptr
a pointer into data space
.PE 1
.IE 0
The table below lists the i/o codes with their results and
parameters.
This list is similar to the system calls of the UNIX Version 7
operating system.
.BP
.A
To execute a monitor call, proceed as follows:
.IS 2
.N 1
.PS a 4 "" )
.PT
Stack the parameters, in reverse order, last parameter first.
.PT
Push the monitor call number (iocode) onto the stack.
.PT
Execute the MON instruction.
.PE 1
.IE
An error code is present on the top of the stack after
execution of most monitor calls.
If this error code is zero, the call performed the action
requested and the results are available on top of the stack.
Non-zero error codes indicate a failure, in this case no
results are available and the error code has been pushed twice.
This construction enables programs to test for failure with a
single instruction (~TEQ or TNE~) and still find out the cause of
the failure.
The result name 'e' is reserved for the error code.
.N 1
List of monitor calls.
.DS B
number name    parameters      results           function

   1   Exit    status:int                        Terminate this process
   2   Fork                    e,flag,pid:int    Spawn new process
   3   Read    fildes:int;buf:ptr;nbytes:unsp
                               e:int;rbytes:unsp Read from file
   4   Write   fildes:int;buf:ptr;nbytes:unsp
                               e:int;wbytes:unsp Write on a file
   5   Open    string:ptr;flag:int
                               e,fildes:int      Open file for read and/or write
   6   Close   fildes:int      e:int             Close a file
   7   Wait                    e:int;status,pid:int2
                                                 Wait for child
   8   Creat   string:ptr;mode:int
                               e,fildes:int      Create a new file
   9   Link    string1,string2:ptr
                               e:int             Link to a file
  10   Unlink  string:ptr      e:int             Remove directory entry
  12   Chdir   string:ptr      e:int             Change default directory
  14   Mknod   string:ptr;mode,addr:int2
                               e:int             Make a special file
  15   Chmod   string:ptr;mode:int2
                               e:int             Change mode of file
  16   Chown   string:ptr;owner,group:int2
                               e:int             Change owner/group of a file
  18   Stat    string,statbuf:ptr
                               e:int             Get file status
  19   Lseek   fildes:int;off:int4;whence:int
                               e:int;oldoff:int4 Move read/write pointer
  20   Getpid                  pid:int2          Get process identification
  21   Mount   special,string:ptr;rwflag:int
                               e:int             Mount file system
  22   Umount  special:ptr     e:int             Unmount file system
  23   Setuid  userid:int2     e:int             Set user ID
  24   Getuid                  e_uid,r_uid:int2  Get user ID
  25   Stime   time:int4       e:int             Set time and date
  26   Ptrace  request:int;pid:int2;addr:ptr;data:int
                               e,value:int       Process trace
  27   Alarm   seconds:uns2    previous:uns2     Schedule signal
  28   Fstat   fildes:int;statbuf:ptr
                               e:int             Get file status
  29   Pause                                     Stop until signal
  30   Utime   string,timep:ptr
                               e:int             Set file times
  33   Access  string,mode:int e:int             Determine file accessibility
  34   Nice    incr:int                          Set program priority
  35   Ftime   bufp:ptr        e:int             Get date and time
  36   Sync                                      Update filesystem
  37   Kill    pid:int2;sig:int
                               e:int             Send signal to a process
  41   Dup     fildes,newfildes:int
                               e,fildes:int      Duplicate a file descriptor
  42   Pipe                    e,w_des,r_des:int Create a pipe
  43   Times   buffer:ptr                        Get process times
  44   Profil  buff:ptr;bufsiz,offset,scale:intp Execution time profile
  46   Setgid  gid:int2        e:int             Set group ID
  47   Getgid                  e_gid,r_gid:int   Get group ID
  48   Sigtrp  trapno,signo:int
                               e,prevtrap:int    See below
  51   Acct    file:ptr        e:int             Turn accounting on or off
  53   Lock    flag:int        e:int             Lock a process
  54   Ioctl   fildes,request:int;argp:ptr
                               e:int             Control device
  56   Mpxcall cmd:int;vec:ptr e:int             Multiplexed file handling
  59   Exece   name,argv,envp:ptr
                               e:int             Execute a file
  60   Umask   complmode:int2  oldmask:int2      Set file creation mode mask
  61   Chroot  string:ptr      e:int             Change root directory
.DE 1
Codes 0, 11, 13, 17, 31, 32, 38, 39, 40, 45, 49, 50, 52,
55, 57, 58, 62, and 63 are
not used.
.P
All monitor calls, except fork and sigtrp
are the same as the UNIX version 7 system calls.
.P
The sigtrp entry maps UNIX signals onto EM interrupts.
Normally, trapno is in the range 0 to 252.
In that case it requests that signal signo
will cause trap trapno to occur.
When given trap number -2, default signal handling is reset, and when given
trap number -3, the signal is ignored.
.P
The flag returned by fork is 1 in the child process and 0 in
the parent.
The pid returned is the process-id of the other process.
.BP
.S1 "TRAPS AND INTERRUPTS"
EM provides a means for the user program to catch all traps
generated by the program itself, the hardware, or external conditions.
This mechanism uses five instructions: LIM, SIM, SIG, TRP and RTT.
This section of the manual may be omitted on the first reading since it
presupposes knowledge of the EM instruction set.
.P
The action taken when a trap occures is determined by the value
of an internal EM trap register.
This register contains a pointer to a procedure.
Initially the pointer used is zero and all traps halt the
program with, hopefully, a useful message to the outside world.
The SIG instruction can be used to alter the trap register,
it pops a procedure pointer from the
stack into the trap register.
When a trap occurs after storing a nonzero value in the trap
register, the procedure pointed to by the trap register
is called with the trap number
as the only parameter (see below).
SIG returns the previous value of the trap register on the
stack.
Two consecutive SIGs are a no-op.
When a trap occurs, the trap register is reset to its initial
condition, to prevent recursive traps from hanging the machine up,
e.g. stack overflow in the stack overflow handling procedure.
.P
The runtime systems for some languages need to ignore some EM
traps.
EM offers a feature called the ignore mask.
It contains one bit for each of the lowest 16 trap numbers.
The bits are numbered 0 to 15, with the least significant bit
having number 0.
If a certain bit is 1 the corresponding trap never
occurs and processing simply continues.
The actions performed by the offending instruction are
described by the Pascal program in appendix A.
.N
If the bit is 0, traps are not ignored.
The instructions LIM and SIM allow copying and replacement of
the ignore mask.~
.P
The TRP instruction generates a trap, the trap number being found on the
stack.
This is, among other things,
useful for library procedures and runtime systems.
It can also be used by a low level trap procedure to pass the trap to a
higher level one (see example below).
.P
The RTT instruction returns from the trap procedure and continues after the
trap.
In the list below all traps marked with an asterisk ('*') are
considered to be fatal and it is explicitly undefined what happens if
you try to restart after the trap.
.P
The way a trap procedure is called is completely compatible
with normal calling conventions. The only way a trap procedure
differs from normal procedures is the return. It has to use RTT instead
of RET. This is necessary because the complete runtime status is saved on the
stack before calling the procedure and all this status has to be reloaded.
Error numbers are in the range 0 to 252.
The trap numbers are divided into three categories:
.IS 4
.N 1
.PS - 10
.PT ~~0-~63
EM machine errors, e.g. illegal instruction.
.PS - 8
.PT ~0-15
maskable
.PT 16-63
not maskable
.PE
.PT ~64-127
Reserved for use by compilers, run time systems, etc.
.PT 128-252
Available for user programs.
.PE 1
.IE
EM machine errors are numbered as follows:
.DS I 5
.TS
tab(@);
n l l.
0@EARRAY@Array bound error
1@ERANGE@Range bound error
2@ESET@Set bound error
3@EIOVFL@Integer overflow
4@EFOVFL@Floating overflow
5@EFUNFL@Floating underflow
6@EIDIVZ@Divide by 0
7@EFDIVZ@Divide by 0.0
8@EIUND@Undefined integer
9@EFUND@Undefined float
10@ECONV@Conversion error
16*@ESTACK@Stack overflow
17*@EHEAP@Heap overflow
18*@EILLINS@Illegal instruction
19*@EODDZ@Illegal size argument
20*@ECASE@Case error
21*@EMEMFLT@Addressing non existent memory
22*@EBADPTR@Bad pointer used
23*@EBADPC@Program counter out of range
24@EBADLAE@Bad argument of LAE
25@EBADMON@Bad monitor call
26@EBADLIN@Argument of LIN too high
27@EBADGTO@GTO descriptor error
.TE
.DE 0
.P
As an example,
suppose a subprocedure has to be written to do a numeric
calculation.
When an overflow occurs the computation has to be stopped and
the higher level procedure must be resumed.
This can be programmed as follows using the mechanism described above:
.DS B
 mes 2,2,2              ; set sizes
ersave
 bss 2,0,0              ; Room to save previous value of trap procedure
msave
 bss 2,0,0              ; Room to save previous value of trap mask

 pro calcule,0          ; entry point
 lxl 0                  ; fill in non-local goto descriptor with LB
 ste jmpbuf+4
 lor 1                  ; and SP
 ste jmpbuf+2
 lim                    ; get current ignore mask
 ste msave              ; save it
 lim
 loc 4                  ; bit for EFOVFL
 ior 2                  ; set in mask
 sim                    ; ignore EFOVFL from now on
 lpi $catch             ; load procedure identifier
 sig                    ; catch wil get all traps now
 ste ersave             ; save previous trap procedure identifier
; perform calculation now, possibly generating overflow
1                       ; label jumped to by catch procedure
 loe ersave             ; get old trap procedure
 sig                    ; refer all following trap to old procedure
 asp 2                  ; remove result of sig
 loe msave              ; restore previous mask
 sim                    ; done now
; load result of calculation
 ret 2                  ; return result
jmpbuf
 con *1,0,0
 end
.DE 0
.VS 1 1
.DS
Example of catch procedure
 pro catch,0            ; Local procedure that must catch the overflow trap
 lol 2                  ; Load trap number
 loc 4                  ; check for overflow
 bne *1                 ; if other trap, call higher trap procedure
 gto jmpbuf             ; return to procedure calcule
1                       ; other trap has occurred
 loe ersave             ; previous trap procedure
 sig                    ; other procedure will get the traps now
 asp 2                  ; remove the result of sig
 lol 2                  ; stack trap number
 trp                    ; call other trap procedure
 rtt                    ; if other procedure returns, do the same
 end
.DE
