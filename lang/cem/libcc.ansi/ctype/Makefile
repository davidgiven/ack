clean:
	rm -f isalnum.o isalpha.o iscntrl.o isdigit.o isgraph.o \
		islower.o isprint.o ispunct.o isspace.o isupper.o \
		isxdigit.o isascii.o tolower.o toupper.o chartab.o \
		isalnum.c isalpha.c iscntrl.c isdigit.c isgraph.c \
		islower.c isprint.c ispunct.c isspace.c isupper.c \
		isxdigit.c isascii.c tolower.c toupper.c chartab.c \
		OLIST

chartab.c: char.tab
	tabgen -fchar.tab > chartab.c

isalnum.c isalpha.c iscntrl.c isdigit.c isgraph.c islower.c isprint.c \
ispunct.c isspace.c isupper.c isxdigit.c isascii.c tolower.c toupper.c: genfiles
	sh genfiles
