.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
! $Header$
.sect .text
.define mon~
.extern sigtrp~,save~,retu~,save1~

indir	= 0
fork	= 2
getpid	= 024
sigtrp	= 060
EBADMON = 031

HBMASK	= 0177400
REG01M	= 030
REG1M	= 020
ERRMASK	= 040

! Associated with every monitor call is a descriptor.
! The low order three bits describe how values are returned,
! the next two bits specify if arguments are expected in
! r0 and/or r1, the next bit is not used, and the next
! three bits specify the number of arguments disregarding
! arguments in registers.

mon~:
	cmp	02(sp),$sigtrp
	bne	1f
	jmp	sigtrp~
1:	jsr	pc,save~
	mov	(sp)+,r4
	mov	r4,r2
	asl	r4
	mov	args(r4),r3
	mov	r3,r4
	bit	$ERRMASK,r4
	bne	err
	cmp	r2,$fork
	bne	2f
	jbr	fork~
2:	bic	$HBMASK,r2
	bis	$0104400,r2 ! sys = 0104400
	mov	r2,9f
	bit	$REG01M,r3
	beq	1f
	mov	(sp)+,r0
	bit	$REG1M,r3
	beq	1f
	mov	(sp)+,r1
1:	ash	$-6,r3
	beq	2f
	mov	$[9f+2],r2
1:	mov	(sp)+,(r2)+
	sob	r3,1b
2:	sys	indir ; .data2 9f
	bcs	2f
	clr	r3
4:	asr	r4
	bcc	1f
	mov	r0,-(sp)
1:	asr	r4
	bcc	1f
	mov	r1,-(sp)
1:	asr	r4
	bcc	1f
	clr	-(sp)
1:	jmp	retu~
2:	mov	r0,-(sp)
	mov	r0,-(sp)
	jmp	retu~
fork~:
	sys	fork
	br	1f
	bcs	2b
	clr	r1
	br	4b
1:	mov	$1,r1
	br	4b
err:
	mov	$EBADMON,-(sp)
	jsr	pc,trp~
	tst	(sp)+
	jmp	retu~
.sect .data
.align 1
9: .space 12
args:	.data2 ERRMASK	! 0 : error
	.data2 010	! 1 : exit(st); ---
	.data2 07	! 2 : fork(); e10
	.data2 0215	! 3 : read(addr,nb,fild); e-0
	.data2 0215	! 4 : write(addr,nb,fild); e-0
	.data2 0205	! 5 : open(str,flag); e-0
	.data2 014	! 6 : close(fild); e--
	.data2 07	! 7 : wait(); e10
	.data2 0205	! 8 : creat(str,mode); e-0
	.data2 0204	! 9 : link(str1,str2); e--
	.data2 0104	!10 : unlink(str); e--
	.data2 ERRMASK	!11 : error
	.data2 0104	!12 : chdir(str); e--
	.data2 03	!13 : time(); -10
	.data2 0304	!14 : mknod(str,mode,addr); e--
	.data2 0204	!15 : chmod(str,mode); e--
	.data2 0304	!16 : chown(str,owner,grp); e--
	.data2 ERRMASK	!17 : error
	.data2 0204	!18 : stat(str,buf); e--
	.data2 0217	!19 : lseek(high,low,fild); e10
	.data2 01	!20 : getpid(); --0
	.data2 0304	!21 : mount(str1,str2,fl); e--
	.data2 0104	!22 : umount(str); e--
	.data2 014	!23 : setuid(uid); e--
	.data2 03	!24 : getuid(); -01
	.data2 024	!25 : stime(high,low); e--
	.data2 0315	!26 : ptrace(pid,addr,req,d); e-0
	.data2 011	!27 : alarm(sec); --0
	.data2 0114	!28 : fstat(buf,fild); e--
	.data2 0	!29 : pause(); ---
	.data2 0204	!30 : utime(str,timep); e--
	.data2 ERRMASK	!31 : error
	.data2 ERRMASK	!32 : error
	.data2 0204	!33 : access(str,mode): e--
	.data2 010	!34 : nice(incr); ---
	.data2 0100	!35 : ftime(bufp); ---
	.data2 0	!36 : sync(); ---
	.data2 0114	!37 : kill(sig,pid); e--
	.data2 ERRMASK	!38 : error
	.data2 ERRMASK	!39 : error
	.data2 ERRMASK	!40 : error
	.data2 025	!41 : dup(fild,newfild); e-0
	.data2 07	!42 : pipe(); e10
	.data2 0100	!43 : times(buf); ---
	.data2 0400	!44 : profil(buff,siz,off,sc); ---
	.data2 ERRMASK	!45 : error
	.data2 014	!46 : setgid(gid); e--
	.data2 03	!47 : getgid(); -01
	.data2 0	!48 : sigtrp(trap,sig); e-0; SPECIAL TREATMENT
	.data2 ERRMASK	!49 : error
	.data2 ERRMASK	!50 : error
	.data2 0104	!51 : acct(file); e--
	.data2 0304	!52 : phys(seg,siz,phaddr); e--
	.data2 0104	!53 : lock(flag); e--
	.data2 0304	!54 : ioctl(fild,req,argp); e--
	.data2 ERRMASK	!55 : error
	.data2 0204	!56 : mpxcall(cmd,vec); e--
	.data2 ERRMASK	!57 : error
	.data2 ERRMASK	!58 : error
	.data2 0304	!59 : exece(name,argv,envp); e--
	.data2 0104	!60 : umask(complmode); e--
	.data2 0104	!61 : chroot(str); e--
