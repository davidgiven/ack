# $Header$

all:
	-nroff macro.v7 6500_as.1 > 6500_as.opr
	-nroff macro.v7 6800_as.1 > 6800_as.opr
	-nroff macro.v7 6809_as.1 > 6809_as.opr
	-nroff macro.v7 8080_as.1 > 8080_as.opr
	-nroff macro.v7 i86_as.1 > i86_as.opr
	-nroff macro.v7 m68k2_as.1 > m68k2_as.opr
	-nroff macro.v7 m68k_int.1 > m68k_int.opr
	-nroff macro.v7 pdp_as.1 > pdp_as.opr
	-nroff macro.v7 ns_as.1 > ns_as.opr
	-nroff macro.v7 z8000_as.1 > z8000_as.opr
	-nroff macro.v7 z80_as.1 > z80_as.opr
	-tbl macro.v7 ack.1 | nroff >ack.opr
	-nroff macro.v7 arch.1 >arch.1.opr
	-nroff macro.v7 LLgen.1 > LLgen.1.opr
	-nroff macro.v7 arch.5 >arch.5.opr
	-nroff macro.v7 libmon.7 > libmon.opr
	-nroff macro.v7 libpc.7 > libpc.opr
	-nroff macro.v7 cpp.6 > cpp.opr
	-nroff macro.v7 em_ass.6 > em_ass.opr
	-nroff macro.v7 em_decode.6 > em_decode.opr
	-nroff macro.v7 em_opt.6 > em_opt.opr
	-nroff macro.v7 em_pem.6 > em_pem.opr
	-nroff macro.v7 pc_prlib.7 > pc_prlib.opr
	-nroff macro.v7 uni_ass.6 >uni_ass.opr

install:
	-cp 6500_as.1 > /usr/man/man1/6500_as.1
	-cp 6800_as.1 > /usr/man/man1/6800_as.1
	-cp 6809_as.1 > /usr/man/man1/6809_as.1
	-cp 8080_as.1 > /usr/man/man1/8080_as.1
	-cp i86_as.1 > /usr/man/man1/i86_as.1
	-cp m68k2_as.1 > /usr/man/man1/m68k2_as.1
	-cp m68k_int.1 > /usr/man/man1/m68k_int.1
	-cp pdp_as.1 > /usr/man/man1/pdp_as.1
	-cp ns_as.1 > /usr/man/man1/ns_as.1
	-cp z80_as.1 > /usr/man/man1/z80_as.1
	-cp z8000_as.1 > /usr/man/man1/z8000_as.1
	-tbl ack.1 >/usr/man/man1/ack.1
	-cp arch.1 /usr/man/man1/arch.1
	-cp LLgen.1 /usr/man/man1/LLgen.1
	-cp arch.5 /usr/man/man5/arch.5
	-cp libmon.7 /usr/man/man7/em_libmon.7
	-cp libpc.7 /usr/man/man7/em_libpc.7
	-cp cpp.6 /usr/man/man6/cpp.6
	-cp em_ass.6 /usr/man/man6/em_ass.6
	-cp em_decode.6 /usr/man/man6/em_decode.6
	-cp em_opt.6 /usr/man/man6/em_opt.6
	-cp em_pem.6 /usr/man/man6/em_pem.6
	-cp pc_prlib.7 /usr/man/man7/em_pc_prlib.7
	-cp uni_ass.6 /usr/man/man6/uni_ass.6

opr:
	make pr | opr

pr:
	@make all >make.pr.out 2>&1 &
	@cat *.opr

clean:
	-rm -f *.opr
