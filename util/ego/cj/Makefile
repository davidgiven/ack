
EMH=../../../h
EML=../../../lib
SHARE=../share
OBJECTS=cj.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o $(SHARE)/stack_chg.o $(SHARE)/go.o
MSHOBJECTS=$(SHARE)/get.m $(SHARE)/put.m $(SHARE)/alloc.m $(SHARE)/global.m $(SHARE)/debug.m $(SHARE)/files.m $(SHARE)/map.m $(SHARE)/lset.m $(SHARE)/cset.m $(SHARE)/aux.m $(SHARE)/stack_chg.m
SRC=cj.c
.SUFFIXES: .m

.c.o:
	cc $(CFLAGS) -c $<
.c.m:
	ack -O -L -c.m $(CFLAGS) $<
all:	$(OBJECTS)
cj: \
	$(OBJECTS) $(SHOBJECTS)
	 cc -o cj -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

