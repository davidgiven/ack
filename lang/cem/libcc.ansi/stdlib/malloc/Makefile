DIR = .
MALLOC_C = $(DIR)/malloc.c
MALLOC_O = $(DIR)/malloc.o

MALLOCSRC =	READ_ME size_type.h param.h impl.h check.h log.h phys.h \
		mal.c log.c phys.c check.c

$(MALLOC_C):	$(MALLOCSRC) Makefile add_file
		rm -f $(MALLOC_C)
		for i in $(MALLOCSRC) ; do add_file $$i >> $(MALLOC_C) ; done
		rm -f $(MALLOC_O)

pr:
		@pr Makefile add_file $(MALLOCSRC)

opr:
		make pr | opr

clean:
		rm -f *.o malloc.c
