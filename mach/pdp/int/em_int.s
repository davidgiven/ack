/
/  (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The	Netherlands.
/
/	    This product is part of the	Amsterdam Compiler Kit.
/
/  Permission to use, sell, duplicate or disclose this software	must be
/  obtained in writing.	Requests for such permissions may be sent to
/
/	Dr. Andrew S. Tanenbaum
/	Wiskundig Seminarium
/	Vrije Universiteit
/	Postbox	7161
/	1007 MC	Amsterdam
/	The Netherlands
/
/
/------------------------------------------------------------------------------
/
/	This is	an interpreter for EM programs with no virtual memory
/	which is adapted from an EM1 interpreter by Hans van Staveren
/	by	Evert Wattel
/		Vrije Universiteit
/		Amsterdam
/
/	Memory layout:
/
/	 interpreter  em-text  pd  global  tables  heap	 unused	stack
/	__________________________________________________________________
/	|	     |	     |	  |	  |	  |	|      |      |	 |
/	|	     |	     |	  |	  |	  |	|      |      |	 |
/	|      1     |	 2   |	3 |   4	  |   5	  |  6	|      |  7   |	8|
/	|	     |	     |	  |	  |	  |	|      |      |	 |
/	|____________|_______|____|_______|_______|_____|______|______|__|
/
/	1:	Interpreter text+data+bss
/	2:	EM text
/	3:	EM procedure descriptors
/	4:	EM global data area
/	5:	flow, count and	profile	tables
/	6:	EM heap	area
/	7:	EM local data and stack
/	8:	Arguments to interpreter
/
/	Assembly time flags:
/	.test  : controls checking for undefined variables,nil pointers,
/		 array indices,	etc....
/	.prof  : controls generation of	a runtime profile
/	.opfreq: controls runtime frequency count per opcode
/	.flow  : controls generation of	a flow bitmap
/	.count : controls generation of	a flow count
/	.last  : controls generation of	file with last 16
/		 consecutive blocks of lines executed
/
/	Register layout:
/	pcx = EM programcounter
/	lb  = EM base-address
/	nxt = address of start of interpreter loop
/
/	The general structure of this interpreter is as	follows:
/	The opcode byte	of the instruction is placed in	r0
/	with sign-extension and	multiplied by 2.
/	If .opfreq is nonzero each occurence of	each opcode is counted.
/	Then, if .prof is nonzero an estimation	of the time required
/	to execute the instruction is added to a counter associated
/	with the source-line number. This estimation is	roughly	the
/	number of memory-cycles	needed.	At the end of this accounting
/	loprof points to the loword of the double precision counter.
/	This can be used by individual execution routines to add some
/	more to	the counter depending on their operand.
/
/	NOTE: This interpreter can be loaded in	separate I and D space
/
/
/------------------------------------------------------------------------------
/	Declaring of some constants
/------------------------------------------------------------------------------

	nxt	= r4
	pcx	= r3
	lb	= r2

	statd	= -8.

	unixextra= 1280.	/ extra	memory asked by	heap overflow
	und	= 100000	/ undefined memory pattern
	signext	= 177400	/ high bits for	signextension
	EINVAL	= 22.		/ UNIX error code for bad signal

/ Interpreter options
	.float	 = 1
	.opfreq	= 0
	.last	= 1
	V7	= 1
	V6	= 0
	VPLUS	= 0
	HARDWARE_FP	= 1
/------------------------------------------------------------------------------
/	EM1 machine errors (in the range 0-63)
/------------------------------------------------------------------------------

	EARRAY	= 0.
	ERANGE	= 1.
	ESET	= 2.
	EIOVFL	= 3.
	EFOVFL	= 4.
	EFUNFL	= 5.
	EIDIVZ	= 6.
	EFDIVZ	= 7.
	EIUND	= 8.
	EFUND	= 9.
	ECONV	= 10.
	ESTACK	= 16.
	EHEAP	= 17.
	EILLINS	= 18.
	EODDZ	= 19.
	ECASE	= 20.
	EMEMFLT	= 21.
	EBADPTR	= 22.
	EBADPC	= 23.
	EBADLAE	= 24.
	EBADMON	= 25.
	EBADLIN	= 26.
	EBADGTO	= 27.

/------------------------------------------------------------------------------
/	Declaring of some instructions unknown to the assembler
/------------------------------------------------------------------------------

	next	= 10407		/ = mov	nxt,pc;	jump to	decode loop
	rti	= 2		/ return from interrupt
	iot	= 4		/ force	core dump
	stst	= 170300^tst	/ store	floating point status
	indir	= 0		/ for sys indir
	exit	= 1
	fork	= 2
	read	= 3
	write	= 4
	open	= 5
	close	= 6
	creat	= 8.
	break	= 17.
	alarm	= 27.
	pause	= 29.
	sleep	= 35.
	signal	= 48.

/------------------------------------------------------------------------------
/	External references
/------------------------------------------------------------------------------

	.globl	_end

/
/------------------------------------------------------------------------------
/	Now the	real program starts
/------------------------------------------------------------------------------

startoff:
	mov	sp,r0
	mov	sp,ml
	mov	02(sp),filb	/ pointer to argv in filb for error message
	dec	(r0)
	mov	(r0)+,argc	/ pass to userprogram later
	bgt	0f		/ go for argument
	mov	$emfile,forward+2    / e.out is	load file default
	mov	$forward+2,argv
	br	1f
0:
	tst	(r0)+		/ skip interpreter name
	mov	r0,argv		/ pass to userprogram later
	mov	(r0),forward+2	     / argv filename to	open call
1:
.if V7
	tst	(r0)+		/ increment r0 look for	last arg
	bne	1b
	mov	r0,environ
.endif
	sys	indir;forward
	.data
forward:      sys     open;0;0
emfile:	<e.out\0>
.even
	.text
	jes	badarg
	mov	r0,saver0	/ save filedescriptor
	mov	r0,r5		/ duplicate filedescriptor
	sys	read;header;16.	/ skip first header
	jes	badarg		/ failed
	mov	r5,r0		/ recall fildes
	sys	read;header;16.	/ read second header
	jes	badarg		/ failed
	cmp	r0,$16.		/ long enough ?
	jne	badarg		/ no.
	mov	$_end,r0	/ Bottom em-text
	mov	r0,pb		/ program base
	add	txtsiz,r0	/ reserve space	for text
	mov	r0,pd		/ set up proc.descr base
	mov	nprocs,	r3	/ number of procs
	ash	$2,r3		/ proc.	descr is 4 bytes
.if .count +.prof + .flow
	mul	$3,r3		/ or 12	bytes
.endif
	add	r3,r0		/ reserve space
	mov	r0,eb		/ top of pd space
	mov	r0,r3		/ base for data	fill

	add	szdata,r0	/ size of external data
	jcs	toolarge	/ too much text and data
	mov	r0,globmax	/ maximum global
	add	$1280.,r0	/ bit extra core for setup
	mov	r0,sybreak+2	/ set up for core claim
	sys	indir;sybreak	/ ask for the core
	jes	toolarge	/ too much, sorry

	mov	txtsiz,leescal+4     / set up for text read
	mov	pb,leescal+2	     / start address text read
	mov	r5,r0		/ file descriptor input
	sys	indir;leescal	     / read!!
	.data
leescal:
1:	sys	read;0;0	/ read call
	.text
lblread:


/ hier is nu dus de tekst ingelezen. De	sybreak	voor de
/ tabellen  en de data moet opnieuw gebeuren.


.if .last
	mov	$47.,r0
	mov	$lasttab,r5
3:	clr	(r5)+
	sob	r0,3b
	mov	$-1,(r5)
	sub	$96.,r5
	mov	r5,linused
.endif
lblfloat:
.if .float
	sys	signal;8.;sig8	/ catch	floating exception
	ldfps	$7600
	movf	$50200,fr3	/ load 2^32 in fr3 for conversion
				/ unsigned to float
.endif


	sys	signal;11.;sig11	/ catch	segmentation violation
	sys	signal;12.;sig12	/ catch	bad system calls

			/ We make a 1024 buffer	for reading in
			/ data descriptors. When the number of
			/ bytes	in the buffer is less than 512
			/ we read another block. Descriptors of
			/ more than 512	bytes are not allowed.
			/ This is no restriction since they do
			/ not fit in the descriptor format.
lblbuf:

	sub	$02000,sp	/ new buffer bottom
	tst	(sp)		/ ask for core
	mov	sp,r4		/ pointer in descriptor
	mov	saver0,r0	/ recall fildes
	clr	r1		/ clear	registers for byte
	clr	r2		/ format instruction and data
	mov	sp,r5		/ copy
	mov	r5,leescal+2	/ set up for read
	mov	$02000,leescal+4     / idem
	sys	indir;leescal	     / read
	jes	badarg		/ read failed
	cmp	$02000,r0	/ not yet eof?
	bgt	0f		/ eof encountered

	add	$01000,r5	/ buffer middle
	mov	r5,saver1	/ save buffermiddle to compare
	br	datloop		/ start	data initialization
0:	add	r0,r5		/ now pointer at top of	file
	mov	r5,saver1	/ still	set up for compare




datloop:
	cmp	r4,saver1	/ descriptor over middle?
	ble	9f		/ no? go ahead
	jsr	pc,blshift	/ yes? shift block down, read next

9:	dec	ndatad		/ still	data to	initialize?
	blt	finito		/ no? go on
	movb	(r4)+,r1	/ opcode descriptor
	beq	0f		/ if 0 then go there
	mov	r3,r5		/ copy data pointer
	clr	r2		/ unsigned integer byte
	bisb	(r4)+,r2	/ "ored" in for	data size
	asl	r1		/ make opcode even
	mov	datswi(r1),pc  / main data swich

.data
datswi:	0;	dat1;	dat2;	dat3;	dat4;	dat5;	dat6;	dat6;	dofloat
.text
dat3:	asl	r2		/ multiply with	2
dat2: 2: movb	(r4)+,(r3)+	/ copy byte from buffer	to data
	sob	r2,2b		/ until	r2 is 0
	br	datloop		/ next datadescriptor


dat4:	mov	eb,r0		/ external base	should be added
	br	2f		/ for data pointers

dat5:	mov	pb,r0		/ and program base for procedures

2:	movb	(r4)+,(r3)	/ move in first	byte of	pointer
	movb	(r4)+,1(r3)	/ move in second byte of pointer
	add	r0,(r3)+	/ add pointer base
	sob	r2,2b		/ jump back if there is	more
	br	datloop		/ next data descriptor

dat1:	mov	$und,(r3)+	/ reserve words	with undefineds
	sob	r2,2b		/ jump back if more
	br	datloop		/ next data descriptor

0:	mov	r3,r1		/ copy data pointer (odd register)
	sub	r5,r1		/ subtract previous pointer
	movb	(r4)+,(r3)	/ copy first byte of operand
	movb	(r4)+,1(r3)	/ copy second byte
	mul	(r3),r1		/ the number of	bytes to copy
1:	movb	(r5)+,(r3)+	/ is the product of the	operand
	sob	r1,1b		/ and the number of bytes in the
	br	datloop		/ previous operation

dat6:	add	r2,r3		/ new data pointer, the	old is
	mov	r3,r0		/ still	in r5
	asr	r2		/ divide by 2
	beq	6f		/ case 1 byte is special
	sub	$2,r0		/ this is the least significant
				/ byte in PDP11-standard
2:	movb	(r4)+,(r0)+	/ copy low byte
	movb	(r4)+,(r0)	/ copy high byte
	sub	$3,r0		/ next lowest byte
	sob	r2,2b		/ jump if not ready
	br	datloop		/ next descriptor
6:	movb	(r4)+,(r5)	/ copy one byte
	br	datloop		/ next descriptor

blshift:
	mov	saver1,r1	/ bottom of top	half
	mov	r1,r2		/ set up bottom
	sub	$01000,r2
	mov	$1000,r0	/ number to copy
	mov	r0,leescal+4	     / amount to read
	sub	r0,r4		/ decrease pointer
	asr	r0		/ 512 bytes is 256 words
3:	mov	(r1)+,(r2)+	/ copy top half	in bottom half
	sob	r0,3b
	mov	saver1,leescal+2     / set up for read
blockr:
	mov	saver0,r0		/ filedescriptor
	sys	indir;leescal
	jes	badarg
	clr	r1		/ clear	registers which	contain
	clr	r2		/ descriptor bytes later
	cmp	$01000,r0	/ look if eof is encountered
	beq	3f		/ yes? go on
	add	r0,saver1	/ no extra read	necessary
3:	rts	pc

finito:
       cmp     globmax,r3	    / test if data size	ok
       jne     badarg	       / load file error
	mov	eb,filb
	add	$4,filb


	mov	nprocs,r5	/ set up for procdesc read
	mov	pd,r3		/ proc descriptor base
	asl	r5		/ multiply with	4 because
	asl	r5		/ procdes is 4 bytes
1:	mov	saver1,r1	/ look what is available
	sub	r4,r1		/ in buffer to be read
	add	$3,r1		/ let it be a multiple
	bic	$3,r1		/ of four
	sub	r1,r5		/ subtract what	can be read
	asr	r1;   asr  r1;	/ divide by four
0:
	movb	(r4)+,(r3)+	/ copy byte
	movb	(r4)+,(r3)+	/ copy byte
	movb	(r4)+,(r3)+	/ copy byte
	movb	(r4)+,(r3)+	/ copy byte
	add	pb,-2(r3)	/ change em-address in pdp-address
.if .count + .prof + .flow
	clr	(r3)+
	clr	(r3)+
	clr	(r3)+
	clr	(r3)+
.endif
	sob	r1,0b		/ look if there	is more
	tst	r5		/ is there still a descriptor
	ble	2f;		/ no? go on
	jsr	pc,blshift	/ yes? read again
	br	1b

2:
	cmp	eb,r3	     / test if procdes is ok
	jne	badarg		/ load file error
	mov	saver0,r0	/ fildes in r0
	sys	close		/ close	input load file
	mov	ml,sp		/ fresh	stack
	mov	2(sp),*filb
.if .flow + .count + .prof
/					   |==================|
/ Here we fill the fields in the procedure | bytes for locals |
/ descriptor with table	information. The   |------------------|
/ procedure descriptor has six fields,	   |  start address   |
/ like described in this picture. We	   |------------------|
/ construct a linked list of the proc.	   |  count pointer   |
/ descriptors, such that the defined	   |------------------|
/ order	of procedures is compatible	   |   first line nr  |
/ with the text	order. Thereafter we	   |------------------|
/ scan the text	for line information to	   |  link next	proc  |
/ fill the countpointer	and startline	   |------------------|
/ field. The link to the first proc.	   | current file name|
/ is in	firstp,	links are descriptor	   |==================|
/ start	addresses. The last procedure
/ links	to the external	base. All lines	in the text get	a count
/ number, lines	of a procedure get consecutive count numbers,
/ the procedure	count pointer gives the	number of the first line.
/ Count	pointer	zero is	reserved for the case that no line number
/ is yet defined.

makelink:
	mov	pd,r0		/ first	descriptor
	mov	r0,r3		/ points to first proc
	mov	r0,r4		/ pd in	register
	mov	eb,r5		/ eb in	register

0:	mov	r0,r1		/ copy old descriptor bottom
	add	$12.,r0		/ next descriptor
	cmp	r0,r5		/ top of descriptor space
	bhis	4f		/ ready? continue
1:	cmp	2(r0),2(r1)	/ compare start	addresses
	bhis	2f		/ 2(r0)	large? follow link
	sub	$12.,r1		/ 2(r0)	small? previous	descriptor
	cmp	r1,r4		/ is r1	smaller	than pd?
	bhis	1b		/ no? try again
	mov	r3,8.(r0)	/ yes? then r0 has small text address
	mov	r0,r3		/ now r3 again points to first proc
	br	0b		/ next descriptor

2:	mov	8.(r1),r2	/ follow link to compare with 2(r0)
	beq	3f		/ if 0 then no link defined
	cmp	2(r0),2(r2)	/ compare start	addresses
	blo	3f		/ r0 between r1	and r2
	mov	r2,r1		/ r0 above r2,
	br	2b		/ look again.

3:	mov	r0,8.(r1)	/ link of r1 points to r0
	mov	r2,8.(r0)	/ link of r0 points to r2
	br	0b		/ next descriptor

4:	mov	r3,firstp	/ firstp links to first	procedure

procinf:
	mov	$1,maxcount	/ countptr for first proc
	mov	r3,r4		/ points to first proc

0:	mov	r3,-(sp)	/ stack	current	procedure
	mov	$-1,r1		/ minimal line number 0177777
	clr	r5		/ maximum line number on 0
	mov	8.(r3),r4	/ bottom address next descriptor
	beq	6f		/ if 0 last procedure
	mov	2(r4),r4	/ top of current procedure
	br	2f		/ start	looking	for lines
6:	mov	pd,r4		/ top of last procedure
2:
	mov	2(r3),r3	/ start	text address procedure
8:	movb	(r3)+,r2	/ first	opcode for scanning
	cmp	$-2,r2		/ case escape
	beq	1f		/ escape treated at label 1
	cmp	$-106.,r2	/ case lni
	blt	7f		/ ordinary skip	at label 7
	beq	2f		/ lni treated at label 2
	cmp	$-108.,r2	/ case lin.l
	bgt	7f		/ ordinary skip	at label 7
	beq	3f		/ lin.l	at label 3
	clr	r0		/ lin.s0 treated here
	bisb	(r3)+,r0	/ line number in r0
	br	4f		/ compares at label 4
2:	inc	r0		/ lni increases	line number
	br	4f		/ compares at label 4
3:	jsr	pc,wrdoff	/ get 2	byte number
4:
	cmp	r1,r0		/ look if r0 less than minimum
	blo	5f		/ nothing to declare
	mov	r0,r1		/ r0 new minimum
5:	cmp	r0,r5		/ look if r0 more than maximum
	blo	9f		/ nothing spectacular
	mov	r0,r5		/ r0 new maximum
	br	9f		/ line processed

1:	clr	r2
	bisb	(r3)+,r2	/ escaped instruction opcode
	add	$128.,r2	/ ready	for table entry
7:	movb	skipdisp(r2),r2	/ skip the required number of bytes
	add	r2,r3

9:	cmp	r3,r4		/ still	more text in this proc?
	blt	8b		/ yes? again
filpd:
	mov	(sp)+,r3	/ get bottom descriptor	back
	sub	r1,r5		/ number of lines encountered
	bcs	9f		/ no lines then	no file	information
	mov	maxcount,4(r3)	/ this is the count pointer
	mov	r1,6(r3)	/ minimum line in descriptor
	inc	r5
	add	r5,maxcount	/ this is the new maximum
9:	mov	8.(r3),r3	/ follow link to next procedure
	bne	0b		/ restart
.data
.byte 2; .byte 2; .byte	0; .byte 0; .byte 1; .byte 1; .byte 1; .byte 0;
.byte 0; .byte 2; .byte	1; .byte 0; .byte 1; .byte 0; .byte 0; .byte 1;
.byte 1; .byte 1; .byte	0; .byte 0; .byte 2; .byte 1; .byte 0; .byte 2;
.byte 0; .byte 1; .byte	1; .byte 2; .byte 1; .byte 1; .byte 1; .byte 1;
.byte 1; .byte 2; .byte	0; .byte 0; .byte 0; .byte 0; .byte 1; .byte 2;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 1; .byte 2; .byte 2;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 1; .byte 1; .byte 0; .byte 0;

.byte 0; .byte 1; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 1;
.byte 0; .byte 0; .byte	1; .byte 0; .byte 0; .byte 1; .byte 1; .byte 1;
.byte 1; .byte 0; .byte	2; .byte 1; .byte 1; .byte 1; .byte 2; .byte 0;
.byte 0; .byte 1; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 1;
.byte 2; .byte 2; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 1; .byte	0; .byte 0; .byte 0; .byte 0; .byte 2; .byte 1;
.byte 1; .byte 1; .byte	1; .byte 1; .byte 1; .byte 1; .byte 1; .byte 1;
.byte 2; .byte 1; .byte	0; .byte 0; .byte 1; .byte 2; .byte 7; .byte 5;

skipdisp:
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 1; .byte 0; .byte 0; .byte 2; .byte 0;
.byte 0; .byte 1; .byte	1; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	1; .byte 2; .byte 1; .byte 1; .byte 1; .byte 1;
.byte 1; .byte 1; .byte	1; .byte 2; .byte 1; .byte 1; .byte 1; .byte 1;

.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 0; .byte 0; .byte 0; .byte 0;
.byte 0; .byte 0; .byte	0; .byte 0; .byte 1; .byte 0; .byte 0; .byte 0;
.byte 1; .byte 0; .byte	0; .byte 0; .byte 1; .byte 0; .byte 0; .byte 0;
.byte 1; .byte 1; .byte	0; .byte 1; .byte 0; .byte 2; .byte 0; .byte 2;
.byte 1; .byte 0; .byte	0; .byte 0; .byte 1; .byte 1; .byte 0; .byte 1;
.byte 2; .byte 1; .byte	1; .byte 1; .byte 1; .byte 1; .byte 1; .byte 1;

/escaped opcodes

.byte 2; .byte 0; .byte	2; .byte 0; .byte 2; .byte 0; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	2; .byte 0; .byte 2; .byte 2; .byte 0; .byte 2;
.byte 2; .byte 2; .byte	2; .byte 2; .byte 0; .byte 2; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	0; .byte 0; .byte 2; .byte 0; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	2; .byte 0; .byte 2; .byte 0; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	0; .byte 0; .byte 0; .byte 2; .byte 2; .byte 2;
.byte 2; .byte 2; .byte	0; .byte 2; .byte 0; .byte 2; .byte 0; .byte 2;
.byte 0; .byte 2; .byte	0; .byte 2; .byte 0; .byte 2; .byte 2; .byte 2;

.byte 0; .byte 2; .byte	0; .byte 2; .byte 0; .byte 2; .byte 2; .byte 2;
.byte 2; .byte 2; .byte	2; .byte 2; .byte 0; .byte 2; .byte 0; .byte 1;
.byte 2; .byte 2; .byte	2; .byte 2; .byte 0; .byte 2; .byte 0; .byte 2;
.byte 0; .byte 0; .byte	2; .byte 0; .byte 2; .byte 0; .byte 0; .byte 2;
.byte 0; .byte 2; .byte	2; .byte 0; .byte 2; .byte 0; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	0; .byte 2; .byte 0; .byte 2; .byte 0; .byte 2;
.byte 0; .byte 2; .byte	0; .byte 2; .byte 0; .byte 2; .byte 2; .byte 2;
.byte 2; .byte 2; .byte	0; .byte 0; .byte 2; .byte 2; .byte 0; .byte 2;

.byte 0; .byte 2; .byte	0; .byte 2; .byte 0; .byte 2; .byte 0; .byte 2;
.byte 2; .byte 0; .byte	1; .byte 0; .byte 0; .byte 0; .byte 2; .byte 0;
.byte 2; .byte 0; .byte	2; .byte 2; .byte 2; .byte 2; .byte 2; .byte 2;
.byte 0; .byte 2; .byte	0; .byte 1; .byte 2; .byte 0; .byte 0; .byte 2;
.text
	mov	globmax,r3	/ bottom of table space
	mov	r3,r5		/ copy
.if .prof
	mov	r3,ltime	/ set up pointer to base
mov	r3,hiprof
mov	r3,loprof
add	$2,loprof
	mov	maxcount,r0	/ number of lines
	inc	r0
	asl	r0		/ four byter per prof count
	asl	r0
	add	r0,r3
	mov	r0,profsiz
.endif
.if .flow
	mov	r3,lflow	/ set up pointer to base
	mov	maxcount,r0
	ash	$-3,r0		/ divide by 8
	add	$2,r0
	bic	$1,r0		/ Rounded up to	an integral number of words
	add	r0,r3
	mov	r0,flowsiz
.endif
.if .count
	mov	r3,lcount	/ set up pointer
	mov	maxcount,r0
	inc	r0
	ash	$2,r0		/ multiply by 4
	add	r0,r3
	mov	r0,countsiz
.endif
	mov	r3,tblmax
	cmp	r3,sybreak+2	/ core available for tables?
	blos	2f
	mov	r3,sybreak+2
	sys	indir;sybreak	/ ask for core
2:	sub	r5,r3		/ this amount of space required
	asr	r3
2:	clr	(r5)+		/ clear	table space
	sob	r3,2b
.endif
.if  [1	- .count] * [1 - .flow]	* [1 - .prof]
	mov	globmax,tblmax
.endif

		     / start calling sequence here
calseq:
	mov	tblmax,hp
	mov	pd,r3		/ top of em-text and top of stack
	clr	r2		/ are dummy return values
	mov	environ,-(sp)	/ setup	environment pointer
	mov	argv,-(sp)	/ setup	argument pointer
	mov	*argv,*filb	/ setup	first file message
	mov	argc,-(sp)	/ setup	argument count
	mov	entry.,-(sp)	/ start	procedure to call
precal:
	mov	$loop,r4	/ main loop address in r4
	jbr	cai.z		/ according to the cai

noarg:	mov	r0,argv
	mov	$0f,r0;		jbr	rude_error
badarg:	mov	$1f,r0;		jbr	rude_error
toolarge:mov	$2f,r0;		jbr	rude_error


	.data
0:	<no load file\0>
1:	<load file error\0>
2:	<program too large\0>
	.even
	.text


dofloat:
	jsr	pc,atof
	mov	r5,r3		/ restore r3
				/ Assumed that the result is 8
				/ bytes	Recall r2 and move the
				/ amount of bytes asked	for
	clr	r1		/ restore for opcode
	sub	$8.,r2		/ 8 bytes?
	beq	1f		/ yes! later, 4	bytes next
	movfo	fr0,-(sp)	/ push result
	mov	(sp)+,(r3)+	/ write	result in data
	mov	(sp)+,(r3)+	/ idem
	jbr	datloop		/ next loop
1:	movf	fr0,-(sp)	/ push result
	mov	(sp)+,(r3)+	/ write	result in data
	mov	(sp)+,(r3)+	/ write	result in data
	mov	(sp)+,(r3)+	/ write	result in data
	mov	(sp)+,(r3)+	/ write	result in data
	jbr	datloop
atof:
	mov	r2,-(sp)		/ save byte count
	clr	-(sp)
	clrf	fr0
	clr	r2
1:
	movb	(r4)+,r0		/ get byte
	cmp	$' ,r0
	bge	1b
	cmpb	r0,$'+
	beq	1f
	cmpb	r0,$'-
	bne	2f
	inc	(sp)
1:
	movb	(r4)+,r0		/ get next byte
2:
	sub	$'0,r0
	cmp	r0,$9.
	bhi	2f
	jsr	pc,digit
		br 1b
	inc	r2
	br	1b
2:
	cmpb	r0,$'.-'0
	bne	2f
1:
	movb	(r4)+,r0		/ get next byte
	sub	$'0,r0
	cmp	r0,$9.
	bhi	2f
	jsr	pc,digit
		dec r2
	br	1b
2:
	cmpb	r0,$'E-'0
	beq	3f
	cmpb	r0,$'e-'0
	bne	1f
3:
	clr	r3
	clr	r1
	movb	(r4)+,r0		/ get next byte
	cmpb	r0,$'+
	beq	3f
	cmpb	r0,$'-
	bne	5f
	inc	r3
3:
	movb	(r4)+,r0		/ get next byte
5:
	sub	$'0,r0
	cmp	r0,$9.
	bhi	3f
	mul	$10.,r1
	add	r0,r1
	br	3b
3:
	tst	r3
	bne	3f
	neg	r1
3:
	sub	r1,r2
1:
	movf	$one,fr1
	movf	$one,fr2
	mov	r2,-(sp)
	beq	2f
	bgt	1f
	neg	r2
1:
	mulf	$twohalf,fr1
	mulf	$four,fr2
	sob	r2,1b
2:
	tst	(sp)+
	bge	1f
	divf	fr1,fr0
	divf	fr2,fr0
	br	2f
1:
	mulf	fr1,fr0
	mulf	fr2,fr0
2:
	tst	(sp)+
	beq	1f
	negf	fr0
1:	mov	(sp)+,r2
	rts	pc


digit:
	cmpf	$big,fr0
	cfcc
	blt	1f
	mulf	$ten,fr0
	movif	r0,fr1
	addf	fr1,fr0
	rts	pc
1:
	add	$2,(sp)
	rts	pc
/
/
one	= 40200
twohalf	= 40440
four	= 40600
ten	= 41040
big	= 56200
huge	= 77777


/------------------------------------------------------------------------------
/------------------------------------------------------------------------------
/	Main loop of interpreter starts	here
/------------------------------------------------------------------------------

loop:
	movb	(pcx)+,r0		/ pickup opcode	+ sign extend
9:	asl	r0			/ opcode now -256 .. 254 & even

.if .opfreq
	mov	r0,r1
	asl	r1			/ multiply by two again
	add	$1,counttab+514.(r1)	/ cannot be inc
	adc	counttab+512.(r1)	/ double precision counters
.endif
.if .prof
	add	timeinf(r0),*loprof
	adc	*hiprof			   / double precision
.endif
	mov	dispat(r0),pc		/ fast dispatch

/------------------------------------------------------------------------------
/	Two byte opcodes come here for decoding	of second byte
/------------------------------------------------------------------------------

escape1:
	clr	r0
	bisb	(pcx)+,r0		/ fetch	second byte no sign extend
	asl	r0			/ 0 to 512 & even
	cmp	$0500,r0		/ look for righ	range
	jlt	e.illins

.if .opfreq
	mov	r0,r1
	asl	r1			/ multiply by two again
	add	$1,counttab+1026.(r1)	/ cannot be inc
	adc	counttab+1024.(r1)	/ double precision counters
.endif
.if .prof
	add	time2inf(r0),*loprof
	adc	*hiprof			   / double precision
.endif
	mov	dispae1(r0),pc		  / fast dispatch

/----------------------------------------------------------------------------
escape2:
	movb	(pcx)+,r0		/ fetch	second byte and	sign extend
	jne	e.illins

.if .opfreq
	add	$1,counttab+1666.	/ cannot be inc
	adc	counttab+1664.		/ double precision counters
.endif
	jbr	loc.f			/ fast dispatch
/------------------------------------------------------------------------------
/	dispatch tables, first the unescaped opcodes
/
/	name convention	is as follows:
/	each execution routine has as a	name the name of the instruction
/	followed by a dot and a	suffix.
/	suffix can be an integer (sometimes followed by	a W),
/	an 's'or a 'w',	followed by an integer,	an 'l' ,a 'p' ,
/	a 'n', sometimes followed by a 'w',  or	a 'z'.
/	loc.1		routine	to execute loc 1
/	zge.s0		routine	to execute zge 0 thru 255
/	lae.w1		routine	to execute lae 1024 thru lae 2046
/	lof.2W		routine	to execute lof 2*the word size
/	lol.pw		routine	to execute positive lol	instructions
/	loe.l		routine	to execute all loe instructions
/	add.z		routine	to execute instruction without operand
/			or with	operand	on the stack.
/------------------------------------------------------------------------------
	.data



lal.p;	 lal.n;	  lal.0;   lal._1;  lal.w0;  lal.w_1; lal.w_2; lar.1W
ldc.0;	 lde.lw;  lde.w0;  ldl.0;   ldl.w_1; lfr.1W;  lfr.2W;  lfr.s0
lil.w_1; lil.w0;  lil.0;   lil.1W;  lin.l;   lin.s0;  lni.z;   loc.l
loc._1;	 loc.s0;  loc.s_1; loe.lw;  loe.w0;  loe.w1;  loe.w2;  loe.w3
loe.w4;	 lof.l;	  lof.1W;  lof.2W;  lof.3W;  lof.4W;  lof.s0;  loi.l
loi.1;	 loi.1W;  loi.2W;  loi.3W;  loi.4W;  loi.s0;  lol.pw;  lol.nw
lol.0;	 lol.1W;  lol.2W;  lol.3W;  lol._1W; lol._2W; lol._3W; lol._4W
lol._5W; lol._6W; lol._7W; lol._8W; lol.w0;  lol.w_1; lxa.1;   lxl.1
lxl.2;	 mlf.s0;  mli.1W;  mli.2W;  rck.1W;  ret.0;   ret.1W;  ret.s0
rmi.1W;	 sar.1W;  sbf.s0;  sbi.1W;  sbi.2W;  sdl.w_1; set.s0;  sil.w_1
sil.w0;	 sli.1W;  ste.lw;  ste.w0;  ste.w1;  ste.w2;  stf.l;   stf.1W
stf.2W;	 stf.s0;  sti.1;   sti.1W;  sti.2W;  sti.3W;  sti.4W;  sti.s0
stl.pw;	 stl.nw;  stl.0;   stl.1W;  stl._1W; stl._2W; stl._3W; stl._4W
stl._5W; stl.w_1; teq.z;   tgt.z;   tlt.z;   tne.z;   zeq.l;   zeq.s0
zeq.s1;	 zer.s0;  zge.s0;  zgt.s0;  zle.s0;  zlt.s0;  zne.s0;  zne.s_1
zre.lw;	 zre.w0;  zrl._1W; zrl._2W; zrl.w_1; zrl.nw;  escape1; escape2


dispat:		      /	dispatch table for unescaped opcodes


loc.0;	 loc.1;	  loc.2;   loc.3;   loc.4;   loc.5;   loc.6;   loc.7
loc.8;	 loc.9;	  loc.10;  loc.11;  loc.12;  loc.13;  loc.14;  loc.15
loc.16;	 loc.17;  loc.18;  loc.19;  loc.20;  loc.21;  loc.22;  loc.23
loc.24;	 loc.25;  loc.26;  loc.27;  loc.28;  loc.29;  loc.30;  loc.31
loc.32;	 loc.33;  aar.1W;  adf.s0;  adi.1W;  adi.2W;  adp.l ;  adp.1
adp.2;	 adp.s0;  adp.s_1; ads.1W;  and.1W;  asp.1W;  asp.2W;  asp.3W
asp.4W;	 asp.5W;  asp.w0;  beq.l;   beq.s0;  bge.s0;  bgt.s0;  ble.s0
blm.s0;	 blt.s0;  bne.s0;  bra.l;   bra.s_1; bra.s_2; bra.s0;  bra.s1
cal.1;	 cal.2;	  cal.3;   cal.4;   cal.5;   cal.6;   cal.7;   cal.8
cal.9;	 cal.10;  cal.11;  cal.12;  cal.13;  cal.14;  cal.15;  cal.16
cal.17;	 cal.18;  cal.19;  cal.20;  cal.21;  cal.22;  cal.23;  cal.24
cal.25;	 cal.26;  cal.27;  cal.28;  cal.s0;  cff.z;   cif.z;   cii.z
cmf.s0;	 cmi.1W;  cmi.2W;  cmp.z;   cms.s0;  csa.1W;  csb.1W;  dec.z
dee.w0;	 del.w_1; dup.1W;  dvf.s0;  dvi.1W;  fil.l;   inc.z;   ine.lw
ine.w0;	 inl._1W; inl._2W; inl._3W; inl.w_1; inn.s0;  ior.1W;  ior.s0
lae.l;	 lae.w0;  lae.w1;  lae.w2;  lae.w3;  lae.w4;  lae.w5;  lae.w6



/------------------------------------------------------------------------------
/	now dispatch table for escaped opcodes
/------------------------------------------------------------------------------

dispae1:      /dispatch	escaped	opcodes	1

aar.l;	 aar.z;	  adf.l;   adf.z;   adi.l;   adi.z;   ads.l;   ads.z
adu.l;	 adu.z;	  and.l;   and.z;   asp.lw;  ass.l;   ass.z;   bge.l
bgt.l;	 ble.l;	  blm.l;   bls.l;   bls.z;   blt.l;   bne.l;   cai.z
cal.l;	 cfi.z;	  cfu.z;   ciu.z;   cmf.l;   cmf.z;   cmi.l;   cmi.z
cms.l;	 cms.z;	  cmu.l;   cmu.z;   com.l;   com.z;   csa.l;   csa.z
csb.l;	 csb.z;	  cuf.z;   cui.z;   cuu.z;   dee.lw;  del.pw;  del.nw
dup.l;	 dus.l;	  dus.z;   dvf.l;   dvf.z;   dvi.l;   dvi.z;   dvu.l
dvu.z;	 fef.l;	  fef.z;   fif.l;   fif.z;   inl.pw;  inl.nw;  inn.l
inn.z;	 ior.l;	  ior.z;   lar.l;   lar.z;   ldc.l;   ldf.l;   ldl.pw
ldl.nw;	 lfr.l;	  lil.pw;  lil.nw;  lim.z;   los.l;   los.z;   lor.s0
lpi.l;	 lxa.l;	  lxl.l;   mlf.l;   mlf.z;   mli.l;   mli.z;   mlu.l
mlu.z;	 mon.z;	  ngf.l;   ngf.z;   ngi.l;   ngi.z;   nop.z;   rck.l
rck.z;	 ret.l;	  rmi.l;   rmi.z;   rmu.l;   rmu.z;   rol.l;   rol.z
ror.l;	 ror.z;	  rtt.z;   sar.l;   sar.z;   sbf.l;   sbf.z;   sbi.l
sbi.z;	 sbs.l;	  sbs.z;   sbu.l;   sbu.z;   sde.l;   sdf.l;   sdl.pw
sdl.nw;	 set.l;	  set.z;   sig.z;   sil.pw;  sil.nw;  sim.z;   sli.l






sli.z;	 slu.l;	  slu.z;   sri.l;   sri.z;   sru.l;   sru.z;   sti.l
sts.l;	 sts.z;	  str.s0;  tge.z;   tle.z;   trp.z;   xor.l;   xor.z
zer.l;	 zer.z;	  zge.l;   zgt.l;   zle.l;   zlt.l;   zne.l;   zrf.l
zrf.z;	 zrl.pw;  dch.z;   exg.s0;  exg.l;   exg.z;   lpb.z;   gto.l

/------------------------------------------------------------------------------
/	timeinf	tables,	first the unescaped opcodes
/	these tables are parallel to the tables	dispat and dispae1
/	Each entry contains a reasonable estimate of
/	the number of memory-cycles needed to
/	execute	that instruction. The exact amount cannot be
/	supplied, since	this can depend	rather heavily on the
/	size of	the object in set, array case instructions etc.
/	The table timeinf also contains, added to each entry,
/	the number of memory-cycles needed to decode the instruction.
/	This number is currently 6. The	number is computed for
/	the case that all check	and runinf options are off.
/------------------------------------------------------------------------------
.if .prof
23.;	23.;	12.;	12.;	18.;	17.;	19.;	61.
11.;	31.;	21.;	15.;	20.;	30.;	30.;	31.
20.;	18.;	18.;	19.;	29.;	18.;	13.;	20.
10.;	14.;	13.;	27.;	20.;	20.;	20.;	20.
20.;	23.;	16.;	16.;	16.;	16.;	17.;	38.
14.;	26.;	26.;	26.;	26.;	28.;	26.;	25.
11.;	11.;	11.;	11.;	11.;	11.;	11.;	11.
11.;	11.;	11.;	11.;	16.;	16.;	26.;	24.

24.;	53.;	25.;	25.;	18.;	27.;	44.;	54.
30.;	59.;	53.;	21.;	28.;	19.;	51.;	18.
18.;	21.;	27.;	19.;	20.;	18.;	25.;	16.
16.;	15.;	12.;	24.;	24.;	24.;	24.;	25.
26.;	25.;	15.;	13.;	11.;	11.;	11.;	11.
11.;	16.;	14.;	14.;	14.;	14.;	20.;	16.
16.;	21.;	16.;	16.;	16.;	16.;	16.;	16.
26.;	16.;	10.;	10.;	15.;	24.;	10.;	40.

timeinf:

 9.;	10.;	10.;	10.;	10.;	10.;	10.;	10.
10.;	10.;	10.;	10.;	10.;	10.;	10.;	10.
10.;	10.;	10.;	10.;	10.;	10.;	10.;	10.
10.;	10.;	10.;	10.;	10.;	10.;	10.;	10.
10.;	10.;	48.;	53.;	21.;	28.;	20.;	10.
10.;	12.;	13.;	11.;	44.;	11.;	11.;	11.
11.;	11.;	27.;	21.;	17.;	17.;	17.;	17.
81.;	17.;	17.;	21.;	12.;	12.;	11.;	12.

54.;	54.;	54.;	54.;	54.;	54.;	54.;	54.
54.;	54.;	54.;	54.;	54.;	54.;	54.;	54.
54.;	54.;	54.;	54.;	54.;	54.;	54.;	54.
54.;	54.;	54.;	54.;	54.;	41.;	49.;	37.
40.;	53.;	53.;	51.;	60.;	24.;	41.;	11.
20.;	19.;	10.;	53.;	30.;	29.;	11.;	30.
20.;	15.;	15.;	15.;	19.;	44.;	37.;	36.
25.;	19.;	19.;	19.;	19.;	19.;	19.;	19.

/------------------------------------------------------------------------------
/	time2inf table for escaped opcodes
/	cycles necessary for decoding is already accounted for in timeinf
/------------------------------------------------------------------------------

time2inf:

57.;	46.;	61.;	50.;	37.;	26.;	30.;	19.
45.;	34.;	52.;	41.;	37.;	42.;	31.;	21.
21.;	21.;	91.;   108.;	97.;	21.;	21.;	53.
60.;	56.;	55.;	26.;	53.;	42.;	62.;	51.
72.;	61.;	72.;	61.;	38.;	27.;	40.;	29.
53.;	46.;	54.;	38.;	23.;	30.;	30.;	28.
36.;	45.;	34.;	61.;	50.;	39.;	28.;	44.
33.;	68.;	57.;	68.;	57.;	30.;	28.;	54.

45.;	44.;	33.;	70.;	59.;	22.;	27.;	28.
29.;	37.;	28.;	27.;	11.;	47.;	40.;	21.
20.;	35.;	33.;	61.;	50.;	34.;	23.;	39.
28.;   500.;	47.;	36.;	41.;	30.;   100.;	38.
27.;	62.;	39.;	28.;	44.;	33.;	88.;	77.
92.;	81.;	32.;	68.;	57.;	61.;	50.;	37.
26.;	33.;	22.;	45.;	34.;	29.;	28.;	30.
28.;	61.;	52.;	16.;	28.;	27.;	11.;	30.

19.;	36.;	25.;	32.;	21.;	36.;	25.;	31.
39.;	32.;	32.;	14.;	14.;   117.;	45.;	34.
31.;	22.;	20.;	20.;	20.;	20.;	20.;	27.
16.;	26.;	17.;	39.;	47.;	36.;	10.;	29.
.endif
	     .text
/------------------------------------------------------------------------------
/	LOAD CONSTANT, LOAD LOCAL, STORE LOCAL
/------------------------------------------------------------------------------

loc.0:	clr	-(sp)
	next
loc.1:	loc.2:	loc.3:	loc.4:	loc.5:	loc.6:	loc.7:	loc.8:
loc.9:	loc.10:	loc.11:	loc.12:	loc.13:	loc.14:	loc.15:	loc.16:
loc.17:	loc.18:	loc.19:	loc.20:	loc.21:	loc.22:	loc.23:	loc.24:
loc.25:	loc.26:	loc.27:	loc.28:	loc.29:	loc.30:	loc.31:	loc.32:
loc.33:
	asr	r0		/ make multiplication undone
	mov	r0,-(sp)
	next
loc._1:
	mov	$-1,-(sp)
	next
loc.s0:
	clr	r0
	br	2f
loc.s_1:
	mov	$-400,r0
2:	bisb	(pcx)+,r0
	mov	r0,-(sp)
	next
lpi.l:				/ let op, dit is een pointer
				/ zonder offset	op het moment!
loc.l:
	jsr	pc,wrdoff
	mov	r0,-(sp)
	next
ldc.0:
	clr	-(sp)
	clr	-(sp)
	next
ldc.l:
	jsr	pc,wrdoff
	mov	r0,-(sp)
	sxt    -(sp)
	next

loc.f:	jsr	pc,wrdoff;	mov	r0,r1
	jsr	pc,wrdoff;	mov	r0,-(sp)
	mov	r1,-(sp);	next

/__________________________________________________________________________

lol.0:		mov	010(r2),-(sp);	next
lol.1W:		mov	012(r2),-(sp);	next
lol.2W:		mov	014(r2),-(sp);	next
lol.3W:		mov	016(r2),-(sp);	next
lol._1W:	mov	-02(r2),-(sp);	next
lol._2W:	mov	-04(r2),-(sp);	next
lol._3W:	mov	-06(r2),-(sp);	next
lol._4W:	mov	-010(r2),-(sp);	next
lol._5W:	mov	-012(r2),-(sp);	next
lol._6W:	mov	-014(r2),-(sp);	next
lol._7W:	mov	-016(r2),-(sp);	next
lol._8W:	mov	-020(r2),-(sp);	next

lol.w0:		clr	r0;		bisb	(pcx)+,r0
5:		asl	r0;		add	r2,r0
		mov	010(r0),-(sp);	next
lol.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0
2:		asl	r0;		add	r2,r0
		mov	(r0),-(sp);	next
lol.pw:		jsr	pc,wrdoff;	br	5b
lol.nw:		jsr	pc,wrdoff;	br	2b

/------------------------------------------------------------------------------

ldl.0:		mov	10.(r2),-(sp);	mov	8.(r2),-(sp);	 next
ldl.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0
2:		asl	r0;		add	r2,r0
		mov	2(r0),-(sp);	mov	(r0),-(sp);    next
ldl.pw:		jsr	pc,wrdoff;	asl	r0
		add	r2,r0;		mov	10.(r0),-(sp)
		mov	8.(r0),-(sp);	next
ldl.nw:		jsr	pc,wrdoff;	br	2b

/------------------------------------------------------------------------------
loe.lw:		jsr	pc,wrdoff;	br	2f
loe.w0:		loe.w1:		loe.w2:		loe.w3:		loe.w4:
		asr	r0;		add	$0144,r0
		swab	r0;		bisb	(pcx)+,r0
2:		asl	r0;		add	eb,r0
		mov	(r0),-(sp);	next
lde.lw:		jsr	pc,wrdoff;	br	2f
lde.w0:		clr	r0;		bisb	(pcx)+,r0
2:		asl	r0;		add	eb,r0
		mov	2(r0),-(sp);	mov	(r0),-(sp);    next


/------------------------------------------------------------------------------
lil.0:		clr	r0;		br	1f
lil.1W:		mov	$1,r0;		br	1f
lil.pw:		jsr	pc,wrdoff;	br	1f
lil.w0:		clr	r0;		bisb	(pcx)+,r0
1:		add	$04,r0
2:		asl	r0;		add	r2,r0
		mov	(r0),-(sp);	jsr	pc,chckptr
		mov	*(sp),(sp);	next
lil.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0;    br      2b
lil.nw:		jsr	pc,wrdoff;	br     2b
/------------------------------------------------------------------------------
lof.l:		jsr	pc,wrdoff
1:		jsr	pc,chckptr;	add	(sp)+,r0;
		mov	(r0),-(sp);    next

lof.1W:		lof.2W:		lof.3W:		lof.4W:
		add	$0276,r0;
		br	1b
lof.s0:		clr	r0;		bisb	(pcx)+,r0;	br	1b
ldf.l:		jsr	pc,wrdoff;	add	(sp)+,r0
		mov	2(r0),-(sp);	mov	(r0),-(sp);	next
/------------------------------------------------------------------------------
lal.p:		jsr	pc,wrdoff
5:		add	r2,r0;		add	$8.,r0
		mov	r0,-(sp);	next
lal.0:		mov	r2,-(sp);	add	$8.,(sp);	next
lal.w0:		clr	r0;		bisb	(pcx)+,r0
		asl	r0;		br	5b
lal.n:		jsr	pc,wrdoff;	br	2f
lal._1:		mov	$-1,r0
2:		add	r2,r0;		mov	r0,-(sp);	next
lal.w_1:	mov	$-400,r0
3:		bisb	(pcx)+,r0;	asl	r0;		br	2b
lal.w_2:	mov	$-1000,r0;	br	3b

lae.l:		jsr	pc,wrdoff;	br	1f
lae.w0:	lae.w1:	lae.w2:	lae.w3:	lae.w4:	lae.w5:	lae.w6:
		asr	r0
		sub	$0171,r0;	swab	r0
		bisb	(pcx)+,r0;	asl	r0
1:		add	eb,r0;
.if .test
		cmp	globmax,r0;
		bhi	1f;		jsr	pc,e.badlae;
.endif
1:		mov	r0,-(sp);      next
/------------------------------------------------------------------------------
lxl.1:		mov	$1,r0;		br	1f
lxl.2:		mov	$2,r0;		br	1f
lxl.l:		jsr	pc,wrdoff
		bgt	1f;		jlt	e.oddz
		mov	r2,-(sp);	next
1:		mov	r2,r1
2:		mov	8(r1),r1;	sob	r0,2b
		mov	r1,-(sp);	next

lxa.1:		mov	$1,r0;		br	1f
lxa.l:		jsr	pc,wrdoff;	bgt	1f
		jlt	e.oddz;		mov	r2,-(sp)
		add	$10,(sp);	next
1:		mov	r2,r1
2:		mov	8(r1),r1;	sob	r0,2b
		add	$10,r1;		mov	 r1,-(sp);	 next

/------------------------------------------------------------------------------
loi.l:		jsr	pc,wrdoff;	br	2f
loi.1W:	loi.2W:	loi.3W:	loi.4W:
		add	$260,r0;	br	1f
loi.s0:		clr	r0;		bisb	(pcx)+,r0
2:		cmp	$1,r0;		beq	loi.1
1:		jsr	pc,chckptr;	mov	(sp)+,r1;	add	r0,r1
		asr	r0;		jcs	e.oddz
1:		mov	-(r1),-(sp);	sob	r0,1b;		next
loi.1:		jsr	pc,chckptb;	mov	(sp),r1;	clr	r0
		bisb	(r1),r0;	mov	r0,(sp);	next
los.z:
		mov	(sp)+,r0
		br	0f
los.l:
		jsr	pc,wrdoff
0:
		cmp	$04,r0
		beq	4f

		cmp	$02,r0;		beq	3f
		jbr	e.oddz
4:		mov	(sp)+,r0
3:		mov	(sp)+,r0;	br	2b

/------------------------------------------------------------------------------
			/Store group
/------------------------------------------------------------------------------
stl.pw:		jsr	pc,wrdoff;	asl	r0;	br    0f
stl.0:		clr	r0;		br	0f
stl.1W:		mov	$2,r0
0:		add	r2,r0;		mov(sp)+,8.(r0);	next

stl.nw:		jsr	pc,wrdoff;	br	0f
stl.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0
0:		asl	r0;		add	r2,r0
		mov	(sp)+,(r0);	next
stl._1W:	mov	(sp)+,-2(r2);	next
stl._2W:	mov	(sp)+,-4(r2);	next
stl._3W:	mov	(sp)+,-6(r2);	next
stl._4W:	mov	(sp)+,-10(r2);	next
stl._5W:	mov	(sp)+,-12(r2);	next

sdl.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0
0:		asl	r0;		add	r2,r0
2:		mov	(sp)+,(r0)+;	mov	(sp)+,(r0);	next
sdl.nw:		jsr	pc,wrdoff;	br	0b
sdl.pw:		jsr	pc,wrdoff;	asl	r0
		add	r2,r0;		add	$8.,r0;		br	2b

/------------------------------------------------------------------------------

sde.l:		jsr	pc,wrdoff;	add	eb,r0
		br	2b
ste.lw:		jsr	pc,wrdoff;	br	1f
ste.w0:		clr	r0;		br	0f
ste.w1:		mov	$400,r0;	br	0f
ste.w2:		mov	$1000,r0
0:		bisb	(pcx)+,r0
1:		asl	r0;		add	eb,r0
		mov	(sp)+,(r0);	next


/------------------------------------------------------------------------------

stf.l:		jsr	pc,wrdoff;	br	6f
stf.1W:		mov	$2,r0;		br	6f
stf.2W:		mov	$4,r0;		br	6f
stf.s0:		clr	r0;		bisb	(pcx)+,r0
6:		add	(sp)+,r0;	br	7f
sdf.l:		jsr	pc,wrdoff;	add	(sp)+,r0
		jbr	 2b


/------------------------------------------------------------------------------
sil.w0:		clr	r0;		bisb	(pcx)+,r0
5:		asl	r0;		add	r2,r0
		mov	8.(r0),r0;	br	7f
sil.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0
2:		asl	r0;		add	r2,r0
		mov	(r0),r0;
7:		mov	(sp),r1;	mov	r0,(sp);
		jsr	pc,chckptr;	mov	r1,*(sp)+;    next
sil.pw:		jsr	pc,wrdoff;	br	5b
sil.nw:		jsr	pc,wrdoff;	br	2b
/------------------------------------------------------------------------------
sti.1:		jsr	pc,chckptb;	mov	(sp)+,r1;
		movb	(sp)+,(r1);	next
sti.1W:		sti.2W:		sti.3W:		sti.4W:
		add	$114,r0;	br	1f
sti.s0:		clr	r0;		bisb	(pcx)+,r0;	br     1f
sti.l:		jsr	pc,wrdoff
1:		asr	r0;		beq	3f
		jcs	e.oddz;		jsr	pc,chckptr;
		mov	(sp)+,r1
2:		mov	(sp)+,(r1)+;	sob	r0,2b;		next
3:		jcs	sti.1;		jbr	e.oddz
sts.l:		jsr	pc,wrdoff
0:		cmp	$2,r0;		beq	2f
		cmp	$4,r0;		beq	4f;		jbr   e.oddz
4:		mov	(sp)+,r0
2:		mov	(sp)+,r0;	br	1b
sts.z:		mov	(sp)+,r0;	br	0b

/------------------------------------------------------------------------------
/		 POINTER ARITHMETIC
/------------------------------------------------------------------------------
adp.l:		jsr	pc,wrdoff;	add	r0,(sp);    next
adp.1:		add	$1,(sp);	next
adp.2:		add	$2,(sp);	next
adp.s0:		clr	r0;		bisb	(pcx)+,r0
		add	r0,(sp);	next
adp.s_1:	mov	$-400,r0;	bisb	(pcx)+,r0
		add	r0,(sp);	next
ads.l:		jsr	pc,wrdoff;	br	0f
ads.z:		mov	(sp)+,r0
0:		cmp	$1,r0;		beq	1f
		asr	r0;		jcs	e.oddz
2:		mov	(sp)+,r1;	sob	r0,2b
		add	r1,(sp);	next
ads.1W:		mov	(sp)+,r1;	add	r1,(sp);    next
1:		movb	(sp)+,r1
		add	r1,(sp);	next
sbs.l:		jsr	pc,wrdoff;	br	0f
sbs.z:		mov	(sp)+,r0
0:		mov	(sp)+,r1;	sub	r1,(sp)
		beq	0f;		mov	$-1,r1
		br	1f
0:		clr	r1
1:		dec	r0;		beq	3f
		dec	r0;		beq	2f
		asr	r0
4:		mov	r1,-(sp);	sob	r0,4b
2:		next
3:		clrb	1(sp);	next


/------------------------------------------------------------------------------
/------------------------------------------------------------------------------
/	Clears,	increments and decrements
/------------------------------------------------------------------------------
inc.z:		mov	sp,r1;
4:
.if .test
		cmp	(r1),$und;	jne	3f;
		jsr	pc,e.iund;			3:
.endif
		inc	(r1);		bvs	9f;	next
inl._1W:	mov	r2,r1;		sub	$2,r1;		br 4b
inl._2W:	mov	r2,r1;		sub	$4,r1;		br 4b
inl._3W:	mov	r2,r1;		sub	$6,r1;		br 4b
inl.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0;
1:		asl	r0;		mov	r2,r1;
		add	r0,r1;		br	4b
inl.pw:		jsr	pc,wrdoff;	add	$4,r0;
		br	1b;		/      !! proc frame 4	words
inl.nw:		jsr	pc,wrdoff;	br	1b
ine.lw:		jsr	pc,wrdoff;	br	1f
ine.w0:		clr	r0;		bisb	(pcx)+,r0;
1:		asl	r0;		add	eb,r0;
		mov	r0,r1;		br	4b
dec.z:		mov	sp,r1;
4:
.if .test
		cmp	(r1),$und;	jne	3f;
		jsr	pc,e.iund;			3:
.endif
		dec	(r1);		bvs	9f;	next
del.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0;
1:		asl	r0;		mov	r0,r1;
		add	r2,r1;		br	4b
del.pw:		jsr	pc,wrdoff;	add	$4,r0;
		br	1b;		/	!proc frame 4 words
del.nw:		jsr	pc,wrdoff;	br	1b
dee.w0:		clr	r0;		bisb	(pcx)+,r0;
1:		asl	r0;		add	eb,r0;
		mov	r0,r1;		br	4b
dee.lw:		jsr	pc,wrdoff;	br	1b;
9:		jsr	pc,e.iovfl;	next
		/ jump to an error routine for integer overflow
zrl._1W:	clr	-2(r2);		next
zrl._2W:	clr	-4(r2);		next
zrl.w_1:	mov	$-400,r0;	bisb	(pcx)+,r0;
1:		asl	r0;		add	r2,r0;
		clr	(r0);		next
zrl.nw:		jsr	pc,wrdoff;	br	1b
zrl.pw:		jsr	pc,wrdoff;	add	$4,r0;
		br	1b
zre.lw:		jsr	pc,wrdoff;	br	1f
zre.w0:		clr	r0;		bisb	(pcx)+,r0;
1:		asl	r0;		add	eb,r0;
		clr	(r0);		next
zrf.l:		jsr	pc,wrdoff;	br	1f
zrf.z:		mov	(sp)+,r0;
1:		asr	r0;
2:		clr	-(sp);		sob	r0,2b;		next
zer.s0:		clr	r0;		bisb	(pcx)+,r0;
3:		bit	$1,r0;		jne	e.illins
		/ test if number of bytes is even
		br	1b
zer.l:		jsr	pc,wrdoff;	br	3b
zer.z:		mov	(sp)+,r0;	br	3b
/------------------------------------------------------------------------------
/		LOGICAL	GROUP
/------------------------------------------------------------------------------

and.1W:		mov	$1,r1;		mov	$2,r0;
		br	lbland;
and.l:		jsr	pc,wrdoff;	br	0f
and.z:		mov	(sp)+,r0;
0:		ble	9f;		mov	r0,r1;
		asr	r1;		bcs	9f;
lbland:		add	sp,r0;
1:		mov	(sp)+,r5;	com	r5;
		bic	r5,(r0)+;	sob	r1,1b;
		next
ior.1W:		mov	$1,r1;		mov	$2,r0;
		br	0f
ior.s0:		clr	r0;		bisb	(pcx)+,r0;
		br	0f
ior.l:		jsr	pc,wrdoff;	br	0f
ior.z:		mov	(sp)+,r0;
lblior:

0:		ble	9f;		bit	$1,r0;
		bne	9f;		mov	r0,r1;
		mov	sp,r5;	add	r0,r5;	asr	r1;
1:		bis	(sp)+,(r5)+;	sob	r1,1b;		   next
xor.l:		jsr	pc,wrdoff;	br	0f;
xor.z:		mov	(sp)+,r0;
0:		ble	9f;		bit	$1,r0;
		bne	9f;		mov	r0,r1;
		mov	sp,r5;	add	r0,r5;	asr	r1
1:		mov	(sp)+,r0;
		xor	r0,(r5)+;    sob     r1,1b;		next
com.l:		jsr	pc,wrdoff;	br	1f
com.z:		mov	(sp)+,r0;
1:		bit	$1,r0;		bne	9f
		mov	r0,r1;		asr	r1
		add	sp,r0;
2:		com	-(r0);		sob	r1,2b
		next
rol.l:		jsr	pc,wrdoff;	br	3f
rol.z:		mov	(sp)+,r0;
3:		clr	r4;
		mov	(sp)+,r5;	ash	$3,r0;
		div	r0,r4;		mov	r5,r4;
		bge	1f;		add	r0,r4;
1:		ash	$-3,r0;		mov	sp,r1;
		cmp	r0,$1;		beq	1f;
		add	r0,r1;		mov	r1,r5;
		asr	r0;		jcs	9f
		mov	r3,saver0;	mov	r0,r3;
4:		mov	r3,r0;		mov	r5,r1;
2:		rol	-(r1);		sob	r0,2b;
		adc	-2(r5);		sob	r4,4b;
		mov	saver0,r3;	mov	$loop,r4;	next
1:		rolb	(r1)+;		adc	(r1);
		sob	r4,1b;		mov	saver1,r4;	next
ror.l:		jsr	pc,wrdoff;	neg	(sp);	br 3b
ror.z:		mov	(sp)+,r0;	neg	(sp);	br 3b
9:		jsr	 pc,e.oddz  /error codes for odd or
				    /negative number of	bytes

/------------------------------------------------------------------------------
/		SET GROUP
/------------------------------------------------------------------------------

set.s0:		clr	r0;		bisb	(pcx)+,r0
1:
.if .test
		bgt	9f;		jsr	pc,e.set
9:
.endif
		mov	(sp)+,r1
		jsr	pc,settest;	inc	r0
		asr	r0;	      /	if r0 odd choose next even
2:		clr	-(sp);		sob	r0,2b; / empty set
		mov	r1,r0;		ash	$-3,r0;
		add	sp,r0;		bic	$177770,r1;
		bisb	bits(r1),(r0);	next
set.l:		jsr	pc,wrdoff;	br	1b
set.z:		mov	(sp)+,r0;	br	1b
inn.s0:		clr	r0;		bisb	(pcx)+,r0
1:
.if .test
		bgt	9f;		jsr	pc,e.set
9:
.endif
		mov	sp,r5;
		add	r0,r5;		mov	(sp)+,r1;
		jsr	pc,settest;	mov	r1,r0
		ash	$-3,r0;		add	sp,r0;
		clr	-(sp);
		bic	$177770,r1;	   bitb	   bits(r1),(r0)
		beq	2f;		mov	r5,sp;
		mov	$1,(sp);	next
2:		mov	r5,sp;		clr	(sp);	 next
inn.l:		jsr	pc,wrdoff;	br	1b
inn.z:		mov	(sp)+,r0;	br	1b
	.data
bits:	.byte	1
	.byte	2
	.byte	4
	.byte	10
	.byte	20
	.byte	40
	.byte	100
	.byte	200
	.even
	.text
settest:	mov	r0,-(sp);	clc
		ash	$3,r0;		sub	r1,r0;
.if .test
		bgt	3f;		jsr	pc,e.set
.endif
3:		mov	(sp)+,r0;	rts	pc
/------------------------------------------------------------------------------
/		ARRAY	GROUP
/------------------------------------------------------------------------------



lar.1W:		mov	$2,r0;		br	1f
lar.l:		jsr	pc,wrdoff;	br	1f
lar.z:		mov	(sp)+,r0;
1:		jsr	pc,calcarr;	clr	-2(sp);
		sub	r5,sp;		bic	$1,sp;
		mov	sp,r0;
2:		movb	(r1)+,(r0)+;	sob	r5,2b;	next
sar.1W:		mov	$2,r0;		br	1f
sar.l:		jsr	pc,wrdoff;	br	1f
sar.z:		mov	(sp)+,r0;
1:		jsr	pc,calcarr;	mov	sp,r0;
		add	r5,sp;		inc	sp;
		bic	$1,sp;
2:		movb	(r0)+,(r1)+;	sob	r5,2b;	next
aar.1W:		mov	$2,r0;		br	1f
aar.l:		jsr	pc,wrdoff;	br	1f
aar.z:		mov	(sp)+,r0;
1:		jsr	pc,calcarr;	mov	r1,-(sp);	next

calcarr:	sub	$02,r0;		beq	0f;
		jsr	pc,e.oddz;
0:		tst	(sp)+;
		mov	(sp)+,r0;	mov	(sp)+,r1;
		sub	(r0)+,r1;	bge	9f
		jsr	pc,e.array
9:
		cmp	(r0)+,r1;	bge	9f
		jsr	pc,e.array
9:
		mov	(r0),r5;
		mul	r5,r1;		add	(sp)+,r1;
		mov	-010(sp),-(sp);	rts	pc;



/------------------------------------------------------------------------------
/--------------------------------------------------------------
/		CONVERT	GROUP
/--------------------------------------------------------------

cii.z:
		/ convert int to int
		/ 1 byte -> ? :	sign extension
	mov	(sp)+,r0
	inc	r0		/ dest 1 byte =	dest 1 word
	bic	$1,r0
.if .test
	cmp	(sp),$2		/ if size 2 then trap for undefined
	bne	7f
	cmp	2(sp),$und
	bne	7f
	jsr	pc,e.iund	   / this is the trap
7:
.endif
	sub	(sp)+,r0
0:	blt	1f
	asr	r0
	bcc	2f
	movb	(sp),r1
	mov	r1,(sp)
2:	tst	r0
	beq	3f
	tst	(sp)
4:	sxt	-(sp)
	sob	r0,4b
3:	next
1:	sub	r0,sp
.if .test
	mov	sp,r1
	neg	r0
	asr	r0
	tst	(sp)
	blt	3f
5:	tst	-(r1)
	bne	9f
	sob	r0,5b
	next
3:	cmp	-(r1),$-1
	bne	9f
	sob	r0,3b
.endif
	next
/-------
cui.z:	mov	(sp)+,r0
	sub	(sp)+,r0
	clr	-(sp)
	add	$-2,r0
	br	0b
cif.z:
	mov	(sp)+,r0
	jsr	pc,setfloat
	mov	(sp)+,r0
.if .test
	cmp	r0,$2		/ trap if size 2 undefined integer
	bne	7f
	cmp	(sp),$und
	bne	7f
	jsr	pc,e.iund	   / trap for undefined	integer
7:
.endif
	jsr	pc,setint
	movif	(sp)+,fr0
	movf	fr0,-(sp)
	next
cuf.z:
	mov	(sp)+,r0
	jsr	pc,setfloat
	mov	(sp)+,r0
	cmp	r0,$02
	bne	1b
	clr	-(sp)
	mov	$04,r0
	jsr	pc,setint
	movif	(sp)+,fr0
	cfcc
	bge	1f
	addf	fr3,fr0
1:	movf	fr0,-(sp)
	next
/-------
cff.z:
	mov	(sp)+,r0
	cmp	(sp)+,r0
	beq	1f
	jsr	pc,setfloat
	movof	(sp)+,fr0
	movf	fr0,-(sp)
1:	next
/-------
ciu.z:	mov	(sp)+,r0
.if .test
	cmp	(sp),$2		/ trap undefined of size 2
	bne	7f
	cmp	2(sp),$und
	bne	7f
	jsr	pc,e.iund	   / this is the trap
7:
.endif
	sub	(sp)+,r0
	asr	r0
	bcc	2f
	clrb	1(sp)
2:	tst	(sp)
.if .test
	jlt	9f
.endif
6:	tst	r0
	beq	3f
	blt	5f
4:	clr	-(sp)
	sob	r0,4b
3:	next
9:	jsr	pc,e.conv;	next
5:	neg	r0
4:	tst	(sp)+
.if .test
	jne	9b
.endif
	sob	r0,4b
	next
cuu.z:
	mov	(sp)+,r0
	sub	(sp)+,r0
	asr	r0
	jbr	6b
/-------
cfu.z:
	mov	(sp)+,r0
	jsr	pc,setint
	mov	(sp)+,r0
	jsr	pc,setfloat
	movf	(sp)+,fr0
	movfi	fr0,-(sp)
.if .test
	jcs	9b
	jlt	9b
.endif
	next
/-------
cfi.z:
	mov	(sp)+,r0
	jsr	pc,setint
	mov	(sp)+,r0
	jsr	pc,setfloat
	movf	(sp)+,fr0
	movfi	fr0,-(sp)
	jcs	e.conv
	next
/--------------------------------------------------------------
/		INTEGER	ARITHMETIC
/--------------------------------------------------------------

adi.l:	jsr	pc,wrdoff;	br	0f
adi.z:	mov	(sp)+,r0
0:	cmp	r0,$04
	bgt	1f
	cmp	r0,$02
	bgt	2f
	bne	1f
adi.1W:
.if .test
	cmp	(sp),$und	/ trap undefineds of size 2
	beq	6f
	cmp	2(sp),$und
	bne	7f
6:	jsr	pc,e.iund	   / this is the trap
7:
.endif
	add	(sp)+,(sp)
.if .test
	bvs	9f
.endif
	next
adi.2W:	2:	add	(sp)+,02(sp)
.if .test
	bvc	2f
	jsr	pc,e.iovfl
2:
.endif
	add	(sp)+,02(sp)
	adc	(sp)
.if .test
	bvs	9f
.endif

	next
1:
	jsr    pc,e.oddz ;	next
/-------
sbi.l:	jsr	pc,wrdoff;	br	0f
sbi.z:	mov	(sp)+,r0
0:	cmp	r0,$04
	bgt	1b
	cmp	r0,$02
	bgt	2f
	bne	1b
sbi.1W:
.if .test
	cmp	(sp),$und	/ trap for size	2 undefineds
	beq	6f
	cmp	2(sp),$und
	bne	7f
6:	jsr	pc,e.iund	   / this is the trap
7:
.endif
	sub	(sp)+,(sp)
.if .test
	bvs	9f
.endif
	next
sbi.2W:	2:		sub	(sp)+,02(sp)
.if .test
	bvc	2f
	jsr	pc,e.iovfl
2:
.endif
	sub	(sp)+,02(sp)
	sbc	(sp)
.if .test
	bvs	9f
	next
9:	jsr	pc,e.iovfl
.endif
	next
/------
mli.l:	 jsr	pc,wrdoff;	br	0f
mli.z:	 mov	(sp)+,r0
0:
	cmp	r0,$04
	bgt	1f
	beq	mli4
	cmp	r0,$02
	bne	1f
mli.1W:	mov	(sp)+,r1
.if .test
	cmp	r1,$und		/ trap for undefineds of size 2
	beq	6f
	cmp	(sp),$und
	bne	7f
6:	jsr	pc,e.iund	   / this is the trap
7:
.endif
	mul	(sp)+,r1
.if .test

	bcc	9f		/ overflow
	jsr	pc,e.iovfl
9:
.endif
	mov	r1,-(sp)
	next
1:	jmp	e.oddz
/------
mli.2W:		mli4:
.if .prof
	add	$91.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	tst	02(sp)
	sxt	r0
	sub	(sp),r0
	tst	06(sp)
	sxt	r2
	sub	04(sp),r2
	mov	r0,r4
	mul	r2,r4
	mul	06(sp),r0
.if .test
	bge	2f
	inc	r4
2:
.endif
	mul	02(sp),r2
.if .test
	bge	2f
	inc	r4
2:	sub	r2,r5
	sbc	r4
	sub	r0,r5
	sbc	r4
	add	r1,r3
	sbc	r5
	sbc	r4
.endif
	mov	02(sp),r0
	mul	06(sp),r0
.if .test
	bge	2f
	sub	$1,r5
	sbc	r4
.endif
2:	sub	r3,r0
.if .test
	sxt	r2
	sbc	r5
	sbc	r4
	cmp	r2,r4
	bne	2f
	cmp	r2,r5
	beq	9f
2:	jsr	pc,e.iovfl
9:
.endif
	add	$010,sp
	mov	r1,-(sp);
	mov	r0,-(sp);
	jsr	pc,regretu;	     next


/-------
dvi.l:	jsr    pc,wrdoff;      br      0f
dvi.z:	mov    (sp)+,r0
0:	cmp	r0,$04
	bgt	1f
	beq	dvi4
	cmp	r0,$02
	bne	1f
dvi.1W:	mov	02(sp),r1
	sxt	r0
.if .test
	cmp	r1,$und		/ trap for undifined of	size 2
	beq	6f
	cmp	(sp),$und
	bne	7f
6:	jsr	pc,e.iund	   / this is the trap
7:
.endif
	div	(sp)+,r0
	jcs	9f
	mov	r0,(sp)
	next
1:	jmp	e.oddz
/-------
dvi4:
.if .prof
	add	$100.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	mov	02(sp),r3
	bne	1f
	tst	(sp)
	bne	1f
9:	jsr	pc,e.idivz
1:	sxt	r4
	bpl	1f
	neg	r3
1:	cmp	r4,(sp)
	bne	hardldiv
	mov	06(sp),r2
	mov	04(sp),r1
	bge	2f
	neg	r1
	neg	r2
	sbc	r1
	com	r4
2:	mov	r4,-(sp)
	clr	r0
	div	r3,r0
	mov	r0,-(sp)
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	3f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	sxt	r1
	add	r1,r0
3:	mov	r0,r1
	mov	(sp)+,r0
	br	4f
hardldiv:
.if .prof
	add	$75.,*loprof
	adc	*hiprof
.endif
	clr	-(sp)
	mov	010(sp),r2
	mov	06(sp),r1
	bpl	5f
	com	(sp)
	neg	r1
	neg	r2
	sbc	r1
5:	clr	r0
	mov	02(sp),r3
	bge	6f
	neg	r3
	neg	04(sp)
	sbc	r3
	com	(sp)
6:	mov	$16.,r4
9:	clc
	rol	r2
	rol	r1
	rol	r0
	cmp	r3,r0
	bhi	7f
	bcs	8f
	cmp	04(sp),r1
	blos	8f
7:	sob	r4,9b
	br	1f
8:	sub	04(sp),r1
	sbc	r0
	sub	r3,r0
	inc	r2
	sob	r4,9b
1:
	mov	r2,r1
	clr	r0
4:	tst	(sp)+
	beq	1f
	neg	r0
	neg	r1
	sbc	r0
1:	add	$010,sp
	mov	r1,-(sp);
	mov	r0,-(sp);
	jsr	pc,regretu;	     next
/-------
rmi.l:	jsr	pc,wrdoff;	br	0f
rmi.z:	mov	(sp)+,r0
0:	cmp	r0,$04
	bgt	1f
	beq	rmi4
	cmp	r0,$02
	bne	1f
rmi.1W:	mov	02(sp),r1
	sxt	r0
.if .test
	cmp	r1,$und		/ trap for undefineds of size 2
	beq	6f
	cmp	(sp),$und
	bne	7f
6:	jsr	pc,e.iund	   / this is the trap
7:
.endif
	div	(sp)+,r0
	bcs	9f
	mov	r1,(sp)
	next
1:	jmp	e.oddz
/-------
rmi4:
.if .prof
	add	$100.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	mov	02(sp),r3
	bne	1f
	tst	(sp)
	bne	1f
9:	jsr	pc,e.idivz
1:	tst	r3
	sxt	r4
	bpl	1f
	neg	r3
1:	cmp	r4,(sp)
	bne	hardrmi4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	r1,r4
	bge	2f
	neg	r1
	neg	r2
	sbc	r1
2:	mov	r4,-(sp)
	clr	r0
	div	r3,r0
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	3f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	beq	3f
	add	r3,r1
3:	tst	(sp)+
	bpl	4f
	neg	r1
4:	sxt	r0
	br	9f
hardrmi4:
.if .prof
	add	$75.,*loprof
	adc	*hiprof
.endif
	mov	06(sp),r2
	mov	04(sp),r1
	bpl	5f
	neg	r1
	neg	r2
	sbc	r1
5:	clr	r0
	mov	(sp),r3
	bge	6f
	neg	r3
	neg	02(sp)
	sbc	r3
6:	mov	$16.,r4
1:	clc
	rol	r2
	rol	r1
	rol	r0
	cmp	r3,r0
	bhi	7f
	bcs	8f
	cmp	02(sp),r1
	blos	8f
7:	sob	r4,1b
	br	2f
8:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sob	r4,1b
2:	tst	04(sp)
	bge	9f
	neg	r0
	neg	r1
	sbc	r0
9:	add	$010,sp
	mov	r1,-(sp)
	mov	r0,-(sp)
	jsr	pc,regretu;	     next
/-------
ngi.l:	jsr	pc,wrdoff;	br	1f
ngi.z:	mov	(sp)+,r0
1:
lblngi:
	cmp	r0,$02
	bgt	1f
.if .test
	cmp	(sp),$und	/ trap for undefineds of size 2
	bne	7f
	jsr	pc,e.iund
7:
.endif
	neg	(sp)
3:	next
1:	cmp	r0,$04
	bgt	2f
	mov	(sp),r0
	neg	(sp)
	mov	02(sp),r1
	neg	02(sp)
	sbc	(sp)
	cmp	r0,(sp)
.if .test
	bne	3b
	cmp	r1,02(sp)
	bne	3b
2:	jsr	pc,e.iovfl
.endif
	next
/-------
sli.l:	jsr	pc,wrdoff;	br	0f
sli.z:	mov	(sp)+,r0
0:	cmp	r0,$02
	bgt	1f
sli.1W:	mov	(sp)+,r1
	mov	(sp)+,r0
.if .test
	cmp	r0,$und		/ trap for undefined size 2
	bne	7f
	jsr	pc,e.iund
7:
.endif
	ash	r1,r0
.if .test
	bvc	7f
	jsr	pc,e.iovfl
7:
.endif
	mov	r0,-(sp)
	next
1:	cmp	r0,$04
	bgt	2f
	mov	02(sp),r0
	mov	04(sp),r1
	ashc	(sp)+,r0
.if .test
	bvc	7f
	jsr	pc,e.iovfl
7:
.endif
	mov	r0,(sp)
	mov	r1,02(sp)
	next
2:	jmp	e.oddz
/-------
sri.l:	jsr	pc,wrdoff;	br	0f
sri.z:	mov	(sp)+,r0
0:	cmp	r0,$02
	bgt	1f
	mov	(sp)+,r1
	mov	(sp)+,r0
.if .test
	cmp	r0,$und		/ trap for undefined size 2
	bne	7f
	jsr	pc,e.iund
7:
.endif
	neg	r1
	ash	r1,r0
	mov	r0,-(sp)
	next
1:	cmp	r0,$04
	bgt	2f
	mov	02(sp),r0
	mov	04(sp),r1
	neg	(sp)
	ashc	(sp)+,r0
	mov	r0,(sp)
	mov	r1,02(sp)
	next
2:	jmp	e.oddz
/--------------------------------------------------------------
adu.l:	jsr	pc,wrdoff;	br	0f
adu.z:	mov	(sp)+,r0
0:	jsr	pc,tstr0;	add	r0,sp
	mov	sp,r1;		add	r0,sp;		asr	r0
2:	adc	-(sp);		add	-(r1),(sp);	sob r0,2b
	next
sbu.l:	jsr	pc,wrdoff;	br	0f
sbu.z:	mov	(sp)+,r0
0:	jsr	pc,tstr0;	add	r0,sp
	mov	sp,r1;		add	r0,sp;		asr	r0;
2:	sbc	-(sp);		sub	-(r1),(sp);	sob r0,2b
	next
mlu.l:	jsr	pc,wrdoff;	br	0f
mlu.z:	mov	(sp)+,r0
0:	jsr	pc,tstr0
	cmp	r0,$04
	bgt	1f
	beq	mlu4
	mov	(sp)+,r1
	mul	(sp)+,r1
	mov	r1,-(sp)
	next
1:	jmp	e.oddz
mlu4:
.if .prof
	add	$90.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	clr	r0
	mov	02(sp),r1
	mov	06(sp),r3
	mul	r3,r0
	tst	r3
	bge	1f
	ashc	$15.,r0
1:	mov	02(sp),r3
	clr	r2
	mul	04(sp),r2
	add	r3,r0
	mov	06(sp),r3
	clr	r2
	mul	(sp),r2
	add	r3,r0
	add	$010,sp
	mov	r1,-(sp)
	mov	r0,-(sp)
	jsr	pc,regretu;	     next
9:	jmp	e.oddz
/-------
dvu.l:	jsr	pc,wrdoff;	br	0f
dvu.z:	mov	(sp)+,r0
0:
	clr	saver0;
	cmp	r0,$04
	bgt	9b
	beq	dvu4
	clr	r0
	mov	02(sp),r1
	tst	(sp)
	blt	1f
	div	(sp)+,r0
	mov	r0,(sp);	next
1:	mov	(sp),-(sp);
	clr	02(sp);
	clr	-(sp);
	mov	$1,saver0;
dvu4:
.if .prof
	add	$95.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	clr	r0
	tst	(sp)
	bne	harddvu4
	tst	02(sp)
	blt	harddvu4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	02(sp),r3
	div	r3,r0
	mov	r0,-(sp)
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	1f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	sxt	r1
	add	r1,r0
1:	mov	r0,r1
	mov	(sp)+,r0
	br	2f
harddvu4:
.if .prof
	add	$75.,*loprof
	adc	*hiprof
.endif
	mov	06(sp),r2
	mov	04(sp),r1
	mov	(sp),r3
	mov	$17.,r4
	br	3f
6:	rol	r2
	rol	r1
	rol	r0
3:	cmp	r3,r0
	bhi	4f
	blo	5f
	cmp	02(sp),r1
	blos	5f
4:	clc
	sob	r4,6b
	br	7f
5:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sec
	sob	r4,6b
7:	rol	r2
	bcc	8f
	mov	$01,r0
	br	9f
8:	clr	r0
9:	mov	r2,r1
2:	add	$010,sp
	mov	r1,-(sp)
	mov	r0,-(sp)
	jsr	pc,regretu
	tst	saver0;
	beq	2f;
	tst	(sp)+
2:
	  next
/-------
9:	jbr	e.oddz
rmu.l:	jsr	pc,wrdoff;	br	0f
rmu.z:	mov	(sp)+,r0
0:	clr	saver0;
	cmp	r0,$04
	bgt	9b
	beq	rmu4
	cmp	r0,$02
	bne	9b
	mov	$1,saver0;
	mov	02(sp),r1
	tst	(sp)
	blt	1f
	clr	r0
	div	(sp)+,r0
	mov	r1,(sp);		next
1:	mov	(sp),-(sp)
	clr	02(sp)
	clr	-(sp)
rmu4:
.if .prof
	add	$95.,*loprof
	adc	*hiprof
.endif
	jsr	pc,regsave
	clr	r0
	tst	(sp)
	bne	hardrmu4
	tst	02(sp)
	blt	hardrmu4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	02(sp),r3
	div	r3,r0
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	1f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	beq	1f
	add	r3,r1
1:	clr	r0
	br	2f
hardrmu4:
.if .prof
	add	$75.,*loprof
	adc	*hiprof
.endif
	mov	06(sp),r2
	mov	04(sp),r1
	mov	(sp),r3
	mov	$17.,r4
	br	3f
6:	clc
	rol	r2
	rol	r1
	rol	r0
3:	cmp	r3,r0
	bhi	4f
	bcs	5f
	cmp	02(sp),r1
	blos	5f
4:	sob	r4,6b
	br	2f
5:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sob	r4,6b
2:	add	$010,sp
	mov	r1,-(sp)
	mov	r0,-(sp)
	jsr	pc,regretu
	tst	saver0
	beq	2f;
	tst	(sp)+;
2:	  next
/-------
slu.l:	jsr	pc,wrdoff;	br	0f
slu.z:	mov	(sp)+,r0
0:
	cmp	r0,$02
	bgt	1f
	mov	(sp)+,r1
	mov	(sp)+,r0
	ash	r1,r0
	mov	r0,-(sp)
	next
1:	cmp	r0,$04
	bgt	2f
	mov	02(sp),r0
	mov	04(sp),r1
	ashc	(sp)+,r0
	mov	r0,(sp)
	mov	r1,02(sp)
	next
2:	jmp	e.oddz
/-------
sru.l:	jsr	pc,wrdoff;	br	0f
sru.z:	mov	(sp)+,r0
0:
	cmp	r0,$02
	bgt	1f
	mov	2(sp),r1
	neg	(sp)
	clr	r0
	ashc	(sp)+,r0
2:	mov	r1,-(sp)
	next
1:	cmp	r0,$04
	bgt	3f
	mov	02(sp),r0
	mov	04(sp),r1
	neg	(sp)
	beq	4f
	ashc	$-1,r0
	bic	$0100000,r0
	inc	(sp)
	beq	4f
	ashc	(sp)+,r0
4:	mov	r0,(sp)
	mov	r1,02(sp)
	next
3:	jmp	e.oddz
/-------

/--------------------------------------------------------------
		/ FLOATING POINT INSTRUCTIONS
/--------------------------------------------------------------

adf.s0:	clr	r0;		bisb	(pcx)+,r0; br	   0f
adf.l:	jsr	pc,wrdoff;	br	0f
adf.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp)+,fr0
	addf	(sp)+,fr0
	movf	fr0,-(sp)
	next
/-------

sbf.s0:	clr	r0;		bisb	(pcx)+,r0; br	   0f
sbf.l:	jsr	pc,wrdoff;	br	0f
sbf.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp)+,fr0
	subf	(sp)+,fr0
	negf	fr0
	movf	fr0,-(sp)
	next
/-------

mlf.s0:	clr	r0;		bisb	(pcx)+,r0; br	   0f
mlf.l:	jsr	pc,wrdoff;	br	0f
mlf.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp)+,fr0
	mulf	(sp)+,fr0
	movf	fr0,-(sp)
	next
/-------

dvf.s0:	clr	r0;		bisb	(pcx)+,r0; br	   0f
dvf.l:	jsr	pc,wrdoff;	br	0f
dvf.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp)+,fr0
	movf	(sp)+,fr1
	divf	fr0,r1
	movf	fr1,-(sp)
	next
/-------

ngf.l:	jsr	pc,wrdoff;	br	0f
ngf.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	negf	(sp)
	next
/-------

fif.l:	jsr	pc,wrdoff;	br	0f
fif.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp)+,fr0
	modf	(sp)+,fr0
	movf	fr0,-(sp)
	movf	fr1,-(sp)
	next
/-------

fef.l:	jsr	pc,wrdoff;	br	0f
fef.z:	mov	(sp)+,r0
0:
	jsr	pc,setfloat
	movf	(sp),fr0
	movei	fr0,-(sp)
	movie	$0,fr0
	movf	fr0,02(sp)
	next
/----------------------------------------------------------------------------
/	TEST AND BRANCH	GROUP
/----------------------------------------------------------------------------
tlt.z:	tst	(sp)+; blt     true;   clr     -(sp); next
tle.z:	tst	(sp)+; ble     true;   clr     -(sp); next
teq.z:	tst	(sp)+; beq     true;   clr     -(sp); next
tne.z:	tst	(sp)+; bne     true;   clr     -(sp); next
tge.z:	tst	(sp)+; bge     true;   clr     -(sp); next
tgt.z:	tst	(sp)+; bgt     true;   clr     -(sp); next
/----------------------------------------------------------------------------
zlt.s0:		tst	(sp)+; blt     yesbr2; br      nobr2
zlt.l:		tst	(sp)+; blt     yesbr3; br      nobr3
zle.s0:		tst	(sp)+; ble     yesbr2; br      nobr2
zle.l:		tst	(sp)+; ble     yesbr3; br      nobr3
zeq.s0:		tst	(sp)+; beq     yesbr2; br      nobr2
zeq.s1:		tst	(sp)+; beq     yesbr4; br      nobr2
zeq.l:		tst	(sp)+; beq     yesbr3; br      nobr3
zne.s0:		tst	(sp)+; bne     yesbr2; br      nobr2
zne.l:		tst	(sp)+; bne     yesbr3; br      nobr3
zne.s_1:	tst	(sp)+; bne     yesbr5; br      nobr2
zge.s0:		tst	(sp)+; bge     yesbr2; br      nobr2
zge.l:		tst	(sp)+; bge     yesbr3; br      nobr3
zgt.s0:		tst	(sp)+; bgt     yesbr2; br      nobr2
zgt.l:		tst	(sp)+; bgt     yesbr3; br      nobr3
great:
true:	mov	$1,-(sp)
	next

/------------------------------------------------------------------------------

blt.s0:	cmp	(sp)+,(sp)+;  bgt     yesbr2; br      nobr2
blt.l:	cmp	(sp)+,(sp)+;  bgt     yesbr3; br      nobr3
ble.s0:	cmp	(sp)+,(sp)+;  bge     yesbr2; br      nobr2
ble.l:	cmp	(sp)+,(sp)+;  bge     yesbr3; br      nobr3
beq.l:	cmp	(sp)+,(sp)+;  beq     yesbr3; br      nobr3
beq.s0:	cmp	(sp)+,(sp)+;  beq     yesbr2; br      nobr2
bne.s0:	cmp	(sp)+,(sp)+;  bne     yesbr2; br      nobr2
bne.l:	cmp	(sp)+,(sp)+;  bne     yesbr3; br      nobr3
bge.s0:	cmp	(sp)+,(sp)+;  ble     yesbr2; br      nobr2
bge.l:	cmp	(sp)+,(sp)+;  ble     yesbr3; br      nobr3
bgt.s0:	cmp	(sp)+,(sp)+;  blt     yesbr2; br      nobr2
bgt.l:	cmp	(sp)+,(sp)+;  blt     yesbr3; br      nobr3

bra.s0:	yesbr2:
	clr	r0;
5:	bisb	(pcx)+,r0
1:	add	r0,pcx
	next
bra.l:	yesbr3:
	jsr	pc,wrdoff
	br	1b
bra.s1:		yesbr4:		mov	$400,r0;	br 5b
bra.s_1:	yesbr5:		mov	$-400,r0;	br 5b
bra.s_2:	mov	$-800,r0;	br 5b
nobr2:	tstb	(pcx)+
	next
nobr3:	cmpb	(pcx)+,(pcx)+
	next
/
/------------------------------------------------------------------------------
/	Compare	group
/------------------------------------------------------------------------------

calccomp:	mov	(sp)+,saver1;		/ old pc
		mov	sp,r1;
		add	r0,r1;		mov	r1,r5;
		add	r0,r1;		mov	r1,saver0;   / new sp
		mov	r0,r1;		asr	r1;
2:		cmp	(r5)+,(sp)+;	blt	3f;
		bgt	4f;		dec	r1;
		beq	5f
2:		cmp	(r5)+,(sp)+;	blo	3f;
		bhi	4f;		sob	r1,2b;
5:		mov	saver0,sp;	clr    -(sp);
		br	5f
4:		mov	saver0,sp;	mov	$1,-(sp);
		br	5f
3:		mov	saver0,sp;	mov	$-1,-(sp);
		br	5f
5:		mov	saver1,-(sp);	rts  pc

cmi.1W:		mov	$2,r0;		br	1f
cmi.2W:		mov	$4,r0;		br	1f
cmi.l:		jsr	pc,wrdoff;	br	1f
cmi.z:		mov	(sp)+,r0;
1:		jsr	pc,calccomp;	next
cms.s0:		clr	r0;		bisb	(pcx)+,r0
1:		jsr	pc,calccomp;	tst	(sp)+;
		bne	great;		clr	-(sp);		next
cms.l:		jsr	pc,wrdoff;	br	1b
cms.z:		mov	(sp)+,r0;	br	1b
cmu.l:		jsr	pc,wrdoff;	br	1f
cmu.z:		mov	(sp)+,r0;	br	1f
cmp.z:		mov	$2,r0;
1:		jsr	pc,calccomp;	mov	(sp)+,r1;
		jlo	less;		jhi	great;
		clr	-(sp);		next
cmf.s0:		clr	r0;		bisb	(pcx)+,r0;
1:		cmp	$4,r0;		beq	3f
		cmp	$8,r0;		beq	3f
		jsr	pc,e.oddz;
3:		jsr	pc,setfloat
		movf	(sp)+,fr0;	cmpf	(sp)+,fr0;
4:		cfcc;	jlt	less;	jgt	great;
		clr	-(sp);		next
cmf.l:		jsr	pc,wrdoff;	br	1b
cmf.z:		mov	(sp)+,r0;	br	1b
less:		mov	$-1,-(sp);	next

/------------------------------------------------------------------------------
/------------------------------------------------------------------------------
/------------------------------------------------------------------------------
/	call and return	group
/------------------------------------------------------------------------------
/
/	Frame format on	the stack is:
/
/	| Parameter 1		  |
/	| - - -	- - - -	- - - -	- |
/	| Static link =	param 0	  | < AB
/	|_________________________|	 ____
/	|-------------------------|	     |	P
/	| Saved	line number	  |	     |	R
/	|-------------------------|	     |	O
/	| Saved	file address	  |	     |	C
/	|-------------------------|	     |	F
/	| Return address	  |	     |	R
/	|-------------------------|	     |	A
/	| Dynamic link		  | < LB     |	M
/	|_________________________|	 ____|	E
/	|-------------------------|
/	|			  |
/	| local	variable -1	  |
/	|-------------------------|
/	|			  |
/
/
/	The static link	and the	parameters are set by the
/	calling	procedure; the frame is	set by cal which reserves
/	also the space for local variables.
/------------------------------------------------------------------------------

cal.1:	cal.2:	cal.3:	cal.4:	cal.5:	cal.6:	cal.7:	cal.8:
cal.9:	cal.10:	cal.11:	cal.12:	cal.13:	cal.14:	cal.15:	cal.16:
cal.17:	cal.18:	cal.19:	cal.20:	cal.21:	cal.22:	cal.23:	cal.24:
cal.25:	cal.26:	cal.27:	cal.28:

	asr	r0;		sub	$077,r0;	br  1f
cal.s0:	clr	r0;		bisb	(pcx)+,r0;	br  1f
cai.z:	mov	(sp)+,r0;	br	1f
cal.l:	jsr	pc,wrdoff
lblcal:
1:
	cmp	nprocs,r0
	jlos	e.badpc
1:	asl	r0;		asl	r0;
.if .flow + .count + .prof
	mov	r0,r5;
	asl	r5
	add	r5,r0		/ procdes 12 bytes in this case
.endif
	add	pd,r0;		/ now r0 points	to the bottom
				/ of the proc descriptor
	mov	*eb,-(sp)	/ save line number
	mov	*filb,-(sp)	/ save file address

.if .prof + .count + .flow
	tst	10.(r0)		/ yet a	filename defined in this proc?
	bne	4f		/ yes? continue
	mov	(sp),10.(r0)	/ no? Then take	the old	filename
4:	mov	curproc,(sp)	/ save old procdescriptor
	mov	r0,curproc	/ continue with	the new	one
	mov	4(r0),countfld	/ count	pointer	minus line number
	sub	6(r0),countfld	/ to add at line numbers
.endif
	mov	pcx,-(sp)	/ return address
	mov	r2,-(sp)	/ old local base
	mov	sp,r2		/ new local base
	mov	(r0)+,r1	/ number of bytes for locals
	beq	3f		/ no locals
	inc	r1;	asr  r1	/ make even
2:	mov	$und,-(sp)	/ make undefined
	sob	r1,2b		/ for r1 words local
3:	mov	(r0),pcx	/ em start address
	next			/ ready
/------------------------------------------------------------------------------

ret.l:	jsr	pc,wrdoff;	br	1f
ret.1W:	mov	$2,r0;		br	1f
ret.s0:	clr	r0;		bisb	(pcx)+,r0;
	jlt	e.oddz		/ bad	result size
	beq	ret.0		/ no result
	cmp	$32.,r0;	bge	1f;	jsr	pc,e.badlfr
1:	inc	r0;	asr r0	/ make even
	mov	r0,retsize	/ result size
	mov	$retarea,r1	/ must point to	retarea
2:	mov	(sp)+,(r1)+	/ load result
	sob	r0,2b		/ continue when	there is more
	br	3f
retcount:
ret.0:	clr	retsize		/ no function result
3:	mov	r2,sp		/ set sp to bottom frame
	mov	(sp)+,r2	/ restore (dynamic) local base
	jeq	lblhalt;	/ if zero then main procedure returns
	mov	(sp)+,pcx	/ return address in program counter
	mov	eb,r1		/ must point to	external base
	mov	(sp)+,r0	/ file or procdesbase in r0
.if .count + .flow + .prof
	mov	r0,curproc	/ save procdesbase current proc
	mov	4(r0),countfld	/ restore pointer to
	beq	5f		/ countfield 0 non interesting
	sub	6(r0),countfld	/ table	entries
	mov	countfld,countptr
	add	(sp),countptr
.if .prof
	tst	(sp)
	beq	5f
	mov	countptr,r5
	asl	r5
	asl	r5
	add	ltime,r5
	mov	r5,hiprof
	add	$2,r5
	mov	r5,loprof
.endif
5:
	mov	10.(r0),r0	/ pointer to file name in r0
.endif
retlast:
.if .last
	tst	4(r1)		/ file 0 not stacked
	beq	4f
	cmp	r0,4(r1)	/ same file name?
	beq	4f		/ yes continue
	jsr	pc,nexttab	/ next line table entry	in r5
	clr	(r5)		/ 0 indicates file pointer follows
	jsr	pc,nexttab	/ next entry in	r5
	mov	4(r1),(r5)	/ store	pointer	to file	name
5:	mov	(sp),(r1)	/ restored line	number in *eb
	beq	4f		/ do not put 0 in last table
	jsr	pc,nexttab	/ next entry in	r5
	mov	(sp),(r5)	/ line number in table
4:	cmp	(sp),(r1)	/ line number different?
	bne	5b		/ yes? put it in table
.endif
	mov	r0,4(r1)	/ old file address
	mov	(sp)+,(r1)	/ reset	line number
	next			/ main loop again

lfr.l:	jsr	pc,wrdoff;
8:	bit	$1,r0;		jne	e.illins
	cmp	r0,retsize;	jeq	7f;	jsr	pc,e.badlfr
7:	asr	r0;				 / number of words
	tst	r0;		beq	0f;
	mov	$retarea,r1;	/ must point to	retarea
	add	r0,r1;
	add	r0,r1;		/ top of result
2:	mov	-(r1),-(sp);	/ move word of result
	sob	r0,2b;		/ look for more
0:	next
lfr.1W:	mov	$2,r0;		br	8b
lfr.2W:	mov	$4,r0;		br	8b
lfr.s0:	clr	r0;		bisb	(pcx)+,r0;	br	8b
/
/------------------------------------------------------------------------------

/	miscellaneous
/------------------------------------------------------------------------------
exg.s0:	clr	r0;		bisb	(pcx)+,r0;	br	1f
exg.l:	jsr	pc,wrdoff;	br	1f
exg.z:	mov	(sp)+,r0;
1:	cmp	r0,$1;		beq	0f;
	blt	9f;		bit	$1,r0;
	bne	9f;
	add	r0,sp;
	mov	r0,r1;		asr	r1;
	add	sp,r0;
2:	mov	-(sp),r5;	mov	-(r0),(sp);
	mov	r5,(r0);	sob	r1,2b;
	next
0:	swab	(sp);		next
9:	jsr	pc,e.oddz;	next




/------------------------------------------------------------------------------

dup.1W:		mov	(sp),-(sp);	next
dup.l:		jsr	pc,wrdoff;	br	1f;
dus.l:		jsr	pc,wrdoff;	br	0f;
dus.z:		mov	(sp)+,r0;
0:		ble	9b;		bit	$1,r0;
		bne	9b;		add	r0,sp;
		tst	-(sp);		mov	(sp)+,r0;
1:		ble	9b;		inc	r0;
		bic	$1,r0;		mov	r0,r1;
		mov	sp,r5;		add	r0,r5;
		asr	r1;
2:		mov	-(r5),-(sp);	sob	r1,2b;	     next

nop.z:
	next

/------------------------------------------------------------------------------
fil.l:	jsr	pc,wrdoff;	add	eb,r0
	cmp	r0,*filb;	beq	1f;
.if .last
	clr	*eb			/ new file asks	for new	line
	jsr	pc,nexttab;	clr	(r5);
	jsr	pc,nexttab;	mov	*filb,(r5);
.endif
.if .flow + .count + .prof
	mov	curproc,r1	/ current proc descriptor
	mov	4(r1),countfld
	sub	6(r1),countfld	  / start countptr for this proc
	mov	r0,10.(r1)	/ file pointer in procdes
.endif
	mov	r0,*filb
1:	next
lni.z:	inc	*eb;
.if .count + .flow + .prof
	inc	countptr
.if .prof
	add	$4,hiprof
	add	$4,loprof
.endif
.endif
	br	8f
lin.l:	jsr	pc,wrdoff;	br	1f
lin.s0:	clr	r0;		bisb	(pcx)+,r0
1:	cmp    *eb,r0;	       beq     9f;
	mov    r0,*eb
.if .count + .flow + .prof
	mov	countfld,r5
	add	r0,r5		/ this is the right countptr
	mov	r5,countptr
.if .prof
	asl	r5
	asl	r5
	add	ltime,r5
	mov	r5,hiprof
	add	$2,r5
	mov	r5,loprof
.endif
.endif
8:
.if .last
	jsr	pc,nexttab	/ next entry in	lasttab
	mov	*eb,(r5)	/ set endline
.endif
.if .count
	mov	countptr,r1	     / line number in r1
	asl	r1
	asl	r1		/ multiply by 4
	add	lcount,r1	/ r1 is	pointer	to long	integer	now
	add	$1,2(r1)	/ cannot be inc
	adc	(r1)		/ that was all
.endif
.if .flow
	mov	countptr,r1	     / line number in r1
	clr	r0		/ get ready for	divide
	div	$8.,r0		/ r0 = byte offset; r1 = bit offset
	add	lflow,r0	/ r0 is	byte pointer now
	bisb	bits(r1),(r0)	/ set bit
.endif
9:	next

/------------------------------------------------------------------------------

bls.l:	jsr	pc,wrdoff;	br	0f;
bls.z:	mov	(sp)+,r0;
0:	asr	r0
	bhi	1f
	jbr	e.oddz
1:	mov	(sp)+,r1;	sob	r0,1b;
	mov	r1,r0;		br	1f;
blm.l:	jsr	pc,wrdoff
	tst	r0
	br	1f
blm.s0:	clr	r0;		bisb	(pcx)+,r0
1:	jsr	pc,regsave;	jsr	pc,chckptr
	mov	(sp)+,r2;	jsr	pc,chckptr
	mov	(sp)+,r3
	mov	r0,r1
	asr	r0
	beq	2f	/ Now avoid wrong copy.	The pieces may overlap !
	cmp	r3,r2
	beq	2f
	blt	3f
1:	mov	(r3)+,(r2)+
	sob	r0,1b
2:	jsr	pc,regretu;		next
3:	add	r1,r3;		add	r1,r2;
1:	mov	-(r3),-(r2);	sob	r0,1b;		br    2b
/------------------------------------------------------------------------------
/	Range check and	case instructions
/------------------------------------------------------------------------------
csa.l:	jsr	pc,wrdoff;	br	1f;
csa.z:	mov	(sp)+,r0;
1:	sub	$2,r0;		jne	e.illins;
csa.1W:	mov	(sp)+,r0;
	mov	(sp)+,r1;	sub	2(r0),r1;
	blt	6f;		cmp	4(r0),r1;
	blo	6f;		asl	r1;
	add	$6,r1;		add	r0,r1;
5:	mov	(r1),pcx;
	jeq	e.case;		next
6:	mov	r0,r1;		br	5b;

csb.z:	mov	(sp)+,r0;	br	1f;
csb.l:	jsr	pc,wrdoff;
1:	sub	$2,r0;		jne	e.illins;
csb.1W:	mov	(sp)+,r0;	mov	(sp)+,r1;
	mov	2(r0),r5;	mov	r0,pcx;
					/use pcx as ordinary register
2:	add	$4,r0;		cmp	(r0),r1;
	beq	4f;		sob	r5,2b;
	mov	(pcx),pcx;	jeq	e.case;		next
4:	mov	2(r0),pcx;	jeq	e.case;		next

rck.l:	jsr	pc,wrdoff;	br	1f;
rck.z:	mov	(sp)+,r0;
1:	sub	$2,r0;		beq	rck.1W;
	sub	$2,r0;		jne	e.oddz;
	mov	(sp)+,r1;	cmp	(sp),(r1);
	blt	9f;		cmp	(sp),4(r1);
	bgt	9f;		next
rck.1W:	mov	(sp)+,r1;	cmp	(sp),(r1);
	blt	9f;		cmp	(sp),2(r1);
	bgt	9f;		next
9:	mov	$ERANGE,-(sp);	jmp	trp.z;

/------------------------------------------------------------------------------
/	Load and store register
/------------------------------------------------------------------------------
lbllor:
lor.s0:	clr	r0;		bisb	(pcx)+,r0
	cmp	r0,$2;		jhi	e.illins
	asl	r0;		jmp	1f(r0)
1:	br	2f;	br	3f;	br	4f
2:	mov	lb,-(sp);	next
3:	mov	sp,r1;		mov	r1,-(sp);	next
4:	mov	hp,-(sp);	next
lblstr:
str.s0:	clr	r0;		bisb	(pcx)+,r0
	cmp	r0,$2;		jhi	e.illins
	asl	r0;		jmp	1f(r0)
1:	br	2f;	br	3f;	br	4f
2:	mov	(sp)+,lb;	next
3:	mov	(sp)+,r1;	mov	r1,sp;	next
4:	mov	(sp)+,r1;
5:	cmp	r1,sybreak+2;
	blos	5f;		add	$unixextra,sybreak+2;
	sys	indir;sybreak		/ ask for more core
	jec	5b;
	jsr	pc,e.heap;		   / core claim	failed
5:	cmp	r1,globmax;	jlo	e.heap;
	mov	r1,hp;		next

/------------------------------------------------------------------------------

ass.l:	jsr	pc,wrdoff;	br	1f
ass.z:	mov	(sp)+,r0;
1:	cmp	$2,r0;		beq	2f
	cmp	$4,r0;		jne	e.oddz
	mov	(sp)+,r0;
2:	mov	(sp)+,r0;	br	3f
asp.lw:	jsr	pc,wrdoff;	br	2f
asp.w0:	clr	r0;		bisb	(pcx)+,r0;
2:	asl	r0;		br	3f
asp.1W:		asp.2W:		asp.3W:		asp.4W:		asp.5W:
	sub	$88.,r0;
3:	blt	4f;		add	r0,sp;	next
4:	neg	r0;		asr	r0;
2:	mov	$und,-(sp);	sob	r0,2b;	next


/------------------------------------------------------------------------------

dch.z:	mov	(sp)+,r1;	mov	(r1),-(sp);
	cmp	(sp),ml;	jge	e.badptr;
	next			/ dch adjusts local base to
				/ dynamically previous local base

lpb.z:	add	$8.,(sp);	 next	 / static link 8 bytes from lb

/------------------------------------------------------------------------------

gto.l:	jsr	pc,wrdoff;	/ get descriptor address
	add	eb,r0		/ descriptor is	in external address space
	mov	4(r0),r2;	/ new local base after jump
	mov	2(r0),sp;	/ new stack pointer after jump
	mov	(r0),pcx;	/ new program counter
.if .prof + .flow + .count
	mov	firstp,r0
1:	mov	8.(r0),r1
	cmp	r3,2(r1)
	blos	2f
	mov	r1,r0
	br	1b
2:	mov	r0,curproc	/ procdesbase current proc
	mov	4(r0),countfld	/ restore pointer to
	sub	6(r0),countfld	/ table	entries
	mov	10.(r0),*filb	/ file name restored
.endif
	next
/------------------------------------------------------------------------------
/	Ignore mask
/------------------------------------------------------------------------------

lim.z:	mov	ignmask,-(sp);	next	/ load ignore mask
sim.z:	mov	(sp)+,ignmask;	next	/ store	ignore mask
					/ for trap routines

sig.z:	mov	(sp),r1;		/ exchange previous
	mov	uerrorp,(sp);		/ and stacked error
	mov	r1,uerrorp;		/ procedure pointer
	next
/------------------------------------------------------------------------------
/	Signals	generated by UNIX
/------------------------------------------------------------------------------

sig1:
	mov	$sig.trp+0.,-(sp);	br	1f
sig2:
	mov	$sig.trp+2.,-(sp);	br	1f
sig3:
	mov	$sig.trp+4.,-(sp);	br	1f
sig13:
	mov	$sig.trp+24.,-(sp);	br	1f
sig14:
	mov	$sig.trp+26.,-(sp);	br	1f
sig15:
	mov	$sig.trp+28.,-(sp);	br	1f
sig16:
	mov	$sig.trp+30.,-(sp)	/ push address trap number
1:	mov	*(sp),-2(sp);		/ save trap number
	mov	$-2,*(sp)		/ set defold trap number
	mov	$retutrap,(sp)		/ set return address to	rti
	tst	-(sp)			/ trap number position on stack
	jbr	error

sig12:	mov	r0,-(sp)
	mov	$2,r0;			/ fildes standard error
	sys	0;	9b		/ call write message
	sys	signal;12.;sig12	/ this is a mon-error
	jsr	pc,e.badmon
	mov	(sp)+,r0
	rti

sig11:	mov	r0,saver1	 /save r0
	mov	sybreak+2,r0
	sub	sp,r0
	jgt	e.memflt	/real trap
	mov	$7f,r0
	mov	argv,sp		/setup a new stack
	jbr	rude_error

	.data
7:	<stack overflow\n\0>
	.even
sybreak:sys	break;_end	/For indirect calling of break
	.text
sig8:	mov	r0,saver1;
	sys	signal;8.;sig8
	mov	$ECONV,fpperr+6
	stfps	r0
	bit	$100,r0
	beq	1f
	mov	$ECONV,fpperr+6
1:	stst	r0
	mov	$retutrap,-(sp)
	mov	fpperr(r0),-(sp)
	mov	saver1,r0
	jbr	error
retutrap:	 rti
	.data
fpperr:	EILLINS; EILLINS; EFDIVZ; ECONV; EFOVFL; EFUNFL; EFUND;	EILLINS
	.text
/------------------------------------------------------------------------------
/	Errors,traps and all their friends

/------------------------------------------------------------------------------

savereg:	mov	r1,-(sp)	/ stack	r1 so r1 scratch register
		mov	2(sp),r1	/ now r1 contains return address
		mov	r0,2(sp)	/ save r0
		mov	r2,-(sp)	/ save r2
		mov	r3,-(sp)	/ save r3
		mov	r4,-(sp)	/ save r4
		mov	r5,-(sp)	/ save r5
		mov	12.(sp),r0	/ copy error number or param 0
		movf	fr0,-(sp)	/ save float registers
		movf	fr1,-(sp)	/ fr0 and fr1
		stfps	-(sp)		/ and status
		mov	$retsize+26.,r5
		mov	$13.,r2
8:		mov	-(r5),-(sp);	sob	r2,8b
		mov	r0,-(sp)	/ extra	errno (param 0)	on stack
		mov	r1,-(sp)	/ set return address
		rts	pc

restoreg:	mov	(sp)+,r1	/ return address in r1
		add	$2,sp		/ skip error number (param 0)
		mov	$13.,r2;
		mov	$retsize,r5;
8:		mov	(sp)+,(r5)+;	sob	r2,8b
		ldfps	(sp)+		/ restore floating point status
		movf	(sp)+,fr1	/ restore float	registers
		movf	(sp)+,fr0	/ fr0 and fr1
		mov	(sp)+,r5	/ restore r5
		mov	(sp)+,r4	/ restore r4
		mov	(sp)+,r3	/ restore r3
		mov	(sp)+,r2	/ restore r2
		mov	2(sp),r0	/ restore r0
		mov	r1,2(sp)	/ reset	return address
		mov	(sp)+,r1	/ restore r1
		rts	pc

/------------------------------------------------------------------------------
/	Various	error entries
/------------------------------------------------------------------------------

e.badlfr:	mov	r0,r5;	mov	$2,r0;	mov	$blfr,9f+2
		sys	0;9f;
		mov	r5,r0;	rts	pc
.data
9:		sys	write;7;0;
blfr:		<badlfr\n\0>
.even
.text

e.iund:		mov	$EIUND,-(sp);	       br      error
e.iovfl:	mov	$EIOVFL,-(sp);	       br      error
e.idivz:	mov	$EIDIVZ,-(sp);	       br      error
e.range:	mov	$ERANGE,-(sp);	       br      error
e.set:		mov	$ESET,-(sp);	       br      error
e.badptr:	mov	$EBADPTR,-(sp);	       br      fatal
e.fovfl:	mov	$EFOVFL,-(sp);	       br      error
e.funfl:	mov	$EFUNFL,-(sp);	       br      error
e.fdivz:	mov	$EFDIVZ,-(sp);	       br      error
e.fund:		mov	$EFUND,-(sp);	       br      error
e.conv:		mov	$ECONV,-(sp);	       br      error
e.stack:	mov	$ESTACK,-(sp);	       br      fatal
e.badpc:	mov	$EBADPC,-(sp);	       br      fatal
e.badlin:	mov	$EBADLIN,-(sp);	       br      error
e.badlae:	mov	$EBADLAE,-(sp);	       br      error
e.array:	mov	$EARRAY,-(sp);	       br      error
e.badmon:	mov	$EBADMON,-(sp);	       br      error
e.case:		mov	$ECASE,-(sp);	       br      fatal
e.oddz:		mov	$EODDZ,-(sp);	       br      fatal
e.illins:	mov	$EILLINS,-(sp);	       br      fatal
e.heap:		mov	$EHEAP,-(sp);	       br      fatal
e.memflt:	mov	$EMEMFLT,-(sp);	       br      fatal
e.badgto:	mov	$EBADGTO,-(sp);		br	error
/------------------------------------------------------------------------------

fatal:	mov	$hlt.z,-(sp)	/ return from fatal halts
	mov	2(sp),-(sp)	/ dup error number
error:
	jsr	pc,savereg	/ save old register contents
	cmp	$16.,r0;
	ble	9f;
	mov	$1,r1
	ash	r0,r1
	bic	ignmask,r1
	bne	9f
	jsr	pc,restoreg
	add	$2,sp		/ remove error number from stack
	rts	pc
9:
	cmp	uerrorp,$-1	/ has the user defined a trapprocedure ?
	beq	notrap		/ if not kill it off fast
	mov	uerrorp,-(sp)	/ go for cal
	mov	$-1,uerrorp	/ user must set	trap again
	jbr	precal		/ call trap routine

/------------------------------------------------------------------------------

rtt.z:	mov	r2,sp		/ sp to	bottom frame
	add	$8,sp		/ set to top frame
	jsr	pc,restoreg	/ restore status and registers
	add	$2,sp		/ skip error number
	rts	pc
/------------------------------------------------------------------------------
trp.z:	mov	(sp),-(sp);	/ error	number one down
	mov	r4,2(sp);	/ set return address to	main loop
	jbr	error		/ call error routine

/------------------------------------------------------------------------------

notrap:	mov	(sp),r1		/ error	number
	mov	$num+9.,r0
	mov	r1,retarea	/ set error number for exit status
	jsr	pc,itoa		/ make string
	mov	$num,r0
rude_error:
	mov	r0,8f+4
	mov	*filb,8f	/ pointer to file name
	mov	*eb,r1		/ line number
	mov	$line,8f+6	/ line message
	mov	$line+21.,r0
	jsr	pc,itoa		/ convert to string
1:	mov	$8f,r4
2:	mov	(r4)+,r0	/ next string
	beq	5f
3:	mov	r0,9f+2
4:	tstb	(r0)+		/ find zero byte
	bne	4b
	dec	r0
	sub	9f+2,r0		/ string length
	mov	r0,9f+4
	mov	$2,r0		/ diagnostic output
	sys	0; 9f
	jbr	2b
5:				/ no file
	mov	$-1,argc
	jbr	hlt.z

itoa:	mov	r5,-(sp)
	mov	r0,r5
1:	clr	r0
	div	$10.,r0
	add	$'0,r1
	movb	r1,-(r5)
	mov	r0,r1
	bne	1b
	mov	r5,r0
	mov	(sp)+,r5
	rts	pc

	.data
uerrorp:	-1		/ undefined trap procedure
sep:	<: \0>
line:	< on source line      \n\0>
num:	<error    \0>
	.even
8:	0			/ name of text file
	sep			/ separator
	0			/ error
	line+21.		/ line number if present
	0			/ end of list
lblwri:
9:	sys	write;0;0
	.text
/------------------------------------------------------------------------------
/	Exit instruction, with all it's	crud
/------------------------------------------------------------------------------

hlt.z:
	mov	(sp)+,retarea
.if .count + .flow + .prof
	br	9f
.endif
	bne	9f
	clr	r0
	sys	exit
9:
lblhalt:
	sys	creat;1f;666
.data
1:	<em_runinf\0>
2:	<runinf	file failed\n\0>
.even
.text
	bec	2f
3:	mov	$2b,lblwri+2
	mov	$19.,lblwri+4
	mov	$2.,r0
	sys	indir;lblwri
	br	9f
2:	mov	r0,saver0
	mov	$hp,r1
	mov	r1,ndatad
	mov	$txtsiz,r5
	mov	r5,txtsiz
	sub	r5,r1
	mov	r5,lblwri+2
	mov	r1,lblwri+4
	clr	r1
.if .last
	add	$1,r1
.endif
.if .opfreq
	add	$2,r1
.endif
.if .count
	add	$4,r1
.endif
.if .flow
	add	$8.,r1
.endif
.if .prof
	add	$16.,r1
.endif
	mov	r1,entry.
	sys	indir;lblwri
	bes	3b
	mov	pd,lblwri+2
	mov	tblmax,lblwri+4
	sub	pd,lblwri+4
	mov	saver0,r0
	sys	indir;lblwri
	bes	3b
	mov	ml,lblwri+2
	mov	ml,lblwri+4
	neg	lblwri+4
	mov	saver0,r0
	sys	indir;lblwri
	bes	3b
9:
	mov	retarea,r0	/ set exit status
2:	sys	exit

/------------------------------------------------------------------------------
/	System call interface routine
/------------------------------------------------------------------------------
R0_IN	=	0200
R1_IN	=	0100
R0_OUT	=	040
R1_OUT	=	020
CBIT	=	010

lblmon:
mon.z:
	mov	(sp)+,r0;	/ call number from stack
	cmp	r0,$1		/ sys exit ?
	jeq	hlt.z		/ go there
	bit	$177700,r0;	/ range	0-63?
	bne	mon.bad;	/ no? bad call
	movb	r0,call;	/ move call number in system call
	movb	tab(r0),r5;	/ table	lookup call conditions
	cmp	r5,$-1;		/ compare for special context
	beq	mon.special;	/ yes? jump to special context
monmon:	mov	r5,r4;		/ save call conditions
	rolb	r5		/ R0_IN
	bcc	1f		/ check	if bit 7 is on
	mov	(sp)+,r0;	/ call argument	in r0
1:	rolb	r5		/ R1_IN
	bcc	1f		/ check	if bit 6 is on
	mov	(sp)+,r1;	/ argument in r1
1:	bic	$177770,r4	/ clear	all exept bits	2,1,0
	beq	2f		/ if 0 forget about args
	mov	r3,saver1	/ save r3
	mov	$call+2,r3	/ base of args for call
1:	mov	(sp)+,(r3)+	/ move argument
	sob	r4,1b		/ look for more
	mov	saver1,r3	/ restore r3
2:	sys	indir;call	/ this is it folks
	bcc	1f		/ no error set?	forward
	mov	r0,r4		/ copy error in	r4
1:	rolb	r5		/ R0_OUT
	bcc	1f		/ check	original bit 5
	mov	r0,-(sp)	/ stack	r0 from	errno
1:	rolb	r5		/ R1_OUT
	bcc	1f		/ check	original bit 4
	mov	r1,-(sp)	/ stack	r1
1:	rolb	r5		/ CBIT
	bcc	mon.end		/ no c-bit then	ready
mon.cbit:
	mov	r4,-(sp)	/ stack	errno
	beq	mon.end		/ only once if no error
	mov	r4,-(sp)	/ stack	errno twice when  error
mon.end:
	mov	$loop,r4	/ restore r4
	next			/ ready

mon.special:			/ special calls	decoded	here
	cmp	r0,$fork	/ fork?
	beq	mon.fork
	cmp	r0,$signal	/ signal?
	beq	mon.signal
mon.bad:			/ otherwise undecodable
	mov	saver0,r4	/ restore r4
	jsr	pc,e.badmon	/ mon call error routine
	next

mon.fork:
	clr	r5
	clr	r4
	sys	fork
	inc	r5
	bcc	1f
	mov	r0,r4
1:	mov	r0,-(sp)
	mov	r5,-(sp)
	br	mon.cbit
mon.signal:
msign:
	mov	(sp)+,r1		/ trap number
	mov	(sp)+,r0		/ signal number
	cmp	r0,$16.			/ only 1 - 16 legal
	bhi	sig.bad
	mov	r0,call+2		/ move signal number into call
	beq	sig.bad			/ 0 illegal
	asl	r0			/ make 2-32 and	even
	mov	sig.trp-2(r0),r5	/ previous trap	number
	cmp	r1,$256.		/ values -1 and	-2 special
	bhis	1f
	mov	sig.adr-2(r0),r4	/ zero label means illegal signal
	bne	2f
sig.bad:
	mov	$EINVAL,r4		/ bad signal
	jbr	mon.cbit		/ and continue
1:	cmp	r1,$-3			/ -2: reset default, -3: ignore
	blo	sig.bad
	mov	r1,r4			/ trap number in r4
	inc	r4
	inc	r4			/ -2 ->	0, -3 -> -1
2:	mov	r1,sig.trp-2(r0)	/ new trap number
					/ -3 if	ignored; -2 if default action
	mov	r4,call+4		/ translated trap number in call
	clr	r4
	sys	indir;call
	bcs	sig.bad			/ unlikely to happen
	asr	r0			/ special if old label odd
	bcc	1f
	mov	$-3,r5			/ set ignore signal
1:	mov	r5,-(sp)		/ push trap number
	jbr	mon.cbit


	.data

call:	sys 0;	0;	0;	0;	0
sig.trp:
	-2;	-2;	-2;	-2;	-2;	-2;	-2;	-2
	-2;	-2;	21.;	25.;	 -2;	 -2;	 -2;	 -2
sig.adr:
	sig1;	sig2;	sig3;	0;	0;	0;	0;	sig8
	0;	0;	sig11;	sig12;	sig13;	sig14;	sig15;	sig16

tab:
.if V6
.byte -1					/  0 = indir
.byte -1					/  1 = exit
.byte -1					/  2 = fork
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  3 = read
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  4 = write
.byte 2			+R0_OUT		+CBIT	/  5 = open
.byte 0		+R0_IN			+CBIT	/  6 = close
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/  7 = wait
.byte 2			+R0_OUT		+CBIT	/  8 = creat
.byte 2					+CBIT	/  9 = link
.byte 1					+CBIT	/ 10 = unlink
.byte 2					+CBIT	/ 11 = exec
.byte 1					+CBIT	/ 12 = chdir
.byte 0			+R0_OUT	+R1_OUT		/ 13 = time
.byte 3					+CBIT	/ 14 = mknod
.byte 2					+CBIT	/ 15 = chmod
.byte 2					+CBIT	/ 16 = chown
.byte -1					/ 17 = break
.byte 2					+CBIT	/ 18 = stat
.byte 2		+R0_IN			+CBIT	/ 19 = seek
.byte 0			+R0_OUT			/ 20 = getpid
.byte 3					+CBIT	/ 21 = mount
.byte 1					+CBIT	/ 22 = umount
.byte 0		+R0_IN			+CBIT	/ 23 = setuid
.byte 0			+R0_OUT			/ 24 = getuid
.byte 0	+R1_IN	+R0_IN			+CBIT	/ 25 = stime
.byte 3		+R0_IN	+R0_OUT		+CBIT	/ 26 = ptrace
.byte -1					/ 27 = x
.byte 1		+R0_IN			+CBIT	/ 28 = fstat
.byte -1					/ 29 = x
.byte -1					/ 30 = x
.byte 1		+R0_IN			+CBIT	/ 31 = stty
.byte 1		+R0_IN			+CBIT	/ 32 = gtty
.byte -1					/ 33 = x
.byte 0		+R0_IN			+CBIT	/ 34 = nice
.byte 0		+R0_IN				/ 35 = sleep
.byte 0						/ 36 = sync
.byte 1		+R0_IN			+CBIT	/ 37 = kill
.byte 0			+R0_OUT			/ 38 = csw
.byte -1					/ 39 = x
.byte -1					/ 40 = x
.byte 0		+R0_IN	+R0_OUT		+CBIT	/ 41 = dup
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/ 42 = pipe
.byte 1						/ 43 = times
.byte 4						/ 44 = prof
.byte -1					/ 45 = x
.byte 0		+R0_IN			+CBIT	/ 46 = setgid
.byte 0			+R0_OUT			/ 47 = getgid
.byte -1					/ 48 = signal
.byte -1					/ 49 = reserved	for USG
.byte -1					/ 50 = reserved	for USG
.byte -1					/ 51 = x
.byte -1					/ 52 = x
.byte -1					/ 53 = x
.byte -1					/ 54 = x
.byte -1					/ 55 = x
.byte -1					/ 56 = x
.byte -1					/ 57 = x
.byte -1					/ 58 = x
.byte -1					/ 59 = x
.byte -1					/ 60 = x
.byte -1					/ 61 = x
.byte -1					/ 62 = x
.byte -1					/ 63 = x
.endif

.if VPLUS
.byte -1					/  0 = indir
.byte -1					/  1 = exit
.byte -1					/  2 = fork
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  3 = read
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  4 = write
.byte 2			+R0_OUT		+CBIT	/  5 = open
.byte 0		+R0_IN			+CBIT	/  6 = close
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/  7 = wait
.byte 2			+R0_OUT		+CBIT	/  8 = creat
.byte 2					+CBIT	/  9 = link
.byte 1					+CBIT	/ 10 = unlink
.byte 2					+CBIT	/ 11 = exec
.byte 1					+CBIT	/ 12 = chdir
.byte 0			+R0_OUT	+R1_OUT		/ 13 = time
.byte 3					+CBIT	/ 14 = mknod
.byte 2					+CBIT	/ 15 = chmod
.byte 2					+CBIT	/ 16 = chown
.byte -1					/ 17 = break
.byte 2					+CBIT	/ 18 = stat
.byte 2		+R0_IN			+CBIT	/ 19 = seek
.byte 0			+R0_OUT			/ 20 = getpid
.byte 3					+CBIT	/ 21 = mount
.byte 1					+CBIT	/ 22 = umount
.byte 0		+R0_IN			+CBIT	/ 23 = setuid
.byte 0			+R0_OUT			/ 24 = getuid
.byte 0	+R1_IN	+R0_IN			+CBIT	/ 25 = stime
.byte 3		+R0_IN	+R0_OUT		+CBIT	/ 26 = ptrace
.byte 0		+R0_IN	+R0_OUT			/ 27 = alarm
.byte 1		+R0_IN			+CBIT	/ 28 = fstat
.byte 0						/ 29 = pause
.byte -1					/ 30 = x
.byte 1		+R0_IN			+CBIT	/ 31 = stty
.byte 1		+R0_IN			+CBIT	/ 32 = gtty
.byte 2					+CBIT	/ 33 = access
.byte 0		+R0_IN			+CBIT	/ 34 = nice
.byte 0		+R0_IN				/ 35 = sleep
.byte 0						/ 36 = sync
.byte 1		+R0_IN			+CBIT	/ 37 = kill
.byte 0			+R0_OUT			/ 38 = csw
.byte -1					/ 39 = x
.byte 0		+R0_IN	+R0_OUT	+R1_OUT	+CBIT	/ 40 = tell
.byte 0		+R0_IN	+R0_OUT		+CBIT	/ 41 = dup
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/ 42 = pipe
.byte 1						/ 43 = times
.byte 4						/ 44 = prof
.byte -1					/ 45 = x
.byte 0		+R0_IN			+CBIT	/ 46 = setgid
.byte 0			+R0_OUT			/ 47 = getgid
.byte -1					/ 48 = signal
.byte -1					/ 49 = reserved	for USG
.byte -1					/ 50 = reserved	for USG
.byte 1					+CBIT	/ 51 = acct
.byte -1					/ 52 = x
.byte -1					/ 53 = x
.byte -1					/ 54 = x
.byte -1					/ 55 = x
.byte -1					/ 56 = x
.byte -1					/ 57 = x
.byte -1					/ 58 = x
.byte -1					/ 59 = x
.byte -1					/ 60 = x
.byte -1					/ 61 = x
.byte -1					/ 62 = x
.byte -1					/ 63 = x
.endif

.if V7
.byte -1					/  0 = indir
.byte 0						/  1 = exit
.byte -1					/  2 = fork
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  3 = read
.byte 2		+R0_IN	+R0_OUT		+CBIT	/  4 = write
.byte 2			+R0_OUT		+CBIT	/  5 = open
.byte 0		+R0_IN			+CBIT	/  6 = close
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/  7 = wait
.byte 2			+R0_OUT		+CBIT	/  8 = creat
.byte 2					+CBIT	/  9 = link
.byte 1					+CBIT	/ 10 = unlink
.byte -1					/ 11 = x no exec in em code
.byte 1					+CBIT	/ 12 = chdir
.byte -1					/ 13 = x time is obsolete
.byte 3					+CBIT	/ 14 = mknod
.byte 2					+CBIT	/ 15 = chmod
.byte 3					+CBIT	/ 16 = chown
.byte -1					/ 17 = break
.byte 2					+CBIT	/ 18 = stat
.byte 3		+R0_IN	+R0_OUT	+R1_OUT	+CBIT	/ 19 = lseek
.byte 0			+R0_OUT			/ 20 = getpid
.byte 3					+CBIT	/ 21 = mount
.byte 1					+CBIT	/ 22 = umount
.byte 0		+R0_IN			+CBIT	/ 23 = setuid
.byte 0			+R0_OUT	+R1_OUT		/ 24 = getuid
.byte 0	+R1_IN	+R0_IN			+CBIT	/ 25 = stime
.byte 3		+R0_IN	+R0_OUT		+CBIT	/ 26 = ptrace
.byte 0		+R0_IN	+R0_OUT			/ 27 = alarm
.byte 1		+R0_IN			+CBIT	/ 28 = fstat
.byte 0						/ 29 = pause
.byte 2					+CBIT	/ 30 = utime
.byte -1					/ 31 = x
.byte -1					/ 32 = x
.byte 2					+CBIT	/ 33 = access
.byte 0		+R0_IN			+CBIT	/ 34 = nice
.byte 1						/ 35 = ftime
.byte 0						/ 36 = sync
.byte 1		+R0_IN			+CBIT	/ 37 = kill
.byte -1					/ 38 = x
.byte -1					/ 39 = x
.byte -1					/ 40 = x
.byte 0	+R1_IN	+R0_IN	+R0_OUT		+CBIT	/ 41 = dup
.byte 0			+R0_OUT	+R1_OUT	+CBIT	/ 42 = pipe
.byte 1						/ 43 = times
.byte 4						/ 44 = prof
.byte -1					/ 45 = x
.byte 0		+R0_IN			+CBIT	/ 46 = setgid
.byte 0			+R0_OUT	+R1_OUT		/ 47 = getgid
.byte -1					/ 48 = signal
.byte -1					/ 49 = reserved	for USG
.byte -1					/ 50 = reserved	for USG
.byte 1					+CBIT	/ 51 = acct
.byte 3					+CBIT	/ 52 = phys
.byte 1					+CBIT	/ 53 = lock
.byte 3					+CBIT	/ 54 = ioctl
.byte -1					/ 55 = x
.byte 2					+CBIT	/ 56 = mpxcall
.byte -1					/ 57 = x
.byte -1					/ 58 = x
.byte 3					+CBIT	/ 59 = exece
.byte 1					+CBIT	/ 60 = umask
.byte 1					+CBIT	/ 61 = chroot
.byte -1					/ 62 = x
.byte -1					/ 63 = x
.endif

	.text
/------------------------------------------------------------------------------
/	General	subroutines
/------------------------------------------------------------------------------

wrdoff:	movb	(pcx)+,r0	/get first byte
	swab	r0		/put it	in high	byte
	clrb	r0		/clear low byte	of r0
	bisb	(pcx)+,r0	/"or" second byte in
	rts	pc		/done

/------------------------------------------------------------------------------

tstr0:	cmp	r0,$04;		jgt	e.oddz;
	cmp	r0,$02;		jne	e.oddz;		rts pc

chckptr:			/ this routine traps a pointer outside
				/ the globals, the stack or the	heap
	bit	$1,2(sp);	bne	8f
chckptb:
	mov	2(sp),r5;
	cmp	r5,sp;		bhis	9f
	cmp	r5,hp;		bhis	8f
.if  .count + .prof + .flow
	cmp	r5,tblmax;	bhis	9f
	cmp	r5,globmax;	bhis	8f
.endif
	cmp	r5,eb;		bhis	9f
8:	jsr	pc,e.badptr
9:	rts	pc

.if .last
nexttab: mov	linused,r5;
	add	$2,r5		/ increment lasttab
	cmp	r5,$linused	/ top of table reached?
	blo	1f
	sub	$96.,r5
1:	mov	r5,linused
	rts	pc
.endif
regsave:
	mov	r5,savearea
	mov	$[savearea+2],r5
	mov	r4,(r5)+
	mov	r3,(r5)+
	mov	r2,(r5)
	rts	pc
regretu:
	mov	$[savearea+6],r5
	mov	(r5),r2
	mov	-(r5),r3
	mov	-(r5),r4
	mov	-(r5),r5
	rts	pc

setfloat:
	cmp	r0,$8.
	bne	1f
	setd
	rts	pc
1:	cmp	r0,$04
	bne	3f
	setf
2:	rts	pc
3:	jmp	e.oddz
setint:
	cmp	r0,$04
	bne	4f
	setl
	rts	pc
4:	cmp	r0,$02
	bne	3b
	seti
5:	rts	pc



/------------------------------------------------------------------------------
/	Leftover data
/------------------------------------------------------------------------------




	.bss
filb:	.=.+2
curproc:.=.+2
linmax:	.=.+2
countptr:.=.+2
countfld:.=.+2
hiprof:	.=.+2
loprof:	.=.+2
ignmask:.=.+2		/ ignore mask for trap
retsize:.=.+2		/ size of return value of function
retarea:.=.+8		/ return area for function value
savearea:	.=.+8	/ save register	area
saver0:	.=.+2
saver1:	.=.+2
header:
	txtsiz:	.=.+2	/ program textsize in bytes
	ndatad:	.=.+2	/ number of loadfile descriptors
	nprocs:	.=.+2	/ number of entries in procedure descriptors
option:	entry.:	.=.+2	/ procedure number to start
	nlines:	.=.+2	/ maximum sorceline number
	szdata:	.=.+2	/ address of lowest uninitialized byte
	firstp:	.=.+2	/ descriptor address first basic block of text
	maxcount: .=.+2	/ total	number of processable source lines
argc:	.=.+2
argv:	.=.+2
environ:
	.=.+2
pb:	.=.+2
pd:	.=.+2
eb:	.=.+2
globmax:
	.=.+2
tblmax:	.=.+2
ml:	.=.+2
.if .last
lasttab:.=.+96.	       / 16 descriptors	of integers + index at the end
linused:.=.+2
.endif

.if .opfreq
counttab:
	.=.+1664.
.endif

.if .count
lcount:	.=.+2
countsiz:.=.+2
.endif

.if .flow
lflow:	.=.+2
flowsiz:.=.+2
.endif

.if .prof
ltime:	.=.+2
profsiz:.=.+2
.endif

hp:	.=.+2
