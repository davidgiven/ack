all:
	-tbl macro.v7 ack.1 | nroff >ack.opr
	-nroff macro.v7 arch.1 >arch.1.opr
	-nroff macro.v7 arch.5 >arch.5.opr
	-nroff macro.v7 libmon.7 > libmon.opr
	-nroff macro.v7 libpc.7 > libpc.opr
	-nroff macro.v7 em_ass.6 > em_ass.opr
	-nroff macro.v7 em_decode.6 > em_decode.opr
	-nroff macro.v7 em_opt.6 > em_opt.opr
	-nroff macro.v7 em_pem.6 > em_pem.opr
	-nroff macro.v7 pc_prlib.7 > pc_prlib.opr
	-nroff macro.v7 uni_ass.6 >uni_ass.opr

install:
	-tbl ack.1 >/usr/man/man1/ack.1
	-cp arch.1 /usr/man/man1/arch.1
	-cp arch.5 /usr/man/man5/arch.5
	-cp libmon.7 /usr/man/man7/em_libmon.7
	-cp libpc.7 /usr/man/man7/em_libpc.7
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
