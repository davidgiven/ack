# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/occam

FILES=	$(SRC_DIR)/p0 \
	$(SRC_DIR)/p1 \
	$(SRC_DIR)/p2 \
	$(SRC_DIR)/p3 \
	$(SRC_DIR)/p4 \
	$(SRC_DIR)/p5 \
	$(SRC_DIR)/p6 \
	$(SRC_DIR)/p7 \
	$(SRC_DIR)/p8 \
	$(SRC_DIR)/p9

PIC=pic
EQN=eqn
TBL=tbl

$(TARGET_HOME)/doc/occam.doc:	$(FILES) channel.h.t channel.c.t
	soelim $(FILES) | $(PIC) | $(TBL) | $(EQN) > $@

channel.h.t:	$(SRC_HOME)/h/ocm_chan.h
	$(SRC_DIR)/ctot <$(SRC_HOME)/h/ocm_chan.h >channel.h.t

channel.c.t:	channel.c
	$(SRC_DIR)/ctot <channel.c >channel.c.t

channel.c:	$(SRC_HOME)/lang/occam/lib/tail_ocm.a
	$(UTIL_HOME)/bin/arch x $(SRC_HOME)/lang/occam/lib/tail_ocm.a channel.c
