#include "system.h"
#include "em_abs.h"
#define MOVL	movl	(sp)+,(ap)+
#define MOVQ	movq	(sp)+,(ap)+

        # $Header$

.set sigvec,108

.globl	.mon

.mon:
	movl	(sp)+,r2	# return address in r2
	movl	(sp)+,r0	# monitor call in r0
	movl	ap,r3
	movl	$args,ap
	cvtlw	r0,(ap)+
	cmpl	r0,$61
	jgtru	Lbadmon
	ashl	$2,r0,r0
	jmp	*L0(r0)
.data
.align	2
L0:
#ifndef BSD41a
.long	Lbadmon,	Lexit,		Lfork,		Lread,		Lwrite
.long	Lopen,		Lclose,		Lwait,		Lcreat,		Llink
.long	Lunlink,	Lbadmon,	Lchdir,		Lbadmon,	Lbadmon
.long	Lchmod,		Lchown,		Lbadmon,	Lstat,		Llseek
.long	Lgetpid,	Lbadmon,	Lbadmon,	Lbadmon,	Lgetuid
.long	Lbadmon,	Lptrace,	Lalarm,		Lfstat,		Lpause
.long	Lutime,		Lbadmon,	Lbadmon,	Laccess,	Lnice
.long	Lbadmon,	Lsync,		Lkill,		Lbadmon,	Lbadmon
.long	Lbadmon,	Ldup,		Lpipe,		Lbadmon,	Lprofil
.long	Lbadmon,	Lbadmon,	Lgetgid,	Lsigtrp,	Lbadmon
.long	Lbadmon,	Lacct,		Lbadmon,	Lbadmon,	Lioctl
.long	Lbadmon,	Lbadmon,	Lbadmon,	Lbadmon,	Lexece
.long	Lumask,		Lchroot
#else
.long	Lbadmon,	Lexit,		Lfork,		Lread,		Lwrite
.long	Lopen,		Lclose,		Lwait,		Lcreat,		Llink
.long	Lunlink,	Lbadmon,	Lchdir,		Lbadmon,	Lmknod
.long	Lchmod,		Lchown,		Lbadmon,	Lstat,		Llseek
.long	Lgetpid,	Lmount,		Lumount,	Lsetuid,	Lgetuid
.long	Lstime,		Lptrace,	Lalarm,		Lfstat,		Lpause
.long	Lutime,		Lbadmon,	Lbadmon,	Laccess,	Lnice
.long	Lftime,		Lsync,		Lkill,		Lbadmon,	Lbadmon
.long	Lbadmon,	Ldup,		Lpipe,		Ltimes,		Lprofil
.long	Lbadmon,	Lsetgid,	Lgetgid,	Lsigtrp,	Lbadmon
.long	Lbadmon,	Lacct,		Lbadmon,	Llock,		Lioctl
.long	Lbadmon,	Lmpxall,	Lbadmon,	Lbadmon,	Lexece
.long	Lumask,		Lchroot
#endif
.text

	# Each system call first stores its number of arguments, then the
	# arguments themselves.
	# The system calls are grouped according to their number of arguments
	# and their results.
	# Le*: an errornumber is expected
	# L*r0*: the value of r0 is expected
	# L*r1*: the value of r1 is expected
	# Lnores: no results expected

	# 0 arguments.
Lsync:
Lpause:
	clrl	(ap)+
Lnores:
	jsb	Lsyscall
	movl	r3,ap
	jmp	(r2)
Lgetpid:
	clrl	(ap)+
Lr0:
	jsb	Lsyscall
	pushl	r0
	movl	r3,ap
	jmp	(r2)
Lgetuid:
Lgetgid:
	clrl	(ap)+
Lr0r1:
	jsb	Lsyscall
	pushl	r0
	pushl	r1
	movl	r3,ap
	jmp	(r2)
Lpipe:
Lwait:	
Lfork:
	clrl	(ap)+
Ler0r1:
	jsb	Lsyscall
	pushl	r0
	pushl	r1
	clrl	-(sp)
	movl	r3,ap
	jmp	(r2)

	# 1 argument.
Lexit:
Ltimes:
	cvtbl	$1,(ap)+
	MOVL
	jbr	Lnores
Lclose:	
Lunlink:	
Lchdir:
Lumount:
Lsetuid:
Lstime:
Lnice:
Lftime:
Lsetgid:
Lacct:
Llock:
Lchroot:
	cvtbl	$1,(ap)+
	MOVL
Le:
	jsb	Lsyscall
	clrl	-(sp)
	movl	r3,ap
	jmp	(r2)
Lalarm:
Lumask:
	cvtbl	$1,(ap)+
	MOVL
	jbr	Lr0

	# 2 arguments.
Llink:
Lchmod:	
Lstat:	
Lfstat:
Lutime:
Laccess:
Lkill:
Lmpxall:
	cvtbl	$2,(ap)+
	MOVQ
	jbr	Le
Ldup:
	cvtbl	$2,(ap)+
	MOVQ
Ler0:
	jsb	Lsyscall
	pushl	r0
	clrl	-(sp)
	movl	r3,ap
	jmp	(r2)

	# 3 arguments.
Lioctl:
Lexece:
Lmknod:
Lchown:	
Lmount:
	cvtbl	$3,(ap)+
	MOVL
	MOVQ
	jbr	Le
Llseek:
Lread:
Lwrite:
	cvtbl	$3,(ap)+
	MOVL
	MOVQ
	jbr	Ler0

	# 4 arguments.
Lprofil:
	cvtbl	$4,(ap)+
	MOVQ
	MOVQ
	jbr	Lnores
Lptrace:
	cvtbl	$4,(ap)+
	MOVQ
	MOVQ
	jbr	Ler0

Lopen:
#ifndef BSD41a
	# The old open(name, mode) system call is simulated by
	# open(name, mode, 0).
	cvtbl	$3,(ap)+
#else
	cvtbl	$2,(ap)+
#endif
	MOVQ
#ifndef BSD41a
	clrl	(ap)+
#endif
	jbr	Ler0
Lcreat:	
#ifndef BSD41a
	# The old creat(name, mode) system call is simulated by
	# open(name, O_WRONLY | O_CREAT | O_TRUNC, mode).
	cvtbl	$5,-2(ap)
	cvtbl	$3,(ap)+
#else
	cvtbl	$2,(ap)+
#endif
	MOVL
#ifndef BSD41a
	movl	$0x601,(ap)+
#endif
	MOVL
	jbr	Ler0

Lsyscall:
	movl	$args,ap
	chmk	(ap)+
	bcc	L1
	cvtwl	r0,(sp)		# Push the error returned twice,
	cvtwl	r0,-(sp)	# overwrite the return address
	movl	r3,ap
	jmp	(r2)
L1:
	rsb
Lbadmon:
	pushl	$EBADMON
	jmp	.fat
Lsigtrp:
	movl	4(sp),r1	# Sig in r1
	movl	(sp)+,(sp)	# Trapno on top of stack
	cmpl	r1,$16
	jgtru	badsig
	tstl	r1
	jeql	badsig
#ifdef BSD42
	movl	$3,(ap)+	# sigvec(sig, vec, ovec)
#else BSD42
	movl	$2,(ap)+	# signal(sig, func)
#endif BSD42
	movl	r1,(ap)+
	movl	sigadr0 [r1],r0
	tstl	(sp)
	blss	L2
	cmpl	(sp),$252
	bgtr	badtrp
	movl	r0,(ap)+
	brb	sys
L2:	cmpl	$-3,(sp)
	bneq	L3
	movl	$1,(ap)+	# SIG_IGN
	brb	sys
L3:	cmpl	$-2,(sp)
	bneq	badtrp
	clrl	(ap)+		# SIG_DFL
sys:
#ifdef BSD42
	movl	-(ap),vec	# vec->sv_handler = func
	movl	$vec,(ap)+
	movl	$ovec,(ap)+	# Not used.
#endif BSD42
	pushl	sigtrp0 [r1]
	movl	4(sp),sigtrp0 [r1]
	movl	(sp)+,(sp)	# Old trap number on top of stack.
	movl	$args,ap
#ifdef BSD42
	movw	$sigvec,(ap)
#endif
	chmk	(ap)+
	bcc	L4
	movl	(sp),sigtrp0 [r1] # Error, reset old trap number.
	pushl	r0
	movl	r3,ap
	jmp	(r2)
L4:	clrl	-(sp)
	movl	r3,ap
	jmp	(r2)
badsig:
	movl	$-1,(sp)
	pushl	(sp)
	movl	r3,ap
	jmp	(r2)
badtrp:
	movl	sigtrp0 [r1],(sp)
	pushl	$-1
	movl	r3,ap
	jmp	(r2)

sigs:
	.word 0x0000
	# Routine to catch signals.
	pushl	fp
	movl	12(fp),fp
	movl	12(fp),fp	# Restore local base, two levels!
	movl	4(ap),ap
	pushl	sigtrp0 [ap]
	jsb	.trp
	movl	(sp)+,fp
	ret

.data
#ifdef BSD42
vec:
	.long	0		# sv_handler
	.long	0		# sv_mask
	.long	0		# sv_onstack
ovec:
	.long	0		# sv_onstack
	.long	0		# sv_mask
	.long	0		# sv_onstack
#endif BSD42
args:
	.word	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0

sigadr0:
	.long	0	#0
	.long	sigs	#1
	.long	sigs	#2
	.long	sigs	#3
	.long	sigs	#4
	.long	sigs	#5
	.long	sigs	#6
	.long	sigs	#7
	.long	sigs	#8
	.long	sigs	#9
	.long	sigs	#10
	.long	sigs	#11
	.long	sigs	#12
	.long	sigs	#13
	.long	sigs	#14
	.long	sigs	#15
	.long	sigs	#16

sigtrp0:
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
	.long -2
