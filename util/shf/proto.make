# $Header$

#PARAMS		do not remove this line!

install:
	-rm -f $(TARGET_HOME)/bin/march
	cp $(SRC_HOME)/util/shf/march.sh $(TARGET_HOME)/bin/march
	chmod +w $(TARGET_HOME)/bin/march

clean:
	@echo always clean

lint:
	@echo no C source files here

cmp:
	cmp $(SRC_HOME)/util/shf/march.sh $(TARGET_HOME)/bin/march

opr:
	make pr|opr

pr:
	@pr $(SRC_HOME)/util/shf/march.sh $(SRC_HOME)/util/shf/proto.make
