# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/descr
CPP = $(UTIL_HOME)/lib.bin/cpp
EMH = $(TARGET_HOME)/h

SOURCES = \
	$(SRC_DIR)/i86.descr \
	$(SRC_DIR)/m68k2.descr \
	$(SRC_DIR)/pdp.descr \
	$(SRC_DIR)/vax4.descr \
	$(SRC_DIR)/m68k4.descr \
	$(SRC_DIR)/m68020.descr \
	$(SRC_DIR)/sparc.descr \
	$(SRC_DIR)/em22.descr \
	$(SRC_DIR)/em24.descr \
	$(SRC_DIR)/em44.descr

TARGETS = i86descr m68k2descr vax4descr pdpdescr m68k4descr m68020descr \
	  em22descr em24descr em44descr sparcdescr

PRFILES = $(SRC_DIR)/proto.make $(SRC_DIR)/descr.sed $(SOURCES)

all:	$(TARGETS)

install:	all
	for i in $(TARGETS) ; do cp $$i $(TARGET_HOME)/lib/ego/$$i ; done

cmp:	all
	-for i in $(TARGETS) ; do cmp $$i $(TARGET_HOME)/lib/ego/$$i ; done

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f $(TARGETS) *.o Out out nohup.out

i86descr:	$(SRC_DIR)/i86.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/i86.descr | sed -f $(SRC_DIR)/descr.sed > i86descr

pdpdescr:	$(SRC_DIR)/pdp.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/pdp.descr | sed -f $(SRC_DIR)/descr.sed > pdpdescr

m68k2descr:	$(SRC_DIR)/m68k2.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/m68k2.descr | sed -f $(SRC_DIR)/descr.sed > m68k2descr

m68k4descr:	$(SRC_DIR)/m68k4.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/m68k4.descr | sed -f $(SRC_DIR)/descr.sed > m68k4descr

m68020descr:	$(SRC_DIR)/m68020.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/m68020.descr | sed -f $(SRC_DIR)/descr.sed > m68020descr

vax4descr:	$(SRC_DIR)/vax4.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/vax4.descr | sed -f $(SRC_DIR)/descr.sed > vax4descr

em22descr:	$(SRC_DIR)/em22.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/em22.descr | sed -f $(SRC_DIR)/descr.sed > em22descr

em24descr:	$(SRC_DIR)/em24.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/em24.descr | sed -f $(SRC_DIR)/descr.sed > em24descr

em44descr:	$(SRC_DIR)/em44.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/em44.descr | sed -f $(SRC_DIR)/descr.sed > em44descr

sparcdescr:	$(SRC_DIR)/sparc.descr $(SRC_DIR)/descr.sed
		$(CPP) -P -I$(EMH) $(SRC_DIR)/sparc.descr | sed -f $(SRC_DIR)/descr.sed > sparcdescr

