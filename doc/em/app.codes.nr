.BP
.AP "EM CODE TABLES"
The following table is used by the assembler for EM machine
language.
It specifies the opcodes used for each instruction and
how arguments are mapped to machine language arguments.
The table is presented in three columns,
each line in each column contains three or four fields.
Each line describes a range of interpreter opcodes by
specifying for which instruction the range is used, the type of the
opcodes (mini, shortie, etc..) and range for the instruction
argument.
.A
The first field on each line gives the EM instruction mnemonic,
the second field gives some flags.
If the opcodes are minis or shorties the third field specifies
how many minis/shorties are used.
The last field gives the number of the (first) interpreter
opcode.
.N 1
Flags :
.IS 3
.N 1
Opcode type, only one of the following may be specified.
.PS - 5 "  "
.PT \-
opcode without argument
.PT m
mini
.PT s
shortie
.PT 2
opcode with 2-byte signed argument
.PT 4
opcode with 4-byte signed argument
.PT 8
opcode with 8-byte signed argument
.PE
Secondary (escaped) opcodes.
.PS - 5 "  "
.PT e
The opcode thus marked is in the secondary opcode group instead
of the primary
.PE
restrictions on arguments
.PS - 5 "  "
.PT N
Negative arguments only
.PT P
Positive and zero arguments only
.PE
mapping of arguments
.PS - 5 "  "
.PT w
argument must be divisible by the wordsize and is divided by the
wordsize before use as opcode argument.
.PT o
argument ( possibly after division ) must be >= 1 and is
decremented before use as opcode argument
.PE
.IE
If the opcode type is 2,4 or 8 the resulting argument is used as
opcode argument (least significant byte first).
.N
If the opcode type is mini, the argument is added
to the first opcode \- if in range \- .
If the argument is negative, the absolute value minus one is
used in the algorithm above.
.N
For shorties with positive arguments the first opcode is used
for arguments in the range 0..255, the second for the range
256..511, etc..
For shorties with negative arguments the first opcode is used
for arguments in the range \-1..\-256, the second for the range
\-257..\-512, etc..
The byte following the opcode contains the least significant
byte of the argument.
First some examples of these specifications.
.PS - 5
.PT "aar mwPo 1 34"
Indicates that opcode 34 is used as a mini for Positive
instruction arguments only.
The w and o indicate division and decrementing of the
instruction argument.
Because the resulting argument must be zero ( only opcode 34 may be used
), this mini can only be used for instruction argument 2.
Conclusion: opcode 34 is for "AAR 2".
.PT "adp sP 1 41"
Opcode 41 is used as shortie for ADP with arguments in the range
0..255.
.PT "bra sN 2 60"
Opcode 60 is used as shortie for BRA with arguments \-1..\-256,
61 is used for arguments \-257..\-512.
.PT "zer e\- 145"
Escaped opcode 145 is used for ZER.
.PE
The interpreter opcode table:
.N 1
.IS 3
.DS B
.so itables
.DE 0
.IE
.P
The table above results in the following dispatch tables.
Dispatch tables are used by interpreters to jump to the
routines implementing the EM instructions, indexed by the next opcode.
Each line of the dispatch tables gives the routine names
of eight consecutive opcodes, preceded by the first opcode number
on that line.
Routine names consist of an EM mnemonic followed by a suffix.
The suffices show the encoding used for each opcode.
.N
The following suffices exist:
.N 1
.VS 1 0
.IS 4
.PS - 11
.PT .z
no arguments
.PT .l
16-bit argument
.PT .lw
16-bit argument divided by the wordsize
.PT .p
positive 16-bit argument
.PT .pw
positive 16-bit argument divided by the wordsize
.PT .n
negative 16-bit argument
.PT .nw
negative 16-bit argument divided by the wordsize
.PT .s<num>
shortie with <num> as high order argument byte
.PT .sw<num>
shortie with argument divided by the wordsize
.PT .<num>
mini with <num> as argument
.PT .<num>W
mini with <num>*wordsize as argument
.PE 3
<num> is a possibly negative integer.
.VS 1 1
.IE
The dispatch table for the 256 primary opcodes:
.DS B
.ta 7n 16n 25n 34n 43n 52n 61n 70n
   0	loc.0	loc.1	loc.2	loc.3	loc.4	loc.5	loc.6	loc.7
   8	loc.8	loc.9	loc.10	loc.11	loc.12	loc.13	loc.14	loc.15
  16	loc.16	loc.17	loc.18	loc.19	loc.20	loc.21	loc.22	loc.23
  24	loc.24	loc.25	loc.26	loc.27	loc.28	loc.29	loc.30	loc.31
  32	loc.32	loc.33	aar.1W	adf.s0	adi.1W	adi.2W	adp.l	adp.1
  40	adp.2	adp.s0	adp.s\-1	ads.1W	and.1W	asp.1W	asp.2W	asp.3W
  48	asp.4W	asp.5W	asp.w0	beq.l	beq.s0	bge.s0	bgt.s0	ble.s0
  56	blm.s0	blt.s0	bne.s0	bra.l	bra.s\-1	bra.s\-2	bra.s0	bra.s1
  64	cal.1	cal.2	cal.3	cal.4	cal.5	cal.6	cal.7	cal.8
  72	cal.9	cal.10	cal.11	cal.12	cal.13	cal.14	cal.15	cal.16
  80	cal.17	cal.18	cal.19	cal.20	cal.21	cal.22	cal.23	cal.24
  88	cal.25	cal.26	cal.27	cal.28	cal.s0	cff.z	cif.z	cii.z
  96	cmf.s0	cmi.1W	cmi.2W	cmp.z	cms.s0	csa.1W	csb.1W	dec.z
 104	dee.w0	del.w\-1	dup.1W	dvf.s0	dvi.1W	fil.l	inc.z	ine.lw
 112	ine.w0	inl.\-1W	inl.\-2W	inl.\-3W	inl.w\-1	inn.s0	ior.1W	ior.s0
 120	lae.l	lae.w0	lae.w1	lae.w2	lae.w3	lae.w4	lae.w5	lae.w6
 128	lal.p	lal.n	lal.0	lal.\-1	lal.w0	lal.w\-1	lal.w\-2	lar.W
 136	ldc.0	lde.lw	lde.w0	ldl.0	ldl.w\-1	lfr.1W	lfr.2W	lfr.s0
 144	lil.w\-1	lil.w0	lil.0	lil.1W	lin.l	lin.s0	lni.z	loc.l
 152	loc.\-1	loc.s0	loc.s\-1	loe.lw	loe.w0	loe.w1	loe.w2	loe.w3
 160	loe.w4	lof.l	lof.1W	lof.2W	lof.3W	lof.4W	lof.s0	loi.l
 168	loi.1	loi.1W	loi.2W	loi.3W	loi.4W	loi.s0	lol.pw	lol.nw
 176	lol.0	lol.1W	lol.2W	lol.3W	lol.\-1W	lol.\-2W	lol.\-3W	lol.\-4W
 184	lol.\-5W	lol.\-6W	lol.\-7W	lol.\-8W	lol.w0	lol.w\-1	lxa.1	lxl.1
 192	lxl.2	mlf.s0	mli.1W	mli.2W	rck.1W	ret.0	ret.1W	ret.s0
 200	rmi.1W	sar.1W	sbf.s0	sbi.1W	sbi.2W	sdl.w\-1	set.s0	sil.w\-1
 208	sil.w0	sli.1W	ste.lw	ste.w0	ste.w1	ste.w2	stf.l	stf.W
 216	stf.2W	stf.s0	sti.1	sti.1W	sti.2W	sti.3W	sti.4W	sti.s0
 224	stl.pw	stl.nw	stl.0	stl.1W	stl.\-1W	stl.\-2W	stl.\-3W	stl.\-4W
 232	stl.\-5W	stl.w\-1	teq.z	tgt.z	tlt.z	tne.z	zeq.l	zeq.s0
 240	zeq.s1	zer.s0	zge.s0	zgt.s0	zle.s0	zlt.s0	zne.s0	zne.s\-1
 248	zre.lw	zre.w0	zrl.\-1W	zrl.\-2W	zrl.w\-1	zrl.nw	escape1	escape2
.DE 2
The list of secondary opcodes (escape1):
.N	1
.DS	B
.ta 7n 16n 25n 34n 43n 52n 61n 70n
   0	aar.l	aar.z	adf.l	adf.z	adi.l	adi.z	ads.l	ads.z
   8	adu.l	adu.z	and.l	and.z	asp.lw	ass.l	ass.z	bge.l
  16	bgt.l	ble.l	blm.l	bls.l	bls.z	blt.l	bne.l	cai.z
  24	cal.l	cfi.z	cfu.z	ciu.z	cmf.l	cmf.z	cmi.l	cmi.z
  32	cms.l	cms.z	cmu.l	cmu.z	com.l	com.z	csa.l	csa.z
  40	csb.l	csb.z	cuf.z	cui.z	cuu.z	dee.lw	del.pw	del.nw
  48	dup.l	dus.l	dus.z	dvf.l	dvf.z	dvi.l	dvi.z	dvu.l
  56	dvu.z	fef.l	fef.z	fif.l	fif.z	inl.pw	inl.nw	inn.l
  64	inn.z	ior.l	ior.z	lar.l	lar.z	ldc.l	ldf.l	ldl.pw
  72	ldl.nw	lfr.l	lil.pw	lil.nw	lim.z	los.l	los.z	lor.s0
  80	lpi.l	lxa.l	lxl.l	mlf.l	mlf.z	mli.l	mli.z	mlu.l
  88	mlu.z	mon.z	ngf.l	ngf.z	ngi.l	ngi.z	nop.z	rck.l
  96	rck.z	ret.l	rmi.l	rmi.z	rmu.l	rmu.z	rol.l	rol.z
 104	ror.l	ror.z	rtt.z	sar.l	sar.z	sbf.l	sbf.z	sbi.l
 112	sbi.z	sbs.l	sbs.z	sbu.l	sbu.z	sde.l	sdf.l	sdl.pw
 120	sdl.nw	set.l	set.z	sig.z	sil.pw	sil.nw	sim.z	sli.l
 128	sli.z	slu.l	slu.z	sri.l	sri.z	sru.l	sru.z	sti.l
 136	sts.l	sts.z	str.s0	tge.z	tle.z	trp.z	xor.l	xor.z
 144	zer.l	zer.z	zge.l	zgt.l	zle.l	zlt.l	zne.l	zrf.l
 152	zrf.z	zrl.pw	dch.z	exg.s0	exg.l	exg.z	lpb.z	gto.l
.DE 2
Finally, the list of opcodes with four byte arguments (escape2).
.DS
.ta 7n 16n 25n 34n 43n 52n 61n 70n
   0	loc
.DE 0
