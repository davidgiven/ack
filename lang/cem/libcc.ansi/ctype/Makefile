CFLAGS=-L -LIB

.SUFFIXES: .o .e .c

.e.o:
	$(CC) $(CFLAGS) -c -o $@ $*.e


genfiles: genfiles.c
	$(CC) genfiles.c -o genfiles

chartab.c: char.tab
	tabgen -fchar.tab > chartab.c

isalnum.c isalpha.c iscntrl.c isdigit.c isgraph.c islower.c isprint.c \
ispunct.c isspace.c isupper.c isxdigit.c isascii.c tolower.c toupper.c: genfiles
	genfiles

isalnum.o:
isalpha.o:
iscntrl.o:
isdigit.o:
isgraph.o:
islower.o:
isprint.o:
ispunct.o:
isspace.o:
isupper.o:
isxdigit.o:
isascii.o:
tolower.o:
toupper.o:
chartab.o:
