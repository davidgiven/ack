/ $Header$
.text
.globl mon~
.globl sigtrp~,save~,retu~,save1~

indir	= 0
fork	= 2
getpid	= 20.
sigtrp	= 48.
EBADMON = 25.

HBMASK	= 0177400
REG01M	= 030
REG1M	= 020
ERRMASK	= 040

/ Associated with every monitor call is a descriptor.
/ The low order three bits describe how values are returned,
/ the next two bits specify if arguments are expected in
/ r0 and/or r1, the next bit is not used, and the next
/ three bits specify the number of arguments disregarding
/ arguments in registers.

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
	bis	$sys,r2
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
2:	sys	indir ; 9f
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
.data
.even
9: .=.+12.
args:	ERRMASK	/ 0 : error
	010	/ 1 : exit(st); ---
	07	/ 2 : fork(); e10
	0215	/ 3 : read(addr,nb,fild); e-0
	0215	/ 4 : write(addr,nb,fild); e-0
	0205	/ 5 : open(str,flag); e-0
	014	/ 6 : close(fild); e--
	07	/ 7 : wait(); e10
	0205	/ 8 : creat(str,mode); e-0
	0204	/ 9 : link(str1,str2); e--
	0104	/10 : unlink(str); e--
	ERRMASK	/11 : error
	0104	/12 : chdir(str); e--
	03	/13 : time(); -10
	0304	/14 : mknod(str,mode,addr); e--
	0204	/15 : chmod(str,mode); e--
	0304	/16 : chown(str,owner,grp); e--
	ERRMASK	/17 : error
	0204	/18 : stat(str,buf); e--
	0217	/19 : lseek(high,low,fild); e10
	01	/20 : getpid(); --0
	0304	/21 : mount(str1,str2,fl); e--
	0104	/22 : umount(str); e--
	014	/23 : setuid(uid); e--
	03	/24 : getuid(); -01
	024	/25 : stime(high,low); e--
	0315	/26 : ptrace(pid,addr,req,d); e-0
	011	/27 : alarm(sec); --0
	0114	/28 : fstat(buf,fild); e--
	0	/29 : pause(); ---
	0204	/30 : utime(str,timep); e--
	ERRMASK	/31 : error
	ERRMASK	/32 : error
	0204	/33 : access(str,mode): e--
	010	/34 : nice(incr); ---
	0100	/35 : ftime(bufp); ---
	0	/36 : sync(); ---
	0114	/37 : kill(sig,pid); e--
	ERRMASK	/38 : error
	ERRMASK	/39 : error
	ERRMASK	/40 : error
	025	/41 : dup(fild,newfild); e-0
	07	/42 : pipe(); e10
	0100	/43 : times(buf); ---
	0400	/44 : profil(buff,siz,off,sc); ---
	ERRMASK	/45 : error
	014	/46 : setgid(gid); e--
	03	/47 : getgid(); -01
	0	/48 : sigtrp(trap,sig); e-0; SPECIAL TREATMENT
	ERRMASK	/49 : error
	ERRMASK	/50 : error
	0104	/51 : acct(file); e--
	0304	/52 : phys(seg,siz,phaddr); e--
	0104	/53 : lock(flag); e--
	0304	/54 : ioctl(fild,req,argp); e--
	ERRMASK	/55 : error
	0204	/56 : mpxcall(cmd,vec); e--
	ERRMASK	/57 : error
	ERRMASK	/58 : error
	0304	/59 : exece(name,argv,envp); e--
	0104	/60 : umask(complmode); e--
	0104	/61 : chroot(str); e--
