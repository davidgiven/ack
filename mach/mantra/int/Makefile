P = mloop
em2tl:	a.out
	cv a.out em2tl
	rm a.out
a.out:	$P02tl.s $P12tl.s $P22tl.s $P32tl.s $P42tl.s $P52tl.s $P62tl.s $P72tl.s $P82tl.s $P92tl.s $Pa2tl.s $Pb2tl.s $Pc2tl.s $Pe2tl.s
	m68k2 -s $P?2tl.s >symem2tl
$P02tl.s:	$P0
	prep $P0 $P02tl.s
$P12tl.s:	$P1
	prep $P1 $P12tl.s
$P22tl.s:	$P2
	prep $P2 $P22tl.s
$P32tl.s:	$P3
	prep $P3 $P32tl.s
$P42tl.s:	$P4
	prep $P4 $P42tl.s
$P52tl.s:	$P5
	prep $P5 $P52tl.s
$P62tl.s:	$P6
	prep $P6 $P62tl.s
$P72tl.s:	$P7
	prep $P7 $P72tl.s
$P82tl.s:	$P8
	prep $P8 $P82tl.s
$P92tl.s:	$P9
	prep $P9 $P92tl.s
$Pa2tl.s:	$Pa
	prep $Pa $Pa2tl.s
$Pb2tl.s:	$Pb
	prep $Pb $Pb2tl.s
$Pc2tl.s:	$Pc
	prep $Pc $Pc2tl.s
$Pe2tl.s:	$Pe
	prep $Pe $Pe2tl.s

install:	em2tl
	rm -f ../../../bin/em
	cp em2tl ../../../bin/em

cmp:		em2tl
	cmp em2tl ../../../bin/em

clean:
	rm -f em2tl *.s temp.c

opr:
	make pr | opr

pr:
	@pr `pwd`/makefile `pwd`/prep
	@pr `pwd`/header `pwd/deffile `pwd`/$P?
