EM=/proj/em/Work

OFILES = dist.o error.o

IFILES = -I$(EM)/h -I$(EM)/modules/h
CC = cc

.c.o :
	$(CC) $(IFILES) -c $<

em_parser : $(OFILES)
	  touch em_parser
