EMHOME=../..
FILES= p0 p1 p2 p3 p4 p5 p6 p7 p8 p9

PIC=pic
EQN=eqn
TBL=tbl
../occam.doc:	p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 channel.h.t channel.c.t
	cat $(FILES) | $(PIC) | $(TBL) | $(EQN) > $@

channel.h.t:	$(EMHOME)/h/ocm_chan.h
	ctot <$(EMHOME)/h/ocm_chan.h >channel.h.t

channel.c.t:	channel.c
	ctot <channel.c >channel.c.t

channel.c:	$(EMHOME)/lang/occam/lib/tail_ocm.a
		arch x tail_ocm.a channel.c
