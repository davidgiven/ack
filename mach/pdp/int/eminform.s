/ 
/  (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
/ 
/           This product is part of the Amsterdam Compiler Kit.
/ 
/  Permission to use, sell, duplicate or disclose this software must be
/  obtained in writing. Requests for such permissions may be sent to
/ 
/       Dr. Andrew S. Tanenbaum
/       Wiskundig Seminarium
/       Vrije Universiteit
/       Postbox 7161
/       1007 MC Amsterdam
/       The Netherlands
/ 
/  
/------------------------------------------------------------------------------
/
	indir	= 0
	exit	= 1
	fork	= 2
	read	= 3
	write	= 4
	open	= 5
	close	= 6
	creat	= 8.
	unlink	= 10.
	break	= 17.
	alarm	= 27.
	pause	= 29.
	sleep	= 35.
	signal	= 48.

	iot	=	4
/------------------------------------------------------------------------------
/	External references
/------------------------------------------------------------------------------

	.globl	_end

/
/------------------------------------------------------------------------------
/	Now the	real program starts
/------------------------------------------------------------------------------

startoff:
	mov	sp,memlim
	mov	$ldfail,r5
	mov	$emfile,forward+2
	sys	indir;forward
	jes	badruninf;
	mov	r0,saver0;
	mov	$rhfail,r5;
	sys	read;header;34.
	jes	badruninf;
	cmp	$34.,r0
	jne	badruninf
	mov	$rtfail,r5
	mov	$_end,loadstart
	mov	option,r1
	bit	$17774,r1
	bne	1f
	mov	tblmax,globmax
1:
	bit	$1,r1
	beq	1f
	mov	saver0,r0
	sys	read;lasttab;98.
	jes	badruninf;

1:	bit	$2,r1
	beq	1f
	mov	saver0,r0
	sys	read;opfrb;512.
	jes	badruninf
	mov	saver0,r0
	sys	read;opfra;512.
	jes	badruninf
	mov	saver0,r0
	sys	read;opfrc;640.
	jes	badruninf
1:	bit	$4,r1
	beq	1f
	mov	saver0,r0
	sys	read;lcount;4
	jes	badruninf
1:	bit	$8.,r1
	beq	1f
	mov	saver0,r0
	sys	read;lflow;4
	jes	badruninf
1:	bit	$16.,r1
	beq	1f
	mov	saver0,r0
	sys	read;ltime;4
	jes	badruninf
1:
	mov	tblmax,r3
	sub	pd,r3
	sub	ml,r3
	mov	$_end,sybreak+2
	add	r3,sybreak+2;
	sys	indir;sybreak
	mov	r3,lblread+4
	mov	$_end,lblread+2;
	mov	saver0,r0
	sys	indir;lblread;
	jes	badruninf
	mov	saver0,r0
	sys	close
	sys	unlink;emfile
lbladj:
	add	$header,linused
	sub	header,linused

	mov	pd,filext
	sub	loadstart,filext
	mov	filext,filarg
	add	ml,filarg
	sub	tblmax,filarg
lbllast:
	bit	$1,option	/test for last
	beq	2f		/ no last table	jump
	sys	creat;emlast;666
	bcs	2f
	cmp	linused,$lasttab
	bhis	3f
	mov	$yetmsg,lblwri+2
	mov	$40.,lblwri+4
	sys	indir;lblwri
	br	2f
3:	mov	r0,saver0
	cmp	linused-2,$-1
	bne	6f
	mov	$linused-2,linused
	clr	linused-2
6:	jsr	pc,slilast
	inc	r2
	jsr	pc,nexttab
	tst	(r5)
	beq	3f
	sub	$2,linused
	cmp	eb,(r5)
	blo	3f
7:	cmp	r2,$lnend
	blt	1f
	jsr	pc,slnlast
1:	jsr	pc,lnrnxt
	bne	7b
3:	jsr	pc,sfllast
	bne	6b
	mov	saver0,r0
	sys	close
2:

lblopfreq:
	bit	$2,option
	beq	9f
	sys	creat;emopf;666
	bcs	9f
	mov	r0,saver0
	mov	$opcode,r3
	mov	$104.,r4
	mov	$counttab,r5
0:	mov	$lnlast,r2
	mov	$8.,r0
2:	movb	(r3)+,(r2)+
	sob	r0,2b
	add	$8,r2
	mov	r2,-(sp)
	jsr	pc,ltoa
	mov	(sp)+,r2
	add	$5,r2
	mov	$8.,r0
2:	movb	(r3)+,(r2)+
	sob	r0,2b
	add	$8,r2
	mov	r2,-(sp)
	jsr	pc,ltoa
	mov	(sp)+,r2
	add	$5,r2
	mov	$8.,r0
2:	movb	(r3)+,(r2)+
	sob	r0,2b
	add	$8,r2
	mov	r2,-(sp)
	jsr	pc,ltoa
	mov	(sp)+,r2
	add	$5,r2
	mov	$8.,r0
2:	movb	(r3)+,(r2)+
	sob	r0,2b
	add	$8,r2
	mov	r2,-(sp)
	jsr	pc,ltoa
	mov	(sp)+,r2
	mov	r3,-(sp)
	jsr	pc,slnlast
	mov	(sp)+,r3
	sob	r4,0b
	mov	saver0,r0
	sys	close
9:
lblcount:
	clr	filb
	bit	$4,option
	beq	9f
	sys	creat;emcount;666
	bcs	9f
	mov	r0,saver0
	mov	lcount,r5
	add	$4,r5
	sub	filext,r5
	jsr	pc,procrun
	mov	saver0,r0
	sys	close
9:
lblprof:
	bit	$16.,option
	beq	9f
	sys	creat;emprof;666
	bcs	9f
	mov	r0,saver0
	mov	ltime,r5
	sub	filext,r5
	mov	$profmsg,lblwri+2
	mov	$29,lblwri+4
	mov	$profmsg+27,r2
	jsr	pc,ltoa
	mov	saver0,r0
	sys	indir;lblwri
	jsr	pc,procrun
	mov	saver0,r0
	sys	close
9:
lblflow:
	mov	$1,filb
	bit	$8.,option
	beq	9f
	sys	creat;emflow;666
	bcs	9f
	mov	lflow,r5
	mov	$-1,flowsiz
	sub	filext,r5
	jsr	pc,procrun
	mov	saver0,r0
	sys	close
9:
	clr	r0
	sys	exit

badruninf:
	mov	$0f,lblwri+2
	mov	$21.,lblwri+4
	mov	$2,r0
	sys	indir;lblwri
	mov	r5,lblwri+2
	mov	$23.,lblwri+4
	mov	$2,r0
	sys	indir;lblwri
	iot			/force core dump
.data
lblwri:
        sys     write;0;0
forward:
        sys     open;0;0
sybreak:
        sys     break;0;
lblread:
        sys     read;0;0
0:      <em_runinf load error,\0>
ldfail: < open runinf failed   \n\0>
rhfail: < read header failed   \n\0>
rtfail: < read tables failed   \n\0>
msgto:  <to>
emlast: <em_last\0>
emcount:<em_count\0>
emfile: <em_runinf\0>
emopf:  <em_opfreq\0>
emprof: <em_profile\0>
emflow: <em_flow\0>
yetmsg: <no line or file message processed yet \n\n\0>
unknown:<with unknown filename\0>
lilast: <\nlines               \0>
lnlast: <                              >
        <                              >
lnend:  <                                  \0>
fllast: <    of file                       >
        <                                  \0>
profmsg:<\ninitialization             \n\0>
sep:    <\n\0>
/----------------------------------------------------------------------------
opcode:
<loc.0   loc.1   loc.2   loc.3   loc.4   loc.5   loc.6   loc.7   >
<loc.8   loc.9   loc.10  loc.11  loc.12  loc.13  loc.14  loc.15  >
<loc.16  loc.17  loc.18  loc.19  loc.20  loc.21  loc.22  loc.23  >
<loc.24  loc.25  loc.26  loc.27  loc.28  loc.29  loc.30  loc.31  >
<loc.32  loc.33  aar.1W  adf.s0  adi.1W  adi.2W  adp.l   adp.1   >
<adp.2   adp.s0  adp.s_1 ads.1W  and.1W  asp.1W  asp.2W  asp.3W  >
<asp.4W  asp.5W  asp.w0  beq.l   beq.s0  bge.s0  bgt.s0  ble.s0  >
<blm.s0  blt.s0  bne.s0  bra.l   bra.s_1 bra.s_2 bra.s0  bra.s1  >
<cal.1   cal.2   cal.3   cal.4   cal.5   cal.6   cal.7   cal.8   >
<cal.9   cal.10  cal.11  cal.12  cal.13  cal.14  cal.15  cal.16  >
<cal.17  cal.18  cal.19  cal.20  cal.21  cal.22  cal.23  cal.24  >
<cal.25  cal.26  cal.27  cal.28  cal.s0  cff.z   cif.z   cii.z   >
<cmf.s0  cmi.1W  cmi.2W  cmp.z   cms.s0  csa.1W  csb.1W  dec.z   >
<dee.w0  del.w_1 dup.1W  dvf.s0  dvi.1W  fil.l   inc.z   ine.lw  >
<ine.w0  inl._1W inl._2W inl._3W inl.w_1 inn.s0  ior.1W  ior.s0  >
<lae.l   lae.w0  lae.w1  lae.w2  lae.w3  lae.w4  lae.w5  lae.w6  >

<lal.p   lal.n   lal.0   lal._1  lal.w0  lal.w_1 lal.w_2 lar.1W  >
<ldc.0   lde.lw  lde.w0  ldl.0   ldl.w_1 lfr.1W  lfr.2W  lfr.s0  >
<lil.w_1 lil.w0  lil.0   lil.1W  lin.l   lin.s0  lni.z   loc.l   >
<loc._1  loc.s0  loc.s_1 loe.lw  loe.w0  loe.w1  loe.w2  loe.w3  >
<loe.w4  lof.l   lof.1W  lof.2W  lof.3W  lof.4W  lof.s0  loi.l   >
<loi.1   loi.1W  loi.2W  loi.3W  loi.4W  loi.s0  lol.pw  lol.nw  >
<lol.0   lol.1W  lol.2W  lol.3W  lol._1W lol._2W lol._3W lol._4W >
<lol._5W lol._6W lol._7W lol._8W lol.w0  lol.w_1 lxa.1   lxl.1   >
<lxl.2   mlf.s0  mli.1W  mli.2W  rck.1W  ret.0   ret.1W  ret.s0  >
<rmi.1W  sar.1W  sbf.s0  sbi.1W  sbi.2W  sdl.w_1 set.s0  sil.w_1 >
<sil.w0  sli.1W  ste.lw  ste.w0  ste.w1  ste.w2  stf.l   stf.1W  >
<stf.2W  stf.s0  sti.1   sti.1W  sti.2W  sti.3W  sti.4W  sti.s0  >
<stl.pw  stl.nw  stl.0   stl.1W  stl._1W stl._2W stl._3W stl._4W >
<stl._5W stl.w_1 teq.z   tgt.z   tlt.z   tne.z   zeq.l   zeq.s0  >
<zeq.s1  zer.s0  zge.s0  zgt.s0  zle.s0  zlt.s0  zne.s0  zne.s_1 >
<zre.lw  zre.w0  zrl._1W zrl._2W zrl.w_1 zrl.nw  escape1 escape2 >

<aar.l   aar.z   adf.l   adf.z   adi.l   adi.z   ads.l   ads.z   >
<adu.l   adu.z   and.l   and.z   asp.lw  ass.l   ass.z   bge.l   >
<bgt.l   ble.l   blm.l   bls.l   bls.z   blt.l   bne.l   cai.z   >
<cal.l   cfi.z   cfu.z   ciu.z   cmf.l   cmf.z   cmi.l   cmi.z   >
<cms.l   cms.z   cmu.l   cmu.z   com.l   com.z   csa.l   csa.z   >
<csb.l   csb.z   cuf.z   cui.z   cuu.z   dee.lw  del.pw  del.nw  >
<dup.l   dus.l   dus.z   dvf.l   dvf.z   dvi.l   dvi.z   dvu.l   >
<dvu.z   fef.l   fef.z   fif.l   fif.z   inl.pw  inl.nw  inn.l   >
<inn.z   ior.l   ior.z   lar.l   lar.z   ldc.l   ldf.l   ldl.pw  >
<ldl.nw  lfr.l   lil.pw  lil.nw  lim.z   los.l   los.z   lor.s0  >
<lpi.l   lxa.l   lxl.l   mlf.l   mlf.z   mli.l   mli.z   mlu.l   >
<mlu.z   mon.z   ngf.l   ngf.z   ngi.l   ngi.z   nop.z   rck.l   >
<rck.z   ret.l   rmi.l   rmi.z   rmu.l   rmu.z   rol.l   rol.z   >
<ror.l   ror.z   rtt.z   sar.l   sar.z   sbf.l   sbf.z   sbi.l   >
<sbi.z   sbs.l   sbs.z   sbu.l   sbu.z   sde.l   sdf.l   sdl.pw  >
<sdl.nw  set.l   set.z   sig.z   sil.pw  sil.nw  sim.z   sli.l   >

<sli.z   slu.l   slu.z   sri.l   sri.z   sru.l   sru.z   sti.l   >
<sts.l   sts.z   str.s0  tge.z   tle.z   trp.z   xor.l   xor.z   >
<zer.l   zer.z   zge.l   zgt.l   zle.l   zlt.l   zne.l   zrf.l   >
<zrf.z   zrl.pw  dch.z   exg.s0  exg.l   exg.z   lpb.z   gto.l   \0\0>
	.even
	.text
/------------------------------------------------------------------------------
/	General	subroutines
/------------------------------------------------------------------------------

wrdoff:	movb	(r3)+,r0       /get first byte
	swab	r0		/put it	in high	byte
	clrb	r0		/clear low byte	of r0
	bisb	(r3)+,r0       /"or" second byte in
	rts	pc		/done

/------------------------------------------------------------------------------


nexttab: mov	linused,r5;
	add	$2,r5		/ increment lasttab
	cmp	r5,$linused	/ top of table reached?
	blo	1f
	sub	$96.,r5
1:	mov	r5,linused
	rts	pc

slilast: mov	$lnlast,r2
	mov	$6,r0
	mov	$lilast,r3
2:	movb	(r3)+,(r2)+
	sob	r0,2b
	rts	pc

slnlast: mov	$lnlast,lblwri+2
	mov	r2,r3
	sub	$lnlast,r3
	beq	3f
	inc	r3
	movb	sep,(r2)
1:	mov	r3,lblwri+4
	mov	saver0,r0
	sys	indir; lblwri
	mov	$lnlast,r2
2:	movb	$' ,(r2)+
	sob	r3,2b
3:	mov	$lnlast,r2
	rts	pc

lnrnxt:	jsr	pc,nexttab
	tst	(r5)
	ble	0f
	mov	(r5),r1
	clr	(r5)
	add	$6,r2
	mov	r2,r0
	jsr	pc,itoa
0:	rts	pc

sfllast:jsr	pc,nexttab
	tst	(r5)
	bne	0f
	clr	linused
	mov	eb,-(sp)
	sub	filext,(sp)
	add	$4,(sp)
	mov	*(sp),(sp)
	mov	(sp)+,(r5)
0:	jsr	pc,slnlast
	jsr	pc,filadj
	mov	$14.,r3
	mov	$fllast+14.,r2
	mov	(r5),r0
3:	inc	r3
	movb	(r0)+,(r2)+
	bne	3b
	movb	sep,-(r2)
	mov	$fllast,lblwri+2
	mov	r3,lblwri+4
	mov	saver0,r0
	sys	indir;lblwri
	mov	$fllast+14.,r2
	sub	$10.,r3
2:	movb	$' ,(r2)+
	sob	r3,2b
	clr	(r5)
	tst	linused
	beq	2f
	jsr	pc,slilast
2:	rts	pc

filadj:
	cmp	ml,(r5)
	bhi	8f
	sub	filarg,(r5)
	br	7f
8:	cmp	eb,(r5)
	bhi	7f
	sub	filext,(r5)
7:	rts	pc

procrun:
	mov	firstp,r4
	sub	filext,r4
0:
	tst	4(r4)
	beq	8f
	jsr	pc,msgfile
	mov	r4,-(sp)
	mov	countsiz,r4
7:	cmp	r2,$lnend+10
	blo	6f
	jsr	pc,slnlast
	mov	$lnlast,r2
6:	tst	filb
	bne	3f
	add	$11.,r2
	mov	r2,-(sp)
	jsr	pc,ltoa
	mov	(sp)+,r2
	br	4f
3:
	jsr	pc,bittoa
	add	$4,r2
4:	sob	r4,7b
	jsr	pc,slnlast
	mov	(sp)+,r4
8:	mov	8.(r4),r4
	beq	8f
	sub	filext,r4
	br	0b
	mov	r0,saver0
	sys	write;sep;1
8:	rts	pc


msgfile:
	jsr	pc,slilast
	add	$6,r2
	mov	6(r4),r1
	mov	r2,r0
	jsr	pc,itoa
	add	$4,r2
	movb	 msgto,(r2)+
	movb	msgto+1,(r2)+
	add	$6,r2
	mov	8.(r4),r1
3:	bne	1f
	mov	maxcount,r1
	br	2f
1:	sub	filext,r1
	tst	4(r1)
	beq	1f
	mov	4(r1),r1
	br	2f
1:	mov	8.(r1),r1
	br	3b
2:	sub	4(r4),r1
	mov	r1,countsiz
	dec	r1		/ is this a bug?
	add	6(r4),r1
	mov	r2,r0
	jsr	pc,itoa
	mov	$fllast,r3
	mov	$14,r1
2:	movb	(r3)+,(r2)+
	sob	r1,2b
	mov	10.(r4),r3
	bne	6f
	mov	$unknown,r3
	br	5f
6:	cmp	ml,r3
	bhi	4f
	sub	filarg,r3
	br	5f
4:	cmp	eb,r3
	bhi	5f
	sub	filext,r3
5:
lblmsgf:
2:	movb	(r3)+,(r2)+
	bne	2b
	jsr	pc,slnlast
	rts	pc

itoa:

	mov	r5,-(sp)
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

	rts	pc
ltoa:
0:	mov	(r5),r1
	clr	r0
	div	$10.,r0
	mov	r0,(r5)
	swab	r1
	clr	r0
	bisb	3(r5),r1
	div	$10.,r0
	movb	r0,3(r5)
	clr	r0
	swab	r1
	bisb	2(r5),r1
	div	$10.,r0
	add	$'0,r1
	movb	r1,-(r2)
	movb	r0,2(r5)
	bne	0b
	tst	(r5)
	bne	0b
	add	$4,r5
	rts	pc

bittoa:
	mov	(r5),r1
	mov	flowsiz,r0
	ash	r0,r1
	bit	$1,r1
	beq	0f
	movb	$'1,(r2)
	br	1f
0:	movb	$'0,(r2)
1:	dec	r0
	cmp	$-16.,r0
	beq	2f
	mov	r0,flowsiz
	br	3f
2:	clr	flowsiz
	add	$2,r5
3:	rts	pc

/------------------------------------------------------------------------------
/	Leftover data
/------------------------------------------------------------------------------




	.bss
filb:	.=.+2
loadstart:	.=.+2
saver0:		.=.+2
filarg:		.=.+2
filext:		.=.+2
memlim:		.=.+2
header:
bstrt:	txtsiz:	.=.+2
bend:	ndatad:	.=.+2
	nprocs:	.=.+2
option:	entry.:	.=.+2
	nlines:	.=.+2
	szdata:	.=.+2
	firstp:	.=.+2
	maxcount: .=.+2
argc:	.=.+2
argv:	.=.+2
environ:
	.=.+2
pb:	.=.+2
pd:	.=.+2
eb:	.=.+2
globmax: .=.+2
tblmax:	.=.+2
ml:	.=.+2
lasttab:.=.+96.	       / 16 descriptors	of integers + index at the end
linused:.=.+2

counttab:
opfra:	.=.+512.
opfrb:	.=.+512.
opfrc:	.=.+640.

lcount:	.=.+2
countsiz:.=.+2

lflow:	.=.+2
flowsiz:.=.+2

ltime:	.=.+2
profsiz:.=.+2
