# $Header$

#PARAMS		do not remove this line!

all:

install:
		
	-mkdir $(TARGET_HOME)/include/tail_ac
	( cd headers; tar cf - `cat LIST` ) | ( cd $(TARGET_HOME)/include/tail_ac ; tar xf - )
